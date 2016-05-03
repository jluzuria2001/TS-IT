#!/bin/bash
#
# sink.sh example Called from dtntrigger
#
# Print received messages. Does some sanity checks...
#

MAXPREVIEW=250

#First parameter is source EID, second is path to payload
src=$1
payload=$2

toprint=$(head -c $MAXPREVIEW $payload | strings -n 1 -e S)
actualsize=$(wc -c "$payload" | cut -f 1 -d ' ')

echo -n "$src said: $toprint"
#mosquitto_pub -h localhost -t test2 -m "$toprint"

if [ "$actualsize" -gt "$MAXPREVIEW" ]
then
        echo -n "..."
fi
echo " ($actualsize bytes)"
