#./bash

#lanch the gateway

cwd=$(pwd)
cd $cwd"/mqtools/Gateway/src/org/eclipse/paho/mqttsn/gateway"
cd  ../../../../../..
java -cp src org.eclipse.paho.mqttsn.gateway.Gateway
