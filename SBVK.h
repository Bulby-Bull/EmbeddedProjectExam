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
void disconnect(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
void subscribe(struct simple_udp_connection *udp_conn, const uip_ipaddr_t *destAddr, char *topicname);
void subACK(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
void unSUB();
void unSUBACK();
void publish(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr, bool command, char *topicname, char *value);
void pubACK(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
void push(struct simple_udp_connection *udp_conn, const uip_ipaddr_t *destAddr, bool command, char *topicname, char *value);
void pushACK(struct simple_udp_connection *udp_conn, const uip_ipaddr_t *destAddr);
bool isConnected();
void startPingThread(struct simple_udp_connection *udp_conn,	const uip_ipaddr_t *destAddr);
void stopPingThread();
void pingreq(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
void pingresp(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
void handleMessage(struct Packet packetRcv,struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
