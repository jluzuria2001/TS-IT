/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 * \brief
 *     An example of using the Delay-Tolerant Networking module
 * \author
 *     Yiwei Chen <yiwei.chen.13@ucl.ac.uk>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#define ENABLE_LOGGING 1
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "net/rime/rime.h"
#include "dtn.h"

#define FLASH_LED(l) {leds_on(l); clock_delay_msec(50); leds_off(l); clock_delay_msec(50);}

#define DTN_CHANNEL 128
#define button_sensor2 button_sensor

void
dtn_recv(struct dtn_conn *c, const linkaddr_t *from, uint16_t packetid)
{
  printf("[DTN] Message(id: %d) received from %02x:%02x: '%s'\n",
         packetid, from->u8[1], from->u8[0],
         (char *)packetbuf_dataptr());
}

const struct dtn_callbacks dtn_call = {dtn_recv};

PROCESS(main_process, "DTN Example Process");
AUTOSTART_PROCESSES(&main_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{
  static struct dtn_conn conn;
  static char msg[100];
  static int count = 0;
  static linkaddr_t my_addr, dest_addr;
  static uint8_t power = DTN_POWER_MIN;
  
  PROCESS_EXITHANDLER(dtn_close(&conn););
  PROCESS_BEGIN();

  printf("Begin\n");
	
  linkaddr_copy(&my_addr, &linkaddr_null);
  	
  my_addr.u8[0] = 15;
  dtn_set_addr(&my_addr);
  
  printf("my address %02x\n", my_addr.u8[0]);
  	
  linkaddr_copy(&dest_addr, &linkaddr_null);
  dest_addr.u8[0] = 9;
  
  printf("dest address %02x\n", dest_addr.u8[0]);	
	
  //dtn_set_power(power);

  printf("Power %i", power);	
  //printf("Channel %d", DTN_CHANNEL);
	
  dtn_open(&conn, DTN_CHANNEL, &dtn_call);
  
  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == sensors_event && data == &button_sensor) {
      count++;
      sprintf(msg, "YiweiChen-%d", count);
      printf("[DTN] Sending DTN message: %s\n", msg);
      packetbuf_copyfrom(msg, strlen(msg) + 1);
      if (dtn_send(&conn, &dest_addr)) {
        FLASH_LED(LEDS_GREEN);
      } else {
        printf("[DTN] Failed sending.\n");
      }
    } else if (ev == sensors_event && data == &button_sensor2) {
      dest_addr.u8[0]++;
      if (dest_addr.u8[0] > 20) dest_addr.u8[0] = 0;
      printf("[DTN] Destination set to %02x:%02x.\n",
             dest_addr.u8[1], dest_addr.u8[0]);
      FLASH_LED(LEDS_BLUE);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

