#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

#include "mqtt-sn.h"

const char *client_id = NULL;
const char *topic_name = NULL;
const char *message_data = NULL;
time_t keep_alive = 30;
const char *mqtt_sn_host = "127.0.0.1";
const char *mqtt_sn_port = MQTT_SN_DEFAULT_PORT;
uint16_t topic_id = 0;
uint8_t topic_id_type = MQTT_SN_TOPIC_TYPE_NORMAL;
int8_t qos = 0;
uint8_t retain = FALSE;
uint8_t debug = 0;


static void usage()
{
    fprintf(stderr, "Usage: mqtt-sn-pub [opts] -t <topic> -m <message>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  -d             Increase debug level by one. -d can occur multiple times.\n");
    fprintf(stderr, "  -h <host>      MQTT-SN host to connect to. Defaults to '%s'.\n", mqtt_sn_host);
    fprintf(stderr, "  -i <clientid>  ID to use for this client. Defaults to 'mqtt-sn-tools-' with process id.\n");
    fprintf(stderr, "  -m <message>   Message payload to send.\n");
    fprintf(stderr, "  -n             Send a null (zero length) message.\n");
    fprintf(stderr, "  -p <port>      Network port to connect to. Defaults to %s.\n", mqtt_sn_port);
    fprintf(stderr, "  -q <qos>       Quality of Service value (0 or -1). Defaults to %d.\n", qos);
    fprintf(stderr, "  -r             Message should be retained.\n");
    fprintf(stderr, "  -t <topic>     MQTT topic name to publish to.\n");
    fprintf(stderr, "  -T <topicid>   Pre-defined MQTT-SN topic ID to publish to.\n");
    fprintf(stderr, "  --fe           Enables Forwarder Encapsulation. Mqtt-sn packets are encapsulated according to MQTT-SN Protocol Specification v1.2, chapter 5.5 Forwarder Encapsulation.\n");
    fprintf(stderr, "  --wlnid        If Forwarder Encapsulation is enabled, wireless node ID for this client. Defaults to process id.\n");
    exit(EXIT_FAILURE);
}

static void parse_opts(int argc, char** argv)
{

    static struct option long_options[] =
    {
        {"fe",    no_argument,       0, 'f' },
        {"wlnid", required_argument, 0, 'w' },
        {0, 0, 0, 0}
    };

    int ch;
    /* getopt_long stores the option index here. */
    int option_index = 0;

    // Missing Parameter?
    if (!(topic_name || topic_id) || !message_data) {
        usage();
    }

    if (qos != -1 && qos != 0) {
        log_err("Only QoS level 0 or -1 is supported.");
        exit(EXIT_FAILURE);
    }

    // Both topic name and topic id?
    if (topic_name && topic_id) {
        log_err("Please provide either a topic id or a topic name, not both.");
        exit(EXIT_FAILURE);
    }

    // Check topic is valid for QoS level -1
    if (qos == -1 && topic_id == 0 && strlen(topic_name) != 2) {
        log_err("Either a pre-defined topic id or a short topic name must be given for QoS -1.");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
    int sock;

    mqtt_sn_disable_frwdencap();

    // Parse the command-line options
    parse_opts(argc, argv);

    // Enable debugging?
    mqtt_sn_set_debug(debug);

    // Create a UDP socket
    sock = mqtt_sn_create_socket(mqtt_sn_host, mqtt_sn_port);
    if (sock) {
        // Connect to gateway
        if (qos >= 0) {
            log_debug("Connecting...");
            mqtt_sn_send_connect(sock, client_id, keep_alive, TRUE);
            mqtt_sn_receive_connack(sock);
        }

        if (topic_id) {
            // Use pre-defined topic ID
            topic_id_type = MQTT_SN_TOPIC_TYPE_PREDEFINED;
        } else if (strlen(topic_name) == 2) {
            // Convert the 2 character topic name into a 2 byte topic id
            topic_id = (topic_name[0] << 8) + topic_name[1];
            topic_id_type = MQTT_SN_TOPIC_TYPE_SHORT;
        } else if (qos >= 0) {
            // Register the topic name
            mqtt_sn_send_register(sock, topic_name);
            topic_id = mqtt_sn_receive_regack(sock);
            topic_id_type = MQTT_SN_TOPIC_TYPE_NORMAL;
        }

        // Publish to the topic
        mqtt_sn_send_publish(sock, topic_id, topic_id_type, message_data, qos, retain);

        // Finally, disconnect
        if (qos >= 0) {
            log_debug("Disconnecting...");
            mqtt_sn_send_disconnect(sock);
            mqtt_sn_receive_disconnect(sock);
        }

        close(sock);
    }

    mqtt_sn_cleanup();

    return 0;
}