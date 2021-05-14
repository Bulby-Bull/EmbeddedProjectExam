#include "structure.h"
#include "sys/log.h"
#include "random.h"
#include <sys/socket.h>

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



// 	bool ackRcv;
// 	int ackTypeWanted;
// 	struct simple_udp_connection *udp_connAck;
// 	uip_ipaddr_t destAddrAck;
// 	struct Packet packetAck;
	
// #define SEND_INTERVAL		  (5 * CLOCK_SECOND)
// PROCESS(ackThread, "Check ack");
// PROCESS_THREAD(ackThread, ev, data)
// 	{
// 	  static struct etimer periodic_timer;
// 	  //static unsigned count;
// 	  //const uip_ipaddr_t *destAddrCast = data;
// 	  PROCESS_BEGIN();
// 	  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
// 	  while(1) {
// 	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

// 	    if(!ackRcv){
// 	    	simple_udp_sendto(udp_connAck,&packetAck,  sizeof(packetAck),&destAddrAck);
// 	    }else{
// 	    	//free(destAddrAck);
// 	    	PROCESS_EXIT();
// 	    }

// 	    /* Add some jitter */
// 	    etimer_set(&periodic_timer, SEND_INTERVAL
// 	      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
// 	  }

//   	PROCESS_END();
// 	}



int qosThread(struct Packet packet){
	// udp_connAck = udp_conn;
 //      	packetAck = packet;
	// destAddrAck = *destAddr;
	// process_start(&ackThread, &destAddr);
	
	return 0;
}

/* Send a packet to a device or a broker, called by connect, connACK, push, ... */
static int sendPacket(int sock, struct Packet packet, sockaddr *dest_addr){
	// if(packet.header.qos){
	// 	ackRcv = false;
	// 	ackTypeWanted = packet.header.mst+1;
	// 	qosThread(packet, udp_conn, destAddr);
	// }
	sendto(sock, &packetRcv, sizeof(packetRcv),MSG_CONFIRM, (const struct sockaddr *) &dest_addr, sizeof(sin6));

	//simple_udp_sendto(udp_conn,&packet,  sizeof(packet),destAddr);
	return 0;
}

//When we receive a paquet, we split the paquet and we check which is the type of message
 int getMessageType(struct Packet packet){
	return packet.header.mst;
}

/* Initiate a connection to a remote device or broker */
void hello(int sock, sockaddr *dest_addr, bool init){
	
	
	struct Packet packet;
	if (init){
	packet = createPacket(HELLO, UNRELIABLE, 0, 0, "init", "testpayload");
	}else{
	packet = createPacket(HELLO, UNRELIABLE, 0, 0, "response", "testpayload");
	}
	sendPacket(sock, packet,destAddr);
}

/* Initiate a connection to a remote device or broker */
void connect(int sock, sockaddr *dest_addr, bool init){
	struct Packet packet = createPacket(CONNECT, RELIABLE, 0, 0, "CONNECT", "testpayload");
	sendPacket(sock, packet,destAddr);
}

/* Return an acknowledge after a CONNECTION paquet */
void connACK(int sock, sockaddr *dest_addr){
	struct Packet packet = createPacket(CONNACK, UNRELIABLE, 0, 0, "CONNACK", "testpayload");
	sendPacket(sock, packet,destAddr);
}

/* Return an acknowledge after subscription to a topic */
void subACK(int sock, sockaddr *dest_addr){
	struct Packet packet;
	packet = createPacket(SUBACK, UNRELIABLE, 0, 0, "", "");
	sendPacket(sock, packet,destAddr);
}
 
/* Return an acknowledge after unsubscription to a topic */
void unSUBACK(){
	//sendPacket();
}


void pubACK(int sock, sockaddr *dest_addr){
	struct Packet packet;
	packet = createPacket(PUBACK, UNRELIABLE, 0, 0, "", "");
	sendPacket(sock, packet,destAddr);
}

/* Transfer an information/command (method for the broker) */
void push(int sock, sockaddr *dest_addr,bool command, char *topicname, char *value){
	struct Packet packet;
	if(command){
		packet = createPacket(PUSH, RELIABLE, 0, 0, topicname, value);
		sendPacket(sock, packet,destAddr);
	}else{
		packet = createPacket(PUSH, UNRELIABLE, 0, 0, topicname, value);
		sendPacket(sock, packet,destAddr);
	}
}


/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/



/* Respond to a ping */
void pingresp(int sock, sockaddr *dest_addr){
	struct Packet packet = createPacket(PINGRESP, UNRELIABLE, 0, 0, "PINGRESP", "pingRespTestPayload");
	sendPacket(sock, packet,destAddr);
}

 
int TOPICSIZE = 1;
struct Topic topics[1]; 

static unsigned count =0;

void handleMessage(struct Packet packetRcv,int sock, sockaddr *dest_addr){
	int msgType = getMessageType(packetRcv);
  	
  	bool exist = 0;
  	bool created = 0;
  	struct Topic tp;

  	int i;
  	int j;

  	if(!ackRcv && msgType == ackTypeWanted){
  		ackRcv=true;
  	}
	switch (msgType){
		case HELLO:
			if(strcmp(packetRcv.header.headerOption ,"init")==0) {
				hello(sock,destAddr,0);
			}else{
				connect(sock,destAddr);
			}
			
			break;
		case PUBLISH:
			//If the publish is in the reliable mode, send puback
			if(packetRcv.header.qos == 1) {
				pubACK(sock,destAddr);
			}
			else{
				//PUBLISH received received, NOT ACK
			}
			//TODO Envoyer aux subscribers avec un push 

			//Loop between all topics
			for (i = 0; i <=1; ++i)
			{
				//If the topic is not null
				if( topics[i].name != NULL ){
					//If the topic correspond to the published topic
					if( ! strcmp(topics[i].name , packetRcv.header.headerOption ) ){
						//Send to all ip a push
						for (j = 0; j <= 1; ++j)
						{
							push(sock ,destAddr, &topics[i].ips[j] , packetRcv.header.qos , packetRcv.header.headerOption , packetRcv.payload );
						}
					}
				}
			}
			break;
		case CONNECT://PUBACK();
			if(count<3){
				count++;
			}else{
			connACK(sock,destAddr);
			}
			break;
		case SUBSCRIBE:

			//Loop for the topics
			for(i=0 ; i <= TOPICSIZE ; ++i){
				//if the topic is not null
				if( topics[i].name != NULL ){
					//if the topic math the received topic
					if( ! strcmp(topics[i].name , packetRcv.header.headerOption ) ){
						//Mark the topic as existing
						exist = 1;
						//We loop into the ip of this topic
						for(j=0 ; j <= 1 ; ++j){

							//Check if subscriber not already in the list TODO
							
							//if an ip slot is empty
							if( &topics[i].ips[j] == NULL ){
								//We write the ip of the sbscriber
								
								topics[i].ips[j] = *destAddr;
								
							}
						}
					}
				}
			}
			//If the topic does not exist
			if(!exist){
				//Loop between the existing project to find an empty slot
				for(i=0 ; i <= TOPICSIZE ; ++i){
					if( topics[i].name == NULL && created == 0){
						created = 1;

						tp.name = packetRcv.header.headerOption;
						
						tp.ips[0] = *destAddr;

						topics[i] = tp;

		
					}
				}
			}

			
			subACK(sock,destAddr);

			//sendPacket();
			break;
		case PINGREQ://ping();
			
			pingresp(sock,destAddr);
			break;
		case DISCONNECT://disconnect();
			connected = false;
			break;	
		case PUSHACK:
			
			break;
		default:
			
			break;
	}

}




