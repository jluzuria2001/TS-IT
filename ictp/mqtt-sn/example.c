/*
  Basic MQTT-SN client library
  Copyright (C) 2013 Nicholas Humfrey
  Copyright (C) 2013 Adam Renner
  Copyright (C) 2016 Jorge Luzuriaga
  adapted to work with z1 at 16.04.15
  last modified: 16.04.29
*/
/*
#include <time.h>
#include <sys/time.h>
#include "clock.h"
#include "dev/leds.h"
*/

#include "contiki.h"
#include "mqtt-sn.h"
#include "simple-udp.h"

#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"

#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/rime/rime.h"

#include <stdio.h>
#include <string.h>

//#include "dev/adxl345.h"
//#include "dev/i2cmaster.h"
#include "dev/tmp102.h"
//#include "dev/button-sensor.h"

#define UDP_PORT 20000

#define REQUEST_RETRIES 4
#define DEFAULT_SEND_INTERVAL (1 * CLOCK_SECOND)
//#define REPLY_TIMEOUT (3 * CLOCK_SECOND)
#define REPLY_TIMEOUT (10 * CLOCK_SECOND)

static struct 	mqtt_sn_connection mqtt_sn_c;

static char 	device_id[17];
static char   	*mqtt_client_id	= "zolertia-z1-node";
static char 	ctrl_topic[22] 	= "0000000000000000/ctrl\0";  //of form "0011223344556677/ctrl" it is null terminated, and is 21 charactes
static char 	pub_topic[21]  	= "0000000000000000/msg\0";

static uint16_t ctrl_topic_id;
static uint16_t publisher_topic_id;
static uint16_t ctrl_topic_msg_id;
static uint16_t reg_topic_msg_id;
static uint16_t mqtt_keep_alive=21;

static int8_t 	qos = 0;
static uint8_t 	retain = FALSE;

static clock_time_t 	send_interval;
static publish_packet_t incoming_packet;
static mqtt_sn_subscribe_request subreq;
static mqtt_sn_register_request  regreq;

uint8_t debug = TRUE;

static enum mqttsn_connection_status connection_state = MQTTSN_DISCONNECTED;

struct test_msg{
	uint16_t 	seq_num;
	uint8_t 	node_id;
	uint8_t 	value1;
	rtimer_clock_t 	sender_timestamp;
	clock_time_t 	clock_timestamp;
};


/*A few events for managing device state*/
static process_event_t mqttsn_connack_event;

PROCESS(example_mqttsn_process, "Configure Connection and Topic Registration");
PROCESS(publish_process, "register topic and publish data");
PROCESS(ctrl_subscription_process, "subscribe to a device control channel");

AUTOSTART_PROCESSES(&example_mqttsn_process);



/*---------------------------------------------------------------------------*/
static void
puback_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen)
{
  printf("Puback received\n");
}

/*---------------------------------------------------------------------------*/
static void
connack_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen)
{
  uint8_t connack_return_code;
  connack_return_code = *(data + 3);
  printf("Connack received\n");
  if (connack_return_code == ACCEPTED) {
    process_post(&example_mqttsn_process, mqttsn_connack_event, NULL);
  } else {
    printf("Connack error: %s\n", mqtt_sn_return_code_string(connack_return_code));
  }
}

/*---------------------------------------------------------------------------*/
static void
regack_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen)
{
  regack_packet_t incoming_regack;
  memcpy(&incoming_regack, data, datalen);
  printf("Regack received\n");
  if (incoming_regack.message_id == reg_topic_msg_id) {
    if (incoming_regack.return_code == ACCEPTED) {
      publisher_topic_id = uip_htons(incoming_regack.topic_id);
    } else {
      printf("Regack error: %s\n", mqtt_sn_return_code_string(incoming_regack.return_code));
    }
  }
}

/*---------------------------------------------------------------------------*/
static void
suback_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen)
{
  suback_packet_t incoming_suback;
  memcpy(&incoming_suback, data, datalen);
  printf("Suback received\n");
  if (incoming_suback.message_id == ctrl_topic_msg_id) {
    if (incoming_suback.return_code == ACCEPTED) {
      ctrl_topic_id = uip_htons(incoming_suback.topic_id);
    } else {
      printf("Suback error: %s\n", mqtt_sn_return_code_string(incoming_suback.return_code));
    }
  }
}

