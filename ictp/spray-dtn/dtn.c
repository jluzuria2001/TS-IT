/**
 * \addtogroup dtn
 * @{
 * \file
 * \brief
 *     Source file for the \ref dtn module
 * \author
 *     Yiwei Chen <yiwei.chen.13@ucl.ac.uk>
 */
#include "dtn.h"

#include <string.h>
#include <stddef.h>
#include "net/rime/rime.h"

#define DTN_CSVLOG 0      /**< 0 - Off, 1 - On */
#define DTN_DEBUG_LEVEL 0 /**< 0 - Nothing, 1 - Important only, 2 - ALL */

#if DTN_DEBUG_LEVEL >= 2
#define INFO(...) printf(__VA_ARGS__)
#define INFOADDR(addr) printf("%02x:%02x", (addr)->u8[1], (addr)->u8[0])
#define IMPT(...) printf(__VA_ARGS__)
#define IMPTADDR(addr) printf("%02x:%02x", (addr)->u8[1], (addr)->u8[0])
#elif DTN_DEBUG_LEVEL == 1
#define INFO(...)
#define INFOADDR(addr)
#define IMPT(...) printf(__VA_ARGS__)
#define IMPTADDR(addr) printf("%02x:%02x", (addr)->u8[1], (addr)->u8[0])
#else
#define INFO(...)
#define INFOADDR(addr)
#define IMPT(...)
#define IMPTADDR(addr)
#endif

#if DTN_CSVLOG
#define CSVLOG_PACKBUF(func) csvlog_packetbuf(func)
#else
#define CSVLOG_PACKBUF(func)
#endif

#define DTN_PENDING (void*)(0)
#define DTN_READY (void*)(1)

struct dtn_hdr * dtn_buf_ptr(void);
int dtn_valid_hdr(void);

