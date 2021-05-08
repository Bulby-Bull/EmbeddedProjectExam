#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"

void connect(struct simple_udp_connection *udp_conn);
void connACK();
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