/*---------------------------------------------------------------------------*/
static void
publish_receiver(struct mqtt_sn_connection *mqc, const uip_ipaddr_t *source_addr, const uint8_t *data, uint16_t datalen)
{
  memcpy(&incoming_packet, data, datalen);
  printf("Published message received\n");
  //see if this message corresponds to ctrl channel subscription request
  if (uip_htons(incoming_packet.topic_id) == ctrl_topic_id) {
    //the new message interval will be read from the first byte of the recieved packet
    send_interval = (uint8_t)incoming_packet.data[0] * CLOCK_CONF_SECOND;
	printf("data: %s\n", incoming_packet.data);
  } else {
    printf("unknown publication received\n");
  }
}

/*---------------------------------------------------------------------------*/
/*Add callbacks here if we make them*/
static const struct mqtt_sn_callbacks mqtt_sn_call = {
  publish_receiver,
  NULL,
  NULL,
  connack_receiver,
  regack_receiver,
  puback_receiver,
  suback_receiver,
  NULL,
  NULL
  };


/*---------------------------------------------------------------------------*/
/*this process will publish data at regular intervals*/
PROCESS_THREAD(publish_process, ev, data)
{
  static uint8_t registration_tries;
  static struct  etimer send_timer;
  static uint8_t buf_len;
  static uint16_t message_number;
  static char    buf[20];
  static mqtt_sn_register_request *rreq = &regreq;

  struct test_msg msg;

  PROCESS_BEGIN();
  send_interval = DEFAULT_SEND_INTERVAL;
  memcpy(pub_topic, pub_topic, 16);
  printf("registering topic\n");
  registration_tries = 0;

  while (registration_tries < 4)
  {
    reg_topic_msg_id = mqtt_sn_register_try(rreq, &mqtt_sn_c, pub_topic, REPLY_TIMEOUT);
    PROCESS_WAIT_EVENT_UNTIL(mqtt_sn_request_returned(rreq));
    if (mqtt_sn_request_success(rreq)) {
      registration_tries = 4;
      printf("registration acked\n");
    }
    else {
      registration_tries++;
      if (rreq->state == MQTTSN_REQUEST_FAILED) {
          printf("Regack error: %s\n", mqtt_sn_return_code_string(rreq->return_code));
      }
    }
  }

  if (mqtt_sn_request_success(rreq)){
    //start topic publishing to topic at regular intervals
    etimer_set(&send_timer, send_interval);
    while(message_number<1800)
    {

	  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));

	  //prepare the package
	  msg.seq_num = message_number;
	  msg.node_id = 0xAB;
	  msg.value1  = tmp102.value(TMP102_READ);
	  msg.value1  = tmp102_read_temp_x100();
	  msg.clock_timestamp = (unsigned long)clock_time();
	  //msg.sender_timestamp = (unsigned int)RTIMER_NOW();

  	  
      /* Print the sensor data */
      printf("nodeID: %u, temp: %u, counter: %u, ts1: %lu\n",
                  msg.node_id, msg.value1, msg.seq_num, msg.clock_timestamp);

      sprintf(buf, "%u, %u, %u, %lu",
                    msg.node_id, msg.value1, msg.seq_num, msg.clock_timestamp);

  //  printf("publishing Msg %d \n", message_number);
  //  sprintf(buf, "Message: %d Node: %s", message_number, device_id);

      message_number++;
			
      buf_len = strlen(buf);

	  mqtt_sn_send_publish(&mqtt_sn_c, publisher_topic_id, MQTT_SN_TOPIC_TYPE_NORMAL, buf, buf_len, qos, retain);
		etimer_set(&send_timer, send_interval);
    }
  } else {
    printf("unable to register topic\n");
  }

  PROCESS_END();
}


