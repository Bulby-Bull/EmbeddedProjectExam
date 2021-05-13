#include "net/ipv6/simple-udp.h"
#include "structure.h" 

//void disconnect(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
bool isConnected();
void hello(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr,bool init);
void startPingThread(struct simple_udp_connection *udp_conn,	const uip_ipaddr_t *destAddr);
void publish(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr, bool command, char *topicname, char *value);
void subscribe(struct simple_udp_connection *udp_conn, const uip_ipaddr_t *destAddr, char *topicname);
void handleMessage(struct Packet packetRcv,struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
//void connACK(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr);