/*-MESSAGE QUEUE-------------------------------------------------------------*/
PACKETQUEUE(dtn_packetqueue, DTN_QUEUE_MAX);
/*-DEBUG FUNCTIONS-----------------------------------------------------------*/
void
print_packetbuf(char *func)
{
  INFO("%s: tot: %d, ", func, packetbuf_totlen());
  struct dtn_hdr *hdrptr = dtn_buf_ptr();
  INFO("hdr: {l: %d", hdrptr->num_copies);
  INFO(", es: ");
  INFOADDR(&(hdrptr->esender));
  INFO(", er: ");
  INFOADDR(&(hdrptr->ereceiver));
  INFO(", ep: %d}, ", hdrptr->epacketid);
  INFO("data: {len: %d, s: %s}\n",
       packetbuf_totlen() - sizeof(struct dtn_hdr),
       hdrptr + 1);
}
/*---------------------------------------------------------------------------*/
void
csvlog_packetbuf(char *func)
{
  struct dtn_hdr *buf;
  linkaddr_t *addr;
  printf("%s, ", func);
  addr = (linkaddr_t *)packetbuf_addr(PACKETBUF_ADDR_SENDER);
  printf("%02x%02x:%02x%02x, ",
         addr->u8[3], addr->u8[2], addr->u8[1], addr->u8[0]);
  addr = (linkaddr_t *)packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  printf("%02x%02x:%02x%02x, ",
         addr->u8[3], addr->u8[2], addr->u8[1], addr->u8[0]);
  if (dtn_valid_hdr()) {
    buf = dtn_buf_ptr();
    addr = &(buf->esender);
    printf("%02x%02x:%02x%02x, ",
         addr->u8[3], addr->u8[2], addr->u8[1], addr->u8[0]);
    addr = &(buf->ereceiver);
    printf("%02x%02x:%02x%02x, ",
         addr->u8[3], addr->u8[2], addr->u8[1], addr->u8[0]);
    printf("%d, %d\n", buf->epacketid, buf->num_copies);
  } else {
    printf("X, X, X, X\n");
  }
}
/*-LOCAL FUNCTIONS-----------------------------------------------------------*/
void
dtn_delay(void)
{
  clock_delay_usec(random_rand() % 10000 + 10000);
}
/*---------------------------------------------------------------------------*/
struct dtn_hdr *
dtn_buf_ptr(void)
{
  struct dtn_hdr *hdrptr;
  if (packetbuf_hdrlen() == sizeof(struct dtn_hdr)) {
    hdrptr = (struct dtn_hdr *)packetbuf_hdrptr();
  } else {
    hdrptr = (struct dtn_hdr *)packetbuf_dataptr();
  }
  return hdrptr;
}
/*---------------------------------------------------------------------------*/
void
dtn_queue_spray(void *ptr)
{
  struct dtn_conn *c = (struct dtn_conn *)ptr;
  IMPT("dtn_queue_spray: Spraying, queue length: %d\n", packetqueue_len(c->q));
  
  if (packetqueue_len(c->q) <= 0) {
    IMPT("dtn_queue_spray: Empty packetqueue, nothing to spray, stopped.\n");
    return;
  }
  
  struct packetqueue_item *q_item;
  for (q_item = packetqueue_first(c->q); q_item; q_item = q_item->next) {
    if (q_item->ptr != DTN_READY) {
      INFO("dtn_queue_spray: packet still pending, skip.\n");
      continue;
    }
    struct dtn_hdr *qbufdata = (struct dtn_hdr *)
                               queuebuf_dataptr(packetqueue_queuebuf(q_item));
    if (qbufdata == NULL) continue;
    if (qbufdata->num_copies == 0) continue;
    packetbuf_clear();
    queuebuf_to_packetbuf(packetqueue_queuebuf(q_item));
    print_packetbuf("dtn_queue_spray");
    dtn_delay();
    broadcast_send(&c->spray_c);
    CSVLOG_PACKBUF("spray");
    INFO("dtn_queue_spray: broadcast Spray sent.\n");
  }
  
  INFO("dtn_queue_spray: Paused spraying.\n");
  if (ctimer_expired(&c->spray_ct)){
    ctimer_set(&c->spray_ct,
               DTN_SPRAY_DELAY * CLOCK_SECOND,
               dtn_queue_spray, (void *)c);
    INFO("dtn_queue_spray: Timer started.\n");
  }
}
/*---------------------------------------------------------------------------*/
struct packetqueue_item *
dtn_queue_find(struct dtn_conn *c)
{
  struct dtn_hdr *bufdata = dtn_buf_ptr();
  struct packetqueue_item *q_item;
  for (q_item = packetqueue_first(c->q); q_item; q_item = q_item->next) {
    struct dtn_hdr *qbufdata = (struct dtn_hdr *)
                               queuebuf_dataptr(packetqueue_queuebuf(q_item));
    if (qbufdata == NULL) continue;
    if (linkaddr_cmp(&(bufdata->esender), &(qbufdata->esender))
        && bufdata->epacketid == qbufdata->epacketid) {
      return q_item;
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
int
dtn_valid_hdr(void)
{
  struct dtn_hdr *bufdata = dtn_buf_ptr();
  if (bufdata->version == DTN_VERSION
      && bufdata->magic[0] == 'S'
      && bufdata->magic[1] == 'W') {
    return 1;
  } else {
    print_packetbuf("dtn_valid_hdr");
    IMPT("dtn_valid_hdr: packet invalid.\n");
    return 0;
  }
}
/*-SPRAY---------------------------------------------------------------------*/
void
dtn_spray_recv(struct broadcast_conn *b_c, const linkaddr_t *from)
{
  INFO("dtn_spray_recv: broadcast received from %02x:%02x.\n",
       from->u8[1], from->u8[0]);
  if (!dtn_valid_hdr()) return;
  struct dtn_conn *c = (struct dtn_conn *)
                       ((void *)b_c - offsetof(struct dtn_conn, spray_c));
  print_packetbuf("dtn_spray_recv");
  struct dtn_hdr *bufdata = dtn_buf_ptr();
  struct dtn_hdr recv_hdr;
  memcpy(&recv_hdr, bufdata, sizeof (struct dtn_hdr));
  
  if (linkaddr_cmp(&(recv_hdr.esender), &linkaddr_node_addr)) { // from me
    INFO("dtn_spray_recv: Spray message is from me, do nothing.\n");
    return;
  }
  
  if (linkaddr_cmp(&(recv_hdr.ereceiver), &linkaddr_node_addr)) { // to me
    dtn_delay();
    unicast_send(&c->request_c, from);
    CSVLOG_PACKBUF("request");
    IMPT("dtn_spray_recv: unicast Request confirmation sent.\n");
    IMPT("dtn_spray_recv: Spray message is to me, invoking callback.\n");
    packetbuf_hdrreduce(sizeof(struct dtn_hdr));
    c->cb->recv(c, &(recv_hdr.esender), recv_hdr.epacketid);
    return;
  }
  
  if (recv_hdr.num_copies == 1) { // not to me and only one copy
    INFO("dtn_spray_recv: Not to me and L == 1, do nothing.\n");
    return;
  }
  
  struct packetqueue_item * item;
  if (item = dtn_queue_find(c)) { // found in the queue
    if (item->ptr == DTN_READY) {
      INFO("dtn_spray_recv: Spray in the queue and ready, do nothing.\n");
    } else { // still pending
      dtn_delay();
      unicast_send(&c->request_c, from);
      CSVLOG_PACKBUF("request");
      IMPT("dtn_spray_recv: Spray in queue but pending, Request sent to ");
      IMPTADDR(from);
      IMPT(".\n");
    }
    return;
  }
  
  // not in the queue
  bufdata->num_copies = 0;
  if (packetqueue_enqueue_packetbuf(c->q,
                                    DTN_MAX_LIFETIME * CLOCK_SECOND,
                                    DTN_PENDING)) {
    INFO("dtn_spray_recv: Enqueued (pending) successfully.\n");
    dtn_delay();
    unicast_send(&c->request_c, from);
    CSVLOG_PACKBUF("request");
    IMPT("dtn_spray_recv: unicast Request sent to ");
    IMPTADDR(from);
    IMPT(".\n");
  } else {
    IMPT("dtn_spray_recv: Failed to enqueue.\n");
  }
}
/*---------------------------------------------------------------------------*/
const struct broadcast_callbacks dtn_spray_call = {dtn_spray_recv};
/*-REQUEST-------------------------------------------------------------------*/
void
dtn_request_recv(struct unicast_conn *u_c, const linkaddr_t *from)
{
  INFO("dtn_request_recv: unicast received from %02x:%02x\n",
       from->u8[1], from->u8[0]);
  if (!dtn_valid_hdr()) return;
  struct dtn_conn *c = (struct dtn_conn *)
                       ((void *)u_c - offsetof(struct dtn_conn, request_c));
  print_packetbuf("dtn_request_recv");
  
  struct packetqueue_item * q_item;
  if (!(q_item = dtn_queue_find(c))) {
    IMPT("dtn_request_recv: Request not in the queue, do nothing.\n");
    return;
  }
  if (q_item->ptr != DTN_READY) {
    IMPT("dtn_request_recv: Request in queue, but pending, do nothing.\n");
    return;
  }
  INFO("dtn_request_recv: Request found in the queue.\n");
  struct dtn_hdr *qbufdata = (struct dtn_hdr *)
                             queuebuf_dataptr(packetqueue_queuebuf(q_item));
  if (linkaddr_cmp(&(qbufdata->ereceiver), from)) {
    qbufdata->num_copies = 0;
    IMPT("dtn_request_recv: receiver got message, set L to 0.\n");
    return;
  }
  if ((qbufdata->num_copies == 1)
      && !linkaddr_cmp(from, &(qbufdata->ereceiver))) {
    IMPT("dtn_request_recv: L == 1, and from != ereceiver, do nothing.\n");
    return;
  }
  if (qbufdata->num_copies == 0) {
    IMPT("dtn_request_recv: L == 0, do nothing.\n");
    return;
  }
  if (c->handoff_qb != NULL) {
    IMPT("dtn_request_recv: Another HandOff in progress, do nothing.\n");
    return;
  }
  c->handoff_qb = (struct dtn_hdr *)
                  queuebuf_dataptr(packetqueue_queuebuf(q_item));
  memcpy(&(c->handoff_hdr), c->handoff_qb, sizeof(struct dtn_hdr));
  queuebuf_to_packetbuf(packetqueue_queuebuf(q_item));
  struct dtn_hdr *bufdata = dtn_buf_ptr();
  bufdata->num_copies /= 2;
  dtn_delay();
  runicast_send(&c->handoff_c, from, DTN_RTX);
  CSVLOG_PACKBUF("handoff");
  IMPT("dtn_request_recv: runicast HandOff(L=%d) sending.\n",
       bufdata->num_copies);
}
/*---------------------------------------------------------------------------*/
const struct unicast_callbacks dtn_request_call = {dtn_request_recv};
/*-HANDOFF-------------------------------------------------------------------*/
void
dtn_handoff_recv(struct runicast_conn *r_c, const linkaddr_t *from,
                 uint8_t seqno)
{
  INFO("dtn_handoff_recv: runicast received from %02x:%02x, seqno %d\n",
       from->u8[1], from->u8[0], seqno);
  if (!dtn_valid_hdr()) return;
  struct dtn_conn *c = (struct dtn_conn *)
                       ((void *)r_c - offsetof(struct dtn_conn, handoff_c));
  struct packetqueue_item * q_item;
  if (!(q_item = dtn_queue_find(c))) { // not found in the queue
    IMPT("dtn_handoff_recv: HandOff not in the queue, do nothing.\n");
    return;
  }
  INFO("dtn_handoff_recv: HandOff found in the queue.\n");
  struct dtn_hdr *qbufdata = (struct dtn_hdr *)
                             queuebuf_dataptr(packetqueue_queuebuf(q_item));
  struct dtn_hdr *bufdata = dtn_buf_ptr();
  qbufdata->num_copies += bufdata->num_copies;
  if (qbufdata->num_copies > DTN_L_COPIES) {
    qbufdata->num_copies = DTN_L_COPIES;
  }
  q_item->ptr = DTN_READY;
  INFO("dtn_handoff_recv: packet state set to ready.\n");
  IMPT("dtn_handoff_recv: HandOff(L=%d) received and processed.\n",
       bufdata->num_copies);
  dtn_queue_spray((void *)c);
}
/*---------------------------------------------------------------------------*/
void
dtn_handoff_sent(struct runicast_conn *r_c, const linkaddr_t *to,
                 uint8_t retransmissions)
{
  INFO("dtn_handoff_sent: runicast sent to %02x:%02x, retried %d\n",
       to->u8[1], to->u8[0], retransmissions);
  struct dtn_conn *c = (struct dtn_conn *)
                       ((void *)r_c - offsetof(struct dtn_conn, handoff_c));
  if (linkaddr_cmp(&(c->handoff_qb->esender), &(c->handoff_hdr.esender))
      && c->handoff_qb->epacketid == c->handoff_hdr.epacketid) {
    int sent_copies = c->handoff_qb->num_copies / 2;
    c->handoff_qb->num_copies = c->handoff_qb->num_copies - sent_copies;
    IMPT("dtn_handoff_sent: HandOff(L=%d) processed.\n", sent_copies);
  } else {
    IMPT("dtn_handoff_sent: not matched (expired), HandOff not processed.\n");
  }
  c->handoff_qb = NULL;
}
/*---------------------------------------------------------------------------*/
void
dtn_handoff_timedout(struct runicast_conn *r_c, const linkaddr_t *to,
                     uint8_t retransmissions)
{
  IMPT("dtn_handoff_timedout: runicast timed out, to %02x:%02x, retried %d\n",
       to->u8[1], to->u8[0], retransmissions);
  struct dtn_conn *c = (struct dtn_conn *)
                       ((void *)r_c - offsetof(struct dtn_conn, handoff_c));
  c->handoff_qb = NULL;
  IMPT("dtn_handoff_sent: HandOff failed.\n");
}
/*---------------------------------------------------------------------------*/
const struct runicast_callbacks dtn_handoff_call = {dtn_handoff_recv,
                                                    dtn_handoff_sent,
                                                    dtn_handoff_timedout};
/*-DTN CALLS-----------------------------------------------------------------*/
void
dtn_open(struct dtn_conn *c, uint16_t dtn_channel,
         const struct dtn_callbacks *cb)
{
  random_init(clock_time());
  packetqueue_init(&dtn_packetqueue);
  c->q = &dtn_packetqueue;
  c->seqno = 0;
  c->cb = cb;
  c->handoff_qb = NULL;
  broadcast_open(&c->spray_c, dtn_channel, &dtn_spray_call);
  unicast_open(&c->request_c, dtn_channel + 1, &dtn_request_call);
  runicast_open(&c->handoff_c, dtn_channel + 2, &dtn_handoff_call);
  IMPT("dtn_open: DTN connection opened at channel (%d, %d, %d).\n",
       dtn_channel, dtn_channel + 1, dtn_channel + 2);
}
/*---------------------------------------------------------------------------*/
void
dtn_close(struct dtn_conn *c)
{
  broadcast_close(&c->spray_c);
  unicast_close(&c->request_c);
  runicast_close(&c->handoff_c);
  IMPT("dtn_close: DTN closed.");
}
/*---------------------------------------------------------------------------*/
int
dtn_send(struct dtn_conn *c, const linkaddr_t *to)
{
  if (linkaddr_cmp(to, &linkaddr_node_addr)) { // to me
    IMPT("dtn_send: send to myself, invoking callback.\n");
    c->cb->recv(c, to, c->seqno++);
    return 1;
  }
  struct dtn_hdr hdr;
  hdr.version = DTN_VERSION;
  hdr.magic[0] = 'S';
  hdr.magic[1] = 'W';
  hdr.num_copies = DTN_L_COPIES;
  hdr.epacketid = c->seqno;
  c->seqno++;
  linkaddr_copy(&hdr.ereceiver, to);
  linkaddr_copy(&hdr.esender, &linkaddr_node_addr);
  packetbuf_hdralloc(sizeof(struct dtn_hdr));
  memcpy(packetbuf_hdrptr(), &hdr, sizeof(struct dtn_hdr));
  print_packetbuf("dtn_send");
  if (packetqueue_enqueue_packetbuf(c->q,
                                    DTN_MAX_LIFETIME * CLOCK_SECOND,
                                    DTN_READY)) {
    INFO("dtn_send: Enqueued successfully.\n");
    dtn_queue_spray((void *)c);
    return 1;
  } else {
    IMPT("dtn_send: Failed to enqueue.\n");
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
void
dtn_set_power(uint8_t power)
{
  set_power(power);
  IMPT("dtn_set_power: Power set to 0x%02x.\n", power);
}
/*---------------------------------------------------------------------------*/
void
dtn_set_addr(linkaddr_t *addr)
{
  linkaddr_set_node_addr(addr);
  IMPT("dtn_set_addr: Local address set to ");
  IMPTADDR(addr);
  IMPT(".\n");
}
/*---------------------------------------------------------------------------*/
/** }@ */


void
clock_delay_msec(uint16_t howlong) {}        

void 
clock_delay_usec(uint16_t dt) {} 

void 
set_power(uint8_t power) {}