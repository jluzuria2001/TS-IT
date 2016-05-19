#!/bin/bash
#--------------------------------------------------------------------
# interactive script to forwarding a MQTT message to a DTN host
# to lunch it needs the -s or --start options
# i.e. "./mqt2dtn.sh"
echo "./mqtt-sn-sub -h localhost -t "0000000000000000/msg" -p 20000 -V"


#--------------------------------------------------------------------
#--CONFIG--
#host="localhost"
host="192.168.1.48"
topic="0000000000000000/msg"
port="20000"

#outfile="backpipe"
#pid=$(cat pidfile)


#(mosquitto_sub -h $host -t $topic > $outfile 2>/dev/null) & echo "$!" > pidfile
#(bash mqtt-sn-sub -h $host -t $topic -p $port > $outfile 2>/dev/null) & echo "$!" > pidfile

./mosquitto_sub -h $host -t $topic -v > receiver.log 2>/dev/null
