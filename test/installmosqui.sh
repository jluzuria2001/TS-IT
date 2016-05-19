#!/bin/bash

## Script to install mosquitto 1.4

git clone https://github.com/eclipse/mosquitto.git
cd mosquitto
git checkout origin/1.4

## MAKE MANUALLY
## /-THE-/-PATH-/-TO-/
## nano config.mk 
## ensure that the websockets option is set to “yes”.
## [ WITH_WEBSOCKETS:=yes ]


sudo apt-get install uuid-dev xsltproc docbook-xsl
make
make test
sudo make install
