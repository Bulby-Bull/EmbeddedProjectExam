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

/* Finish a transmission (a client give the network) */
void disconnect(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
    LOG_INFO("Send disconnect packet to ");
    LOG_INFO_6ADDR(destAddr);
    LOG_INFO("\n");
    struct Packet packet = createPacket(DISCONNECT, UNRELIABLE, 0, 0, "DISCONNECT", "testpayload");
    sendPacket(packet, udp_conn,destAddr);

}

/* Subscribe to a topic */
void subscribe(struct simple_udp_connection *udp_conn, const uip_ipaddr_t *destAddr, char *topicname){
	struct Packet packet;
	packet = createPacket(SUBSCRIBE, RELIABLE, 0, 0, topicname ,"");
	sendPacket(packet, udp_conn,destAddr);
}

 

/* Unsubscribe to a topic */
void unSUB(){
	//sendPacket();
}
 

/* Publish information/command for a topic */
void publish(struct simple_udp_connection *udp_conn, const uip_ipaddr_t *destAddr, bool command, char *topicname, char *value){
	struct Packet packet;
	if(command){
		packet = createPacket(PUBLISH, RELIABLE, 0, 0, topicname, value);
		sendPacket(packet, udp_conn,destAddr);
	}else{
		packet = createPacket(PUBLISH, UNRELIABLE, 0, 0, topicname, value);
		sendPacket(packet, udp_conn,destAddr);
	}
	
}


/* Return an acknowledge when the information/command is received */
void pushACK(struct simple_udp_connection *udp_conn, const uip_ipaddr_t *destAddr){
	struct Packet packet;
	packet = createPacket(PUSHACK, UNRELIABLE, 0, 0, "", "");
	sendPacket(packet, udp_conn,destAddr);
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/




/* Make a ping to check the connection */
void pingreq(struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){ 
	LOG_INFO("Send pingreq packet to ");
	LOG_INFO_6ADDR(destAddr);
      	LOG_INFO_("\n");
	struct Packet packet = createPacket(PINGREQ, UNRELIABLE, 0, 0, "PINGREQ", "testPayload");
	sendPacket(packet, udp_conn,destAddr);
}



PROCESS(wait_ping_process, "WAIT client"); 

//bool ackRcv;
//int ackTypeWanted;
struct simple_udp_connection *udp_connPing;
uip_ipaddr_t destAddrPing; 
int countPing;

PROCESS_THREAD(wait_ping_process, ev, data){
	PROCESS_BEGIN(); 
	 countPing = 0;
	//const struct PingReq *pingReq =  data;
	 //const struct PingReq pingReqs = *pingReq;
	//const uip_ipaddr_t *sender_addr = &(pingReqs.dest_ipaddr);
	LOG_INFO("PING wait_ping_process " );
	LOG_INFO_6ADDR(&destAddrPing); 
	LOG_INFO("\n"); 	
	static struct etimer timer;
	
	//pingreq(udp_connPing,  &destAddrPing);
	etimer_set(&timer, CLOCK_SECOND*10); //Ping each 10 seconds
	//pingreq(&udp_conn,  sender_addr);
	
  	
  	while(1 && countPing < 5){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer)); 
 		LOG_INFO("Send ping number %i to ", countPing );
		LOG_INFO_6ADDR(&destAddrPing); 
		LOG_INFO("\n"); 
		pingreq(udp_connPing,  &destAddrPing);
		countPing++;
		
    		etimer_reset(&timer);
    		
	}
	disconnect(udp_connPing, &destAddrPing);    
	connected = false;
  	PROCESS_END();


} 

void startPingThread(struct simple_udp_connection *udp_conn,	const uip_ipaddr_t *destAddr){
	udp_connPing = udp_conn;
	destAddrPing = *destAddr;
	LOG_INFO("Ping should be to " );
		LOG_INFO_6ADDR(&destAddrPing); 
		LOG_INFO("\n"); 
	process_start(&wait_ping_process,NULL);//START/RESTART PROCESS PING
}

void stopPingThread(){
	process_exit(&wait_ping_process); //STOP PROCESS PING
}

bool isConnected(){
	return connected;
}

static unsigned count =0;
 
  

void handleMessage(struct Packet packetRcv,struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	int msgType = getMessageType(packetRcv);
  	LOG_INFO("Received msg with MessageType = %i\n",msgType);
  	 
 

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
		case PUBACK:
			LOG_INFO("PUBACK received \n");
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
		case CONNACK://connect(data, datalen);
			connected = true;
			break;
		case SUBACK:
			LOG_INFO("SUBACK received \n");
			break;
		case UNSUBACK://UNSUBACK();
			break;
		case PINGRESP://PINGRESP();
			LOG_INFO("PING response received\n");
			stopPingThread();
			break; 
		case PUSH:
			LOG_INFO("SU received topic %s and value %s \n" , packetRcv.header.headerOption , packetRcv.payload);
			pushACK(udp_conn,destAddr);
			break;
		default:
			LOG_INFO("\nERROR BAD MESSAGE TYPE\n\n");
			break;
	}

}




