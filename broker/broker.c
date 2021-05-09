#include "../SBVK.h"

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

static struct simple_udp_connection udp_conn;
//const uip_ipaddr_t *iptest;

static void handleMessage(struct Packet packetRcv,struct simple_udp_connection *udp_conn,const uip_ipaddr_t *destAddr){
	int msgType = getMessageType(packetRcv);
  	LOG_INFO("MessageType = %i",msgType);
	switch (msgType){
		case 1://publish();
			break;
		case 2://subscribe();
			break;
		case 3://disconnect();
			break;
		case 4: //publish();
			break;
		case CONNECT://PUBACK();
			connACK(udp_conn,destAddr);
			break;
		case 6://connect(data, datalen);
			break;
		case 7://SUBACK();
			break;
		case 8://UNSUB();
			break;
		case 9://UNSUBACK();
			break;
		case 10://ping();
			break;
		case 11://PINGRESP();
			break;
		case 12://push();
			break;
		case 13://pushack();
			break;

		default:
			break;
	}
}


static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  LOG_INFO("Received request '%.*s \n' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
  struct Packet* received_struct_ptr;
    received_struct_ptr = (struct Packet*) data;
    struct Packet packetRcv;
    packetRcv = *received_struct_ptr;
    handleMessage(packetRcv,&udp_conn,sender_addr);
  LOG_INFO_("\n");
#if WITH_SERVER_REPLY
  /* send back the same string to the client as an echo reply */
  LOG_INFO("Sending response.\n");
  simple_udp_sendto(&udp_conn, data, datalen, sender_addr);
#endif /* WITH_SERVER_REPLY */
}


PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);
                  
  //iptest = "fd00::c30:0:0:1";
  //simple_udp_sendto(&udp_conn, "MegaTest", 100, iptest);
  LOG_INFO_("Sending mess \n");                   
  //connect(&udp_conn);
  LOG_INFO_("OK paquet sent \n");  

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
