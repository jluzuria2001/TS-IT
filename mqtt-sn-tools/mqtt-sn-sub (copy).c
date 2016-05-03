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
#include <time.h>

#include "mqtt-sn.h"

const char *client_id = NULL;
const char *topic_name = NULL;
const char *mqtt_sn_host = "127.0.0.1";
const char *mqtt_sn_port = MQTT_SN_DEFAULT_PORT;
uint16_t topic_id = 0;
uint16_t keep_alive = 10;
uint8_t retain = FALSE;
uint8_t debug = 0;
uint8_t single_message = FALSE;
uint8_t clean_session = TRUE;
uint8_t verbose = 0;

uint8_t keep_running = TRUE;

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
    if (!topic_name && !topic_id) {
        usage();
    }

    // Both topic name and topic id?
    if (topic_name && topic_id) {
        log_err("Please provide either a topic id or a topic name, not both.");
        exit(EXIT_FAILURE);
    }
}

static void termination_handler (int signum)
{
    switch(signum) {
    case SIGHUP:
        log_debug("Got hangup signal.");
        break;
    case SIGTERM:
        log_debug("Got termination signal.");
        break;
    case SIGINT:
        log_debug("Got interrupt signal.");
        break;
    }

    // Signal the main thread to stop
    keep_running = FALSE;
}

int main(int argc, char* argv[])
{
    int sock;

    mqtt_sn_disable_frwdencap();

    // Parse the command-line options
    parse_opts(argc, argv);

    // Enable debugging?
    mqtt_sn_set_debug(debug);
    mqtt_sn_set_verbose(verbose);
    mqtt_sn_set_timeout(keep_alive / 2);

    // Setup signal handlers
    signal(SIGTERM, termination_handler);
    signal(SIGINT, termination_handler);
    signal(SIGHUP, termination_handler);

    // Create a UDP socket
    sock = mqtt_sn_create_socket(mqtt_sn_host, mqtt_sn_port);
    if (sock) {
        // Connect to server
        log_debug("Connecting...");
        mqtt_sn_send_connect(sock, client_id, keep_alive, clean_session);
        mqtt_sn_receive_connack(sock);

        // Subscribe to the topic
        if (topic_name) {
            mqtt_sn_send_subscribe_topic_name(sock, topic_name, 0);
        } else {
            mqtt_sn_send_subscribe_topic_id(sock, topic_id, 0);
        }

        // Wait for the subscription acknowledgment
        topic_id = mqtt_sn_receive_suback(sock);
        if (topic_id && topic_name && strlen(topic_name) > 2) {
            mqtt_sn_register_topic(topic_id, topic_name);
        }

        // Keep processing packets until process is terminated
        while(keep_running) {
            publish_packet_t *packet = mqtt_sn_wait_for(MQTT_SN_TYPE_PUBLISH, sock);
            if (packet && single_message) {
                break;
            }
        }

        // Finally, disconnect
        log_debug("Disconnecting...");
        mqtt_sn_send_disconnect(sock);
        mqtt_sn_receive_disconnect(sock);

        close(sock);
    }

    mqtt_sn_cleanup();

    return 0;
}
