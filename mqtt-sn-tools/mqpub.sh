#!/bin/bash
#--------------------------------------------------------------------
# interactive script to publish MQTT message to a broker
# to lunch it needs a "0" or "1" as options to publish a messages
# to the motes that indicates that must start to publishing or just 
# be subscribed to receive some messages.
# i.e. "./mqpub.sh 0"


#--------------------------------------------------------------------
#--CONFIG--
#host="localhost"
host="192.168.1.48"
#host="192.168.1.58"
#host="2001:760:2e0b:1724::1"
topicP="0000000000000000/msg"
topicS="0000000000000000/ctrl"
port="20000"


case "$1" in
1)
echo "PUBLISHING ->from<- the motes"
./mqtt-sn-pub -h $host -t $topicS -p $port -m "4444"
;;
0)
COUNTER=0
echo "PUBLISHING ->to<- the motes"
while [ $COUNTER -lt 1800 ]; do
	echo Publishing the message $COUNTER
./mqtt-sn-pub -h $host -t $topicS -p $port -m $COUNTER
	sleep 1
	let COUNTER=COUNTER+1
done
./mqtt-sn-pub -h $host -t $topicS -p $port -m "8888"
;;
'help')
echo "Usage: $0 [1 to PUB|0 to SUB]"
;;
esac
