#!/bin/bash
#--------------------------------------------------------------------
# mosquitto subscriber that will be suscribed to a topic 
# and all the messages received are storages in a logfile called
# "receiver.log"
# we call it as i.e. "./mqsub-log.sh"

#--------------------------------------------------------------------
#--CONFIG--
host="localhost"
#host="192.168.1.48"
topic="0000000000000000/msg"
port="20000"

#---move-and-locate
#cd $HOME/mqtools/mosquitto/client/
#pwd
#mv mqsub-log.sh $HOME/mqtools/mosquitto/mosquitto/client/
#cd $HOME/mqtools/mosquitto/mosquitto/client/
pwd

#---execute
#./mosquitto_sub -h $host -t $topic -v > $HOME/receiver.log 2>/dev/null
## recording
#mosquitto_sub -h $host -t $topic -v > $HOME/receiver.log 2>/dev/null
## just view
mosquitto_sub -h $host -t $topic -v
