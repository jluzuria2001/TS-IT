#!/bin/bash

## Script to install mosquitto 1.4

git clone https://github.com/eclipse/mosquitto.git
cd mosquitto
git checkout origin/1.4

## MAKE MANUALLY
## /-THE-/-PATH-/-TO-/
## nano config.mk 
##
## ensure that the options are set to “no”.
## websockets	[ WITH_WEBSOCKETS:=no ]
## SRV lookup 	[ WITH_SRV:=no ]



sudo apt-get install uuid-dev xsltproc docbook-xsl
make
make test
sudo make install
