#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "structure.h"
#include "sys/log.h"

void connect(struct simple_udp_connection *udp_conn, const uip_ipaddr_t *destAddr);
int getMessageType(struct Packet packet);
void connACK(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
void hello(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr,bool init);
void disconnect();
void subscribe();
void subACK();
void unSUB();
void unSUBACK();
void publish();
void pubACK();
void push();
void pushACK();
void pingreq();
void pingresp();
void handleMessage(struct Packet packetRcv,struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