/*---------------------------------------------------------------------------*/
/*this process will create a subscription and monitor for incoming traffic*/
PROCESS_THREAD(ctrl_subscription_process, ev, data)
{
  static uint8_t subscription_tries;
  static mqtt_sn_subscribe_request *sreq = &subreq;
  PROCESS_BEGIN();
  subscription_tries = 0;
  memcpy(ctrl_topic,ctrl_topic,16);
  printf("requesting subscription\n");

  while(subscription_tries < 4) {
                            //subscribe.request, mqtt_connection, topic_name, qos, time_out
    ctrl_topic_msg_id = mqtt_sn_subscribe_try(sreq, &mqtt_sn_c, ctrl_topic, 0, REPLY_TIMEOUT);
    PROCESS_WAIT_EVENT_UNTIL(mqtt_sn_request_returned(sreq));

    if (mqtt_sn_request_success(sreq)) {
      subscription_tries = 4;
      printf("subscription acked\n");
    }
    else {
      subscription_tries++;
      if (sreq->state == MQTTSN_REQUEST_FAILED) {
          printf("Suback error: %s\n", mqtt_sn_return_code_string(sreq->return_code));
      }
    }
  }
  PROCESS_END();
}


/*---------------------------------------------------------------------------*/
/*this main process will create connection and register topics*/
/*---------------------------------------------------------------------------*/

static struct ctimer connection_timer;
static process_event_t connection_timeout_event;

static void connection_timer_callback(void *mqc)
{
  process_post(&example_mqttsn_process, connection_timeout_event, NULL);
}


PROCESS_THREAD(example_mqttsn_process, ev, data)
{
  static struct etimer periodic_timer;
  static uip_ipaddr_t broker_addr;
  static uint8_t connection_retries = 0;

  PROCESS_BEGIN();

  mqttsn_connack_event = process_alloc_event();

//  mqtt_sn_set_debug(1);

  tmp102_init();
  uip_ip6addr(&broker_addr, 0x2001, 0x760, 0x2e0b, 0x1724, 0, 0, 0, 1);

  mqtt_sn_create_socket(&mqtt_sn_c,UDP_PORT, &broker_addr, UDP_PORT);
  (&mqtt_sn_c)->mc = &mqtt_sn_call;

  	
  sprintf(device_id, "%02X%02X%02X%02X%02X%02X%02X%02X\n",
	linkaddr_node_addr.u8[0],
        linkaddr_node_addr.u8[1],
	linkaddr_node_addr.u8[2],
	linkaddr_node_addr.u8[3],
        linkaddr_node_addr.u8[4],
	linkaddr_node_addr.u8[5],
	linkaddr_node_addr.u8[6],
        linkaddr_node_addr.u8[7]);
	
 if(strcmp(device_id,"0") == 0) {
	printf("Node id not set\n");
  } else {
	 printf("Node id is set to %s.\n", device_id);
  }

	
  /*Wait a little to let system get set*/
  etimer_set(&periodic_timer, 17*CLOCK_SECOND);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

  /*Request a connection and wait for connack*/
  printf("requesting connection \n ");
  connection_timeout_event = process_alloc_event();
  ctimer_set (&connection_timer, REPLY_TIMEOUT, connection_timer_callback, NULL);
  mqtt_sn_send_connect(&mqtt_sn_c, mqtt_client_id, mqtt_keep_alive);
  connection_state = MQTTSN_WAITING_CONNACK;

  while (connection_retries < 4)
  {
    PROCESS_WAIT_EVENT();
    if (ev == mqttsn_connack_event) {
      //if success
      printf("connection acked\n");
      ctimer_stop(&connection_timer);
      connection_state = MQTTSN_CONNECTED;
      connection_retries = 4; //using break here may mess up switch statement of proces
    }
    if (ev == connection_timeout_event) {
      connection_state = MQTTSN_CONNECTION_FAILED;
      connection_retries++;
      printf("connection timeout\n");
      ctimer_restart(&connection_timer);
      if (connection_retries < 4) {
        mqtt_sn_send_connect(&mqtt_sn_c,mqtt_client_id,mqtt_keep_alive);
        connection_state = MQTTSN_WAITING_CONNACK;
      }
    }
  }
  ctimer_stop(&connection_timer);

  if (connection_state == MQTTSN_CONNECTED){

	//INIT THE SUBSCRITION
	process_start(&ctrl_subscription_process, 0);
    etimer_set(&periodic_timer, 5*CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

	//INIT THE PUBLICATION
	process_start(&publish_process, 0);

    //monitor connection
    while(1)
    {
      PROCESS_WAIT_EVENT();
    }
  } else {
    printf("unable to connect\n");
  }


  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
