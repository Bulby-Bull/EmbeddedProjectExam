#include "net/ipv6/simple-udp.h"
#include "structure.h"
#include "sys/log.h"
#include "random.h"


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



	bool ackRcv;
	int ackTypeWanted;
	struct simple_udp_connection *udp_connAck;
	uip_ipaddr_t destAddrAck;
	struct Packet packetAck;
	
#define SEND_INTERVAL		  (5 * CLOCK_SECOND)
PROCESS(ackThread, "Check ack");
PROCESS_THREAD(ackThread, ev, data)
	{
	  static struct etimer periodic_timer;
	  //static unsigned count;
	  //const uip_ipaddr_t *destAddrCast = data;
	  PROCESS_BEGIN();
	  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
	  LOG_INFO("ACK process start send to ");
	    	LOG_INFO_6ADDR(&destAddrAck);
	  while(1) {
	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

	    if(!ackRcv){
	    	LOG_INFO("ACK NOT received, resend to ");
	    	LOG_INFO_("Message type %i ",packetAck.header.mst );
	    	LOG_INFO_6ADDR(&destAddrAck);
      		LOG_INFO_("\n");
	    	simple_udp_sendto(udp_connAck,&packetAck,  sizeof(packetAck),&destAddrAck);
	    }else{
	    	//free(destAddrAck);
	    	PROCESS_EXIT();
	    }

	    /* Add some jitter */
	    etimer_set(&periodic_timer, SEND_INTERVAL
	      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
	  }

  	PROCESS_END();
	}



int qosThread(struct Packet packet, struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	udp_connAck = udp_conn;
	LOG_INFO("Change qos addresse to : ");
	    	LOG_INFO_6ADDR(destAddr);
      		LOG_INFO_("\n");
      	packetAck = packet;
	destAddrAck = *destAddr;
	process_start(&ackThread, &destAddr);
	
	return 0;
}

/* Send a packet to a device or a broker, called by connect, connACK, push, ... */
static int sendPacket(struct Packet packet, struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	if(packet.header.qos){
		ackRcv = false;
		ackTypeWanted = packet.header.mst+1;
		qosThread(packet, udp_conn, destAddr);
	}
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
	struct Packet packet = createPacket(CONNECT, RELIABLE, 0, 0, "CONNECT", "testpayload");
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

/* Return an acknowledge after subscription to a topic */
void subACK(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	struct Packet packet;
	packet = createPacket(SUBACK, UNRELIABLE, 0, 0, "", "");
	sendPacket(packet, udp_conn,destAddr);
}
 
/* Return an acknowledge after unsubscription to a topic */
void unSUBACK(){
	//sendPacket();
}


void pubACK(struct simple_udp_connection *udp_conn, const uip_ipaddr_t *destAddr){
	struct Packet packet;
	packet = createPacket(PUBACK, UNRELIABLE, 0, 0, "", "");
	sendPacket(packet, udp_conn,destAddr);
}

/* Transfer an information/command (method for the broker) */
void push(struct simple_udp_connection *udp_conn, const uip_ipaddr_t *destAddr, bool command, char *topicname, char *value){
	struct Packet packet;
	if(command){
		packet = createPacket(PUSH, RELIABLE, 0, 0, topicname, value);
		sendPacket(packet, udp_conn,destAddr);
	}else{
		packet = createPacket(PUSH, UNRELIABLE, 0, 0, topicname, value);
		sendPacket(packet, udp_conn,destAddr);
	}
}


/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/



/* Respond to a ping */
void pingresp(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	LOG_INFO("Send pingresp packet to ");
	LOG_INFO_6ADDR(destAddr);
      	LOG_INFO_("\n");
	struct Packet packet = createPacket(PINGRESP, UNRELIABLE, 0, 0, "PINGRESP", "pingRespTestPayload");
	sendPacket(packet, udp_conn,destAddr); 
}

 
int TOPICSIZE = 1;
struct Topic topics[1]; 

static unsigned count =0;
void handleMessage(struct Packet packetRcv,struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	int msgType = getMessageType(packetRcv);
  	LOG_INFO("Received msg with MessageType = %i\n",msgType);
  	
  	bool exist = 0;
  	bool created = 0;
  	struct Topic tp;

  	int i;
  	int j;

  	if(!ackRcv && msgType == ackTypeWanted){
  		LOG_INFO("ACK Well received\n");
  		ackRcv=true;
  	}
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
		case PUBLISH:
			//If the publish is in the reliable mode, send puback
			if(packetRcv.header.qos == 1) {
				LOG_INFO("PUBLISH received \n");
				LOG_INFO("PUBACK is sending... \n");
				pubACK(udp_conn,destAddr);
			}
			else{
				LOG_INFO("PUBLISH received received, NOT ACK \n");
			}
			LOG_INFO("SUP le topic est %s et le payload : %s \n", packetRcv.header.headerOption, packetRcv.payload);
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
							push(udp_conn , &topics[i].ips[j] , packetRcv.header.qos , packetRcv.header.headerOption , packetRcv.payload );
						}
					}
				}
			}
			break;
		case CONNECT://PUBACK();
			if(count<3){
			LOG_INFO("Connect received but wait for reliability connack count = %i\n",count);
			count++;
			}else{
			LOG_INFO("Connect received response with connack\n");
			connACK(udp_conn,destAddr);
			}
			break;
		case SUBSCRIBE:

			LOG_INFO("SU00 SUBSCRIBE received \n");

			LOG_INFO("SU01 le topic est %s \n", packetRcv.header.headerOption);

			//Loop for the topics
			for(i=0 ; i <= TOPICSIZE ; ++i){
				//if the topic is not null
				if( topics[i].name != NULL ){
					//if the topic math the received topic
					if( ! strcmp(topics[i].name , packetRcv.header.headerOption ) ){
						//Mark the topic as existing
						exist = 1;
						LOG_INFO("SU11 le topic existe déja \n");
						//We loop into the ip of this topic
						for(j=0 ; j <= 1 ; ++j){

							//Check if subscriber not already in the list TODO
							
							//if an ip slot is empty
							if( &topics[i].ips[j] == NULL ){
								//We write the ip of the sbscriber
								LOG_INFO("SU12 Subscriber registered \n");

								topics[i].ips[j] = *destAddr;
								LOG_INFO("SU13 Address from the pd ");
  								LOG_INFO_6ADDR(destAddr);
  								LOG_INFO_("\n");

							}
						}
					}
				}
			}
			//If the topic does not exist
			if(!exist){
				LOG_INFO("SU21 le topic n'existe pas encore \n");
				//Loop between the existing project to find an empty slot
				for(i=0 ; i <= TOPICSIZE ; ++i){
					if( topics[i].name == NULL && created == 0){
						created = 1;
						LOG_INFO("SU22 le topic a été enregistré \n");

						tp.name = packetRcv.header.headerOption;
						
						tp.ips[0] = *destAddr;

						topics[i] = tp;

						LOG_INFO("SU23 Address from the pd ");
  						LOG_INFO_6ADDR(destAddr);
  						LOG_INFO_("\n");
					}
				}
			}

			LOG_INFO("SUBACK is sending... \n");
			subACK(udp_conn,destAddr);

			//sendPacket();
			break;
		case PINGREQ://ping();
			LOG_INFO("Ping request received\n");
			LOG_INFO_6ADDR(destAddr);
			LOG_INFO("\n");
			pingresp(udp_conn,destAddr);
			break;
		case DISCONNECT://disconnect();
			connected = false;
			break;	
		case PUSHACK:
			LOG_INFO("SU PUSHACK received \n");
			break;
		default:
			LOG_INFO("\nERROR BAD MESSAGE TYPE\n\n");
			break;
	}

}




