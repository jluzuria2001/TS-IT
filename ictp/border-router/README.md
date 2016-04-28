= Placeholder

````
make border-router.upload && make connect-router
````

````
using saved target 'z1'
sudo ../../../../../tools/tunslip6 aaaa::1/64
[sudo] password for zolertia: 
********SLIP started on ``/dev/ttyUSB0''
opened tun device ``/dev/tun0''
ifconfig tun0 inet `hostname` mtu 1500 up
ifconfig tun0 add aaaa::1/64
ifconfig tun0 add fe80::0:0:0:1/64
ifconfig tun0

tun0      Link encap:UNSPEC  HWaddr 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  
          inet addr:127.0.1.1  P-t-P:127.0.1.1  Mask:255.255.255.255
          inet6 addr: fe80::1/64 Scope:Link
          inet6 addr: aaaa::1/64 Scope:Global
          UP POINTOPOINT RUNNING NOARP MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:500 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

*** Address:aaaa::1 => aaaa:0000:0000:0000
Got configuration message of type P
Setting prefix aaaa::
Server IPv6 addresses:
 aaaa::c30c:0:0:13c2
 fe80::c30c:0:0:13c2
````

Verify

````
$ ping6 aaaa::c30c:0:0:13c2
PING aaaa::c30c:0:0:13c2(aaaa::c30c:0:0:13c2) 56 data bytes
64 bytes from aaaa::c30c:0:0:13c2: icmp_seq=1 ttl=64 time=19.8 ms
64 bytes from aaaa::c30c:0:0:13c2: icmp_seq=2 ttl=64 time=20.3 ms
64 bytes from aaaa::c30c:0:0:13c2: icmp_seq=3 ttl=64 time=20.5 ms
64 bytes from aaaa::c30c:0:0:13c2: icmp_seq=4 ttl=64 time=20.8 ms
64 bytes from aaaa::c30c:0:0:13c2: icmp_seq=5 ttl=64 time=20.6 ms
64 bytes from aaaa::c30c:0:0:13c2: icmp_seq=6 ttl=64 time=20.6 ms
````
