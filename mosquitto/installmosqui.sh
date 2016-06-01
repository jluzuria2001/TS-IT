#!/bin/bash

## Script to install mosquitto 1.4

## installing prerequisites
sudo apt-get -y install libssl-dev uuid-dev xsltproc docbook-xsl

## installing mosquitto
git clone https://github.com/eclipse/mosquitto.git
cd mosquitto
git checkout origin/1.4
cd ..
mv client/config.mk mosquitto/

## MAKE MANUALLY
## /-THE-/-PATH-/-TO-/
## nano config.mk 
##
## ensure that the options are set to “no”.
## websockets	[ WITH_WEBSOCKETS:=no ]
## SRV lookup 	[ WITH_SRV:=no ]

cd mosquitto
echo "-----------------TIME  TO  COMPILE ----------------------"
make
echo "----------------TIME TO RUN THE TEST---------------------"
make test
echo "-----------------TIME  TO  INSTALL ----------------------"
sudo make install



