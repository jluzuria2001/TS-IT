The modified clients of mosquitto.

After the installation of mosquitto 1.4 from the sources,
this folder must be replaced on the mosquitto folder instalation to use our clients.


Subscriber
------------------

The modified client print a timestamp in a place of showing the topic name 
when we use the verbosely option (-v) of the client.

To get received messages printed in the following fashion:
##our timestamp, received message 
1463597697 ,22, 2943, 0, 2181
1463597698 ,33, 2912, 0, 2169
1463597699 ,33, 2912, 1, 2343
1463597700 ,44, 2931, 0, 2169

To be used in a command like:
./mosquitto_sub -h $host -t $topic -v 


License
-------

[MIT License].

[MIT License]: http://opensource.org/licenses/MIT
