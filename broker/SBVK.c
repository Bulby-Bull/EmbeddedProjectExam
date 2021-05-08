#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"


//Structure of the header inside the packet
struct Header
{
    unsigned int mst : 4;
    unsigned int qos : 1;
    unsigned int rl: 8;    
    unsigned int test: 3; 
    char headerOption[20];
};

//Structure of a packet
struct Packet
{
    struct Header header;
    char payload[50];
};

//Enumeration for message type (MST)
enum MESSAGE_TYPE { RESERVED = 0, PUBLISH = 1, SUBSCRIBE=2, DISCONNECT = 3 , PUBACK = 4, CONNECT = 5, CONNACK = 6, SUBACK = 7,UNSUB = 8,UNSUBACK = 9, PINGREQ = 10, PINGRESP = 11,PUSH = 12 , PUSHACK = 13};

//Enumeration for both QoS
enum QOS { UNRELIABLE = 0, RELIABLE = 1};

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

//Because of in Cooja Calloc is not implemented in stdlib, we have to create it ourself
void * calloc (size_t a, size_t b){
	return malloc(a*b);
}

/* Convert Structures in string to be sent in udp_send */
/* url source: https://stackoverflow.com/questions/29762048/c-structure-to-string */
static char* convertPacketToString(struct Packet packet){
	/* get lenght of string required to hold struct values */
        size_t len = 0;
        len = snprintf (NULL, len, "%d,%d,%d,%d,%s,%s", packet.header.mst, packet.header.qos, packet.header.rl, packet.header.test, packet.header.headerOption, packet.payload);
        
        /* allocate/validate string to hold all values (+1 to null-terminate) */
        char *packetstr = calloc (1, sizeof *packetstr * len + 1);

        /* write/validate struct values to apstr */
        snprintf (packetstr, len + 1, "%d,%d,%d,%d,%s,%s", packet.header.mst, packet.header.qos, packet.header.rl, packet.header.test, packet.header.headerOption, packet.payload);
	
	return packetstr;
}

/* Send a packet to a device or a broker, called by connect, connACK, push, ... */
static int sendPacket(struct Packet packet, struct simple_udp_connection *udp_conn){
	simple_udp_send(udp_conn, convertPacketToString(packet), 100);
	return 0;
}

//When we receive a paquet, we split the paquet and we check which is the type of message
static int getMessageType(){
	return 0;
}


/* Initiate a connection to a remote device or broker */
void connect(struct simple_udp_connection *udp_conn){
	struct Packet packet = createPacket(CONNECT, UNRELIABLE, 0, 0, "CONNECT", "testpayload");
	
	sendPacket(packet, udp_conn);
}

/* Return an acknowledge after a CONNECTION paquet */
void connACK(){
	getMessageType();
	//createPacket();
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
void pingreq(){
	//sendPacket();
}

/* Respond to a ping */
void pingresp(){
	//sendPacket();
}




