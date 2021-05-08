#include <stdio.h>
#include <string.h>

static struct simple_udp_connection;

struct Header
{
    unsigned int mst : 4;
    unsigned int qos : 1;
    unsigned int rl: 8;    
    unsigned int test: 3; 
    char headerOption[20];
};

struct Packet
{
    struct Header header;
    char payload[50];
};

//Enumeration for message type (MST)
enum MESSAGE_TYPE { RESERVED = 0, PUBLISH = 1, SUBSCRIBE=2, DISCONNECT = 3 , PUBACK = 4, CONNECT = 5, CONNACK = 6, SUBACK = 7,UNSUB = 8,UNSUBACK = 9, PINGREQ = 10, PINGRESP = 11,PUSH = 12 , PUSHACK = 13};

enum QOS { UNRELIABLE = 0, RELIABLE = 1};

static struct Packet createPacket(unsigned int mst, unsigned int qos, unsigned int rl, unsigned int test, char* headerOption, char* payload){
	struct Header header;
	header.mst = mst;
	header.qos = qos;
	header.rl = rl;
	header.test = test;
	strcpy(header.headerOption, headerOption);
	
	struct Packet packet;
	packet.header = header;
	strcpy(header.headerOption, payload);
	
	return packet;
}

static int sendPacket(struct Packet packet, struct simple_udp_connection *udp_conn){
	simple_udp_sendto(&udp_conn, packet, 100);
	return 0;
}

//When we receive a paquet, we split the paquet and we check which is the type of message
static int getMessageType(){
	return 0;
}

void connect(struct simple_udp_connection *udp_conn){
	struct Packet packet = createPacket(CONNECT, UNRELIABLE, 0, 0, "CONNECT", "testpayload");
	
	sendPacket(packet, udp_conn);
}

void connACK(){
	getMessageType();
	//createPacket();
}

void disconnect(){
	//createPacket();
}

void subscribe(){
	//sendPacket();
}

void subACK(){
	//sendPacket();
}

void unSUB(){
	//sendPacket();
}

void unSUBACK(){
	//sendPacket();
}

void publish(){
	//sendPacket();
}

void pubACK(){
	//sendPacket();
}

void push(){
	//sendPacket();
}

void pushACK(){
	//sendPacket();
}

void pingreq(){
	//sendPacket();
}

void pingresp(){
	//sendPacket();
}




