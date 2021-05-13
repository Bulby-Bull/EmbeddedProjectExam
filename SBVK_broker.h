#include "net/ipv6/simple-udp.h"
#include "structure.h" 

void handleMessage(struct Packet packetRcv,struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
