= Placeholder

````
$ ifconfig
eth0      Link encap:Ethernet  HWaddr 00:0c:29:e7:6b:b4
          inet addr:192.168.229.141  Bcast:192.168.229.255  Mask:255.255.255.0
          inet6 addr: 2001:5c0:1508:f300:d828:4fba:a046:d132/64 Scope:Global
          inet6 addr: 2001:5c0:1508:f300:20c:29ff:fee7:6bb4/64 Scope:Global
          inet6 addr: 2001:5c0:1508:f300::1/64 Scope:Global
          inet6 addr: 2803:5840:2:60:d828:4fba:a046:d132/64 Scope:Global
          inet6 addr: 2803:5840:2:60:20c:29ff:fee7:6bb4/64 Scope:Global
          inet6 addr: fe80::20c:29ff:fee7:6bb4/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:4934 errors:0 dropped:0 overruns:0 frame:0
          TX packets:2887 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:3246395 (3.2 MB)  TX bytes:317010 (317.0 KB)
          Interrupt:19 Base address:0x2000 

lo        Link encap:Local Loopback
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          UP LOOPBACK RUNNING  MTU:16436  Metric:1
          RX packets:2249 errors:0 dropped:0 overruns:0 frame:0
          TX packets:2249 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:0 
          RX bytes:208760 (208.7 KB)  TX bytes:208760 (208.7 KB)

tun       Link encap:UNSPEC  HWaddr 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  
          inet6 addr: 2001:5c0:1400:b::3b85/128 Scope:Global
          UP POINTOPOINT RUNNING NOARP MULTICAST  MTU:1280  Metric:1
          RX packets:6 errors:0 dropped:0 overruns:0 frame:0
          TX packets:2 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:500 
          RX bytes:416 (416.0 B)  TX bytes:152 (152.0 B)
````

````
contiki/tools$ sudo ./tunslip6 -t tun01 -s /dev/ttyUSB0 2001:5c0:1508:f301::1/64
********SLIP started on ``/dev/ttyUSB0''
opened tun device ``/dev/tun01''
ifconfig tun01 inet `hostname` mtu 1500 up
ifconfig tun01 add 2001:5c0:1508:f301::1/64
ifconfig tun01 add fe80::4c0:1508:f301:1/64
ifconfig tun01

tun01     Link encap:UNSPEC  HWaddr 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  
          inet addr:127.0.1.1  P-t-P:127.0.1.1  Mask:255.255.255.255
          inet6 addr: fe80::4c0:1508:f301:1/64 Scope:Link
          inet6 addr: 2001:5c0:1508:f301::1/64 Scope:Global
          UP POINTOPOINT RUNNING NOARP MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:500 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

Rime started with address 193.12.0.0.0.0.19.194
MAC c1:0c:00:00:00:00:13:c2 Ref ID: 5064
Contiki-3.x-2162-g709d3d5 started. Node id is set to 5058.
CSMA nullrdc, channel check rate 128 Hz, radio channel 26
Tentative link-local IPv6 address fe80:0000:0000:0000:c30c:0000:0000:13c2
Starting 'Border router process' 'Web server'd
*** Address:2001:5c0:1508:f301::1 => 2001:05c0:1508:f301
Got configuration message of type P
Setting prefix 2001:5c0:1508:f301::
Server IPv6 addresses:
 2001:5c0:1508:f301:c30c::13c2
 fe80::c30c:0:0:13c2
````

````
Node id is not set, using Z1 product ID
Rime started with address 193.12.0.0.0.0.19.200
MAC c1:0c:00:00:00:00:13:c8 Ref ID: 5064
Contiki-3.x-2162-g9ea4ec6 started. Node id is set to 5064.
CSMA nullrdc, channel check rate 128 Hz, radio channel 26
Tentative link-local IPv6 address fe80:0000:0000:0000:c30c:0000:0000:13c8
Starting 'MQTT Demo'
MQTT Demo Process
Subscription topic zolertia/cmd/leds
Init
Registered. Connect attempt 1
Connecting (1)
APP - Application has a MQTT connection
APP - Subscribing!
APP - Application is subscribed to topic successfully
Publishing
APP - Publish to zolertia/evt/status
````


````
$ python mqtt_client.py 
connecting to test.mosquitto.org
Connected with result code 0
Subscribed to zolertia/evt/status
Subscribed to zolertia/cmd/leds
zolertia/evt/status {"d":{"myName":"Zolertia Z1 Node","Seq #":3,"Uptime (sec)":141,"Def Route":"fe80::c30c:0:0:13c2"}}
zolertia/evt/status {"d":{"myName":"Zolertia Z1 Node","Seq #":4,"Uptime (sec)":186,"Def Route":"fe80::c30c:0:0:13c2"}}

````


mosquitto_pub -h "test.mosquitto.org"  -t "zolertia/cmd/led" -m "1" -q 1 -r

