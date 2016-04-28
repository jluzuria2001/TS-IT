= Placeholder

== Requirements

pip install paho-mqtt

Border Router

````
Node id is not set, using Z1 product ID
Rime started with address 193.12.0.0.0.0.19.200
MAC c1:0c:00:00:00:00:13:c8 Ref ID: 5064
Contiki-3.x-2162-g709d3d5 started. Node id is set to 5064.
CSMA nullrdc, channel check rate 128 Hz, radio channel 26
Tentative link-local IPv6 address fe80:0000:0000:0000:c30c:0000:0000:13c8
Starting 'UDP client example process'
UDP client process started
Server address: aaaa::1
Client IPv6 addresses:
fe80::c30c:0:0:13c8
Created a connection with the server :: local/remote port 8765/5678
ID: 171, temp: 2412, x: -211, y: 137, z: -95, batt: 2989, counter: 1
Send readings to 1'
ID: 171, temp: 2412, x: -212, y: 138, z: -94, batt: 2989, counter: 2
Send readings to 1'
ID: 171, temp: 2412, x: -216, y: 138, z: -93, batt: 2990, counter: 3
Send readings to 1'
````

Check

````
$ ping6 aaaa::c30c:0:0:13c8
PING aaaa::c30c:0:0:13c8(aaaa::c30c:0:0:13c8) 56 data bytes
64 bytes from aaaa::c30c:0:0:13c8: icmp_seq=1 ttl=63 time=35.4 ms
64 bytes from aaaa::c30c:0:0:13c8: icmp_seq=2 ttl=63 time=60.2 ms
64 bytes from aaaa::c30c:0:0:13c8: icmp_seq=3 ttl=63 time=35.4 ms
64 bytes from aaaa::c30c:0:0:13c8: icmp_seq=4 ttl=63 time=35.2 ms
````

````
$ python UDPServer.py 
UDP6 server side application V0.1
Started 2016-02-26 09:23:49.673940
UDP server ready: 5678
msg structure size:  13

MQTT: Connected (0) 
2016-02-26 09:23:58 -> aaaa::c30c:0:0:13c8:8765 14
{
  "values": [
    {
      "value": 171, 
      "key": "id"
    }, 
    {
      "value": 0, 
      "key": "counter"
    }, 
    {
      "value": 2320, 
      "key": "temperature"
    }, 
    {
      "value": -135, 
      "key": "x_axis"
    }, 
    {
      "value": 40, 
      "key": "y_axis"
    }, 
    {
      "value": -120, 
      "key": "z_axis"
    }, 
    {
      "value": 2981, 
      "key": "battery"
    }
  ]
}
MQTT: Publishing to {0}... 0 (171)
Sending reply to aaaa::c30c:0:0:13c8
MQTT: Published 2
````

````
$ python mqtt_client.py 
connecting to iot.eclipse.org
Connected with result code 0
Subscribed to v2/zolertia/tutorialthings/#
v2/zolertia/tutorialthings/171 {"values":[{"key": "id", "value": 171},{"key": "counter", "value": 0},{"key": "temperature", "value": 2326},{"key": "x_axis", "value": -129},{"key": "y_axis", "value": 38},{"key": "z_axis", "value": -122},{"key": "battery", "value": 2987}]}
````
