#!/bin/bash
#--------------------------------------------------------------------
# mosquitto subscriber that will be suscribed to a topic 
# and all the messages received are storages in a logfile called
# "receiver.log"
# we call it as i.e. "./mqsub-log.sh"

#--------------------------------------------------------------------
#--CONFIG--
#host="localhost"
host="192.168.1.48"
topic="0000000000000000/msg"
port="20000"

./mosquitto_sub -h $host -t $topic -v > receiver.log 2>/dev/null
