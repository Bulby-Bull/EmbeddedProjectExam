#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "structure.h"
#include "sys/log.h"


#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO



/* Create a packet in function of the command (push, connect, ...) */
static struct Packet createPacket(unsigned int mst, unsigned int qos, unsigned int rl, unsigned int test, char* headerOption, char* payload){
	struct Header header;
	header.mst = mst;
	header.qos = qos;
	header.rl = rl;
	header.test = test;
	strcpy(header.headerOption, headerOption);
	
	struct Packet packet;
	packet.header = header;
	strcpy(packet.payload, payload);
	
	return packet;
}



/* Send a packet to a device or a broker, called by connect, connACK, push, ... */
static int sendPacket(struct Packet packet, struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	simple_udp_sendto(udp_conn,&packet,  sizeof(packet),destAddr);
	return 0;
}

//When we receive a paquet, we split the paquet and we check which is the type of message
 int getMessageType(struct Packet packet){
	return packet.header.mst;
}

/* Initiate a connection to a remote device or broker */
void hello(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr, bool init){
	
	
	struct Packet packet;
	if (init){
	LOG_INFO("Send hello packet to ");
	packet = createPacket(HELLO, UNRELIABLE, 0, 0, "init", "testpayload");
	}else{
	LOG_INFO("Send response hello packet to ");
	packet = createPacket(HELLO, UNRELIABLE, 0, 0, "response", "testpayload");
	}
	LOG_INFO_6ADDR(destAddr);
      	LOG_INFO_("\n");
	sendPacket(packet, udp_conn,destAddr);
}

/* Initiate a connection to a remote device or broker */
void connect(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	LOG_INFO("Send connect packet to ");
	LOG_INFO_6ADDR(destAddr);
      	LOG_INFO_("\n");
	struct Packet packet = createPacket(CONNECT, UNRELIABLE, 0, 0, "CONNECT", "testpayload");
	sendPacket(packet, udp_conn,destAddr);
}

/* Return an acknowledge after a CONNECTION paquet */
void connACK(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	LOG_INFO("Send connack packet to ");
	LOG_INFO_6ADDR(destAddr);
      	LOG_INFO_("\n");
	struct Packet packet = createPacket(CONNACK, UNRELIABLE, 0, 0, "CONNACK", "testpayload");
	sendPacket(packet, udp_conn,destAddr);
}

/* Finish a transmission (a client give the network) */
void disconnect(){
	//createPacket();
}

/* Subscribe to a topic */
void subscribe(){
	//sendPacket();
}

/* Return an acknowledge after subscription to a topic */
void subACK(){
	//sendPacket();
}

/* Unsubscribe to a topic */
void unSUB(){
	//sendPacket();
}

/* Return an acknowledge after unsubscription to a topic */
void unSUBACK(){
	//sendPacket();
}

/* Publish information/command for a topic */
void publish(){
	//sendPacket();
}

void pubACK(){
	//sendPacket();
}

/* Transfer an information/command (method for the broker) */
void push(){
	//sendPacket();
}


/* Return an acknowledge when the information/command is received */
void pushACK(){
	//sendPacket();
}



/* Make a ping to check the connection */
void pingreq(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){ 
	LOG_INFO("Send pingreq packet to ");
	LOG_INFO_6ADDR(destAddr);
      	LOG_INFO_("\n");
	struct Packet packet = createPacket(PINGREQ, UNRELIABLE, 0, 0, "PINGREQ", "testPayload");
	sendPacket(packet, udp_conn,destAddr);
}

/* Respond to a ping */
void pingresp(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	LOG_INFO("Send pingresp packet to ");
	LOG_INFO_6ADDR(destAddr);
      	LOG_INFO_("\n");
	struct Packet packet = createPacket(PINGRESP, UNRELIABLE, 0, 0, "PINGRESP", "pingRespTestPayload");
	sendPacket(packet, udp_conn,destAddr);
	//sendPacket();
}


void handleMessage(struct Packet packetRcv,struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr, bool * pingSend){
	int msgType = getMessageType(packetRcv);
  	LOG_INFO("MessageType = %i\n",msgType);
	switch (msgType){
		case HELLO:
			if(strcmp(packetRcv.header.headerOption ,"init")==0) {
				LOG_INFO("Hello received response with Hello\n");
				hello(udp_conn,destAddr,0);
			}else{
				LOG_INFO("Hello response received, response with connect\n");
				connect(udp_conn,destAddr);
			}
			
			break;
		case PUBLISH://publish();
			break;
		case SUBSCRIBE://subscribe();
			break;
		case DISCONNECT://disconnect();
			break;
		case PUBACK: //publish();
			break;
		case CONNECT://PUBACK();
			LOG_INFO("Connect received response with connack");
			connACK(udp_conn,destAddr);
			break;
		case CONNACK://connect(data, datalen);
			break;
		case SUBACK://SUBACK();
			break;
		case UNSUB://UNSUB();
			break;
		case UNSUBACK://UNSUBACK();
			break;
		case PINGREQ://ping();
			LOG_INFO("Ping request received\n");
			LOG_INFO_6ADDR(destAddr);
			LOG_INFO("\n");
			pingresp(udp_conn,destAddr);
			break;
		case PINGRESP://PINGRESP();
			LOG_INFO("PING response received\n");
			*pingSend = true;
			break;
		case PUSH://push();
			break;
		case PUSHACK://pushack();
			break;

		default:
			break;
	}
	//*pingSend = true;
}





