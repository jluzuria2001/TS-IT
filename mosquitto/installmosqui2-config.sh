#!/bin/bash

## Script to install our suscriber

cd $HOME/mqtools/mosquitto
pwd
#rm mosquitto/cliente/sub_client.c
#mv client/sub_client.c mosquitto/client/
cd mosquitto
make

echo "--- ready to launch the mqsub-log.sh script"
echo "--- in: " $HOME"/mqtools/mosquitto/client"
echo "--- execute: ./mqsub-log.sh"
