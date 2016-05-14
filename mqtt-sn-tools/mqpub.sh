#!/bin/bash
#--------------------------------------------------------------------
# interactive script to forwarding a MQTT message to a DTN host
# to lunch it needs the -s or --start options
# i.e. "./mqt2dtn.sh"
#echo "./mqtt-sn-pub -h localhost -t "0000000000000000/ctrl" -p 20000 -m "1""


#--------------------------------------------------------------------
#--CONFIG--
#host="localhost"
host="192.168.1.48"
#host="2001:760:2e0b:1724::1"
topicP="0000000000000000/msg"
topicS="0000000000000000/ctrl"
port="20000"

#outfile="backpipe"
#pid=$(cat pidfile)


#(mosquitto_sub -h $host -t $topic > $outfile 2>/dev/null) & echo "$!" > pidfile
#(bash mqtt-sn-sub -h $host -t $topic -p $port > $outfile 2>/dev/null) & echo "$!" > pidfile

case "$1" in
1)
echo "PUBLISHING ->from<- the motes"
./mqtt-sn-pub -h $host -t $topicS -p $port -m "1"
;;
0)
COUNTER=0
echo "PUBLISHING ->to<- the motes"
while [ $COUNTER -lt 1800 ]; do
	echo Publishing the message $COUNTER
./mqtt-sn-pub -h $host -t $topicS -p $port -m "hello"
	sleep 1
	let COUNTER=COUNTER+1
done
./mqtt-sn-pub -h $host -t $topicS -p $port -m "0"
;;
'help')
echo "Usage: $0 [1 to PUB|0 to SUB]"
;;
esac
