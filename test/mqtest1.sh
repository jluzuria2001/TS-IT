#./bash

#To Test if mosquitto is working:
#mosquitto_sub -t '$SYS/#' -v

#compile and test
cd mqtools/mqtt-sn-tools
make mqtt-sn-sub
make mqtt-sn-pub
#To subscribe:
./mqtt-sn-sub -p 20000 -t test
#To publish:
#./mqtt-sn-pub -p 20000 -t test -m "A message publish using mqtt-sn"
