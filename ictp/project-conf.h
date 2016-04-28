#ifndef PROJECT_ROUTER_CONF_H_
#define PROJECT_ROUTER_CONF_H_

/* Comment this out to use Radio Duty Cycle (RDC) and save energy */
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC          nullrdc_driver

#undef RF_CHANNEL
#define RF_CHANNEL	           15

#undef CC2420_CONF_CHANNEL
#define CC2420_CONF_CHANNEL        15

#undef IEEE802154_CONF_PANID
#define IEEE802154_CONF_PANID      0x0110

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          4
#endif

#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE       256

#ifndef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW    60
#endif

#ifndef WEBSERVER_CONF_CFS_CONNS
#define WEBSERVER_CONF_CFS_CONNS   2
#endif

#endif /* PROJECT_ROUTER_CONF_H_ */
