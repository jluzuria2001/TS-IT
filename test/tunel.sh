#!/bin/bash

#cd "/home/jorg/Desktop/contiki/alignan2/examples/ictp/border-router"
cd "/home/pi/ictp2016/examples/ictp/border-router"

pwd
make connect-router PREFIX=2001:760:2e0b:1721::1/64 MOTES=/dev/ttyUSB0
