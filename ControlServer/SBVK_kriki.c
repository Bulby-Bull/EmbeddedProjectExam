#include "structureapp.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO
bool connected=false;

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


void ipv6_expander(const struct in6_addr * addr) {
    char str[40];
    sprintf(str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
            (int)addr->s6_addr[0], (int)addr->s6_addr[1],
            (int)addr->s6_addr[2], (int)addr->s6_addr[3],
            (int)addr->s6_addr[4], (int)addr->s6_addr[5],
            (int)addr->s6_addr[6], (int)addr->s6_addr[7],
            (int)addr->s6_addr[8], (int)addr->s6_addr[9],
            (int)addr->s6_addr[10], (int)addr->s6_addr[11],
            (int)addr->s6_addr[12], (int)addr->s6_addr[13],
            (int)addr->s6_addr[14], (int)addr->s6_addr[15]);
    printf("Ipv6 addr = %s \n", str);
}




/* Send a packet to a device or a broker, called by connect, connACK, push, ... */
int sendPacket(int sock, struct Packet packet, struct  sockaddr_in6 dest_addr){
	// if(packet.header.qos){
	// 	ackRcv = false;
	// 	ackTypeWanted = packet.header.mst+1;
	// 	qosThread(packet, udp_conn, destAddr);
	// }
	printf("send packet to");
	sendto(sock, &packet, sizeof(packet),MSG_CONFIRM, ( struct sockaddr *) &dest_addr, sizeof(dest_addr));

	//simple_udp_sendto(udp_conn,&packet,  sizeof(packet),destAddr);
	return 0;
}

//When we receive a paquet, we split the paquet and we check which is the type of message
 int getMessageType(struct Packet packet){
	return packet.header.mst;
}

/* Initiate a connection to a remote device or broker */
void hello(int sock, struct  sockaddr_in6 dest_addr, bool init){
	
	printf("send hello2\n");
	struct Packet packet;
	if (init){
	packet = createPacket(HELLO, UNRELIABLE, 0, 0, "init", "testpayload");
	}else{
	packet = createPacket(HELLO, UNRELIABLE, 0, 0, "response", "testpayload");
	}
	printf("send hello3\n");
	sendPacket(sock, packet,dest_addr);
	printf("send hello 4\n");
}

/* Initiate a connection to a remote device or broker */
void sendConnect(int sock, struct  sockaddr_in6 dest_addr){
	struct Packet packet = createPacket(CONNECT, RELIABLE, 0, 0, "CONNECT", "testpayload");
	sendPacket(sock, packet,dest_addr);
}

/* Return an acknowledge after a CONNECTION paquet */
void connACK(int sock, struct  sockaddr_in6 dest_addr){
	struct Packet packet = createPacket(CONNACK, UNRELIABLE, 0, 0, "CONNACK", "testpayload");
	sendPacket(sock, packet,dest_addr);
}



 
// int TOPICSIZE = 1;
// struct Topic topics[1]; 

static unsigned count =0;

void handleMessage(struct Packet packetRcv,int sock,struct  sockaddr_in6 dest_addr){
	
	int msgType = getMessageType(packetRcv);
  	printf("handleMessage %i \n", msgType);
  	bool exist = 0;
  	bool created = 0;
  	//struct Topic tp;

  	int i;
  	int j;

  	// if(!ackRcv && msgType == ackTypeWanted){
  	// 	ackRcv=true;
  	// }
	switch (msgType){
		case HELLO:
			printf("send hello\n");
			if(strcmp(packetRcv.header.headerOption ,"init")==0) {
				hello(sock,dest_addr,0);
			}else{
				sendConnect(sock,dest_addr);
			}
			
			break;
		case CONNECT://PUBACK();
			// if(count<3){
			// 	count++;
			// }else{
			connACK(sock,dest_addr);
			// }
			break;
		default:
			
			break;
	}

}




