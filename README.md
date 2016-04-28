# TS-IT

MQTT-SN using Z1 boards with Contiki Operative System,

* Gateway (to translate the mqtt-sn messages to mqtt)
* ictp (the firmware to the Z1 motes)
* mqtt-sn-tools (software to test the mqtt-SN from a Linux system)

To prepare a physical scenario we need at least:

* 2 Zolertia Z1 motes
* 1 Raspberry Pi

To deploy the scenario: (Launch in different terminals in the next order)

* installconti (install contiki)
* mqttsnlunchbroker (lunch mqttbroker)
* mqttsnlunchgateway (lunch the java gateway)
* mqtest1 (lunch a subscriber)
* mqtest2 (lunch a publisher)