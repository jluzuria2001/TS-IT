/**
 * \mainpage A DTN implementation for Contiki.
 * 
 * This is a DTN (Delay-Tolerant Networking) implementation over
 * the Contiki operating system.
 * 
 * \defgroup dtn Delay-Tolerant Networking
 * 
 * DTN implementation for Contiki
 * 
 * @{
 * \example example-dtn.c
 * 
 * \section channels Channels
 *     The DTN module uses 3 channels (for spray, request and hand-off).
 * 
 * \file
 *     Header file for the \ref dtn module
 * \author
 *     Yiwei Chen <yiwei.chen.13@ucl.ac.uk>
 */
#ifndef DTN_H
#define DTN_H
#include "net/rime/rime.h"

#define DTN_VERSION 1
#define DTN_L_COPIES 8
#define DTN_QUEUE_MAX 5
#define DTN_MAX_LIFETIME 60
#define DTN_SPRAY_DELAY 5
#define DTN_RTX 3
#define DTN_HANDOFF_NUM_HISTORY_ENTRIES 4

#define DTN_POWER_MAX 0x12
#define DTN_POWER_MIN 0x00

struct dtn_conn;
struct dtn_hdr;

/** Callbacks structure for \ref dtn "DTN" */
struct dtn_callbacks {
  /** Called when receiving a message from the \ref dtn "DTN" connections */
  void (* recv)(struct dtn_conn *c, const linkaddr_t *from, uint16_t packetid);
};

/** Structure for \ref dtn "DTN" message header */
struct dtn_hdr {
  uint8_t version;                /**< DTN protocol version */
  uint8_t magic[2];               /**< magic bytes defined in DTN protocol */
  uint16_t num_copies;            /**< Number of copies (The L value) */
  linkaddr_t esender;             /**< Origin's address */
  linkaddr_t ereceiver;           /**< Destination's address */
  uint16_t epacketid;             /**< Message's sequence number */
};

/** Representation of a \ref dtn "DTN" connections */
struct dtn_conn {
  struct broadcast_conn spray_c;  /**< The broadcast connection for Spray */
  struct unicast_conn request_c;  /**< The unicast connection for Request */
  struct runicast_conn handoff_c; /**< The runicast connection for Hand-Off */
  const struct dtn_callbacks *cb; /**< Pointer to the callbacks structure */
  struct packetqueue *q;          /**< DTN packet queue */
  uint16_t seqno;                 /**< Current sequence number for messages */
  struct ctimer spray_ct;         /**< Timer for Spray */
  struct dtn_hdr *handoff_qb;     /**< Pointer to queue buffer of the message
                                       currently being hand-offed */
  struct dtn_hdr handoff_hdr;     /**< Header of the message currently being
                                       hand-offed */
};

/**
 * Open a DTN connection
 * \param c
 *     Pointer to a struct \ref dtn_conn representing the \ref dtn "DTN"
 *     connection to open.
 * \param dtn_channel
 *     Channel number to use for the spray phase in the \ref dtn "DTN"
 *     connection. Note that its following two channels will also be used for
 *     the request and hand-off phases.
 * \param cb
 *     Pointer to a struct \ref dtn_callbacks.
 * \sa dtn_close, dtn_send
 */
void dtn_open(struct dtn_conn *c, uint16_t dtn_channel,
              const struct dtn_callbacks *cb);

/**
 * Close a DTN connection
 * \param c
 *     Pointer to a struct \ref dtn_conn representing the DTN connection to
 *     close.
 * \sa dtn_open, dtn_send
 */
void dtn_close(struct dtn_conn *c);

/**
 * Send the data in the packet buffer over a DTN connection.
 * \param c
 *     Pointer to a struct \ref dtn_conn representing the DTN connection to
 *     send over.
 * \param to
 *     Pointer to the Rime address of message destination.
 * \retval
 *     Non-Zero if successfully sent
 * \retval
 *     Zero if failed.
 * \sa dtn_open, dtn_close
 */
int dtn_send(struct dtn_conn *c, const linkaddr_t *to);

/**
 * Change the radio power level.
 * \param power
 *     The power level.
 */
void dtn_set_power(uint8_t power);

/**
 * Change the local Rime address.
 * \param addr
 *     Pointer to the Rime address to set as local address.
 */
void dtn_set_addr(linkaddr_t *addr);

#endif
/** }@ */

