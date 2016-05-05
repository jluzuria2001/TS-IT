#!/bin/bash
#--------------------------------------------------------------------
# interactive script to forwarding a MQTT message to a DTN host
# to lunch it needs the -s or --start options
# i.e. "./mqt2dtn.sh"


#--------------------------------------------------------------------
#--CONFIG--
host="localhost"
topic="0000000000000000/msg"

outfile="backpipe"
pid=$(cat pidfile)

#--------------------------------------------------------------------
#--FINISH--
ctrl_c() {
  echo "Cleaning up..."
  rm -f $outfile
  kill $pid 2>/dev/null
  
  if [[ "$?" -eq "0" ]];
  then
     echo "Exit success"
     exit 0
  else
     exit 1
  fi
}

#--------------------------------------------------------------------
#--LISTEN--

listen(){
echo "listening..."

([ ! -p "$outfile" ]) && mkfifo $outfile

#(mosquitto_sub -h $host -t $topic >$outfile 2>/dev/null) & echo "$!" > pidfile

(mosquitto_sub -h $host -t $topic > $outfile 2>/dev/null) & echo "$!" > pidfile

#{command1 & command2; }          > STDOUT_file 2> STDERR_file

echo $(($(date +%s%N)/1000000))
#echo $a

while read line <$outfile
do
   echo $line
done
}

#--------------------------------------------------------------------
#--HELP--
usage(){
echo "mqt2dtn"
echo "Usage: $0 <-s or --start>"
}

case "$1" in
    -s|--start)
	trap ctrl_c INT
	listen
	;;
    *)
	usage
	exit 1
	;;
esac
