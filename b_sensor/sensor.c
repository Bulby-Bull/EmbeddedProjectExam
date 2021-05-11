#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <sys/types.h>
#include "../SBVK.h"
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (60 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;

//static bool * pingSend; 

static bool * isFirstConnection;  
 

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
 PROCESS(wait_ping_process, "WAIT client");
AUTOSTART_PROCESSES(&udp_client_process ); 
/*---------------------------------------------------------------------------*/

 
PROCESS_THREAD(wait_ping_process, ev, data){
	const struct PingReq *pingReq =  data;
	 const struct PingReq pingReqs = *pingReq;
	const uip_ipaddr_t *sender_addr = &(pingReqs.dest_ipaddr);
	LOG_INFO("PING wait_ping_process " );
	LOG_INFO_6ADDR(sender_addr); 
	LOG_INFO("\n"); 	
	static struct etimer timer;
	
	pingreq(&udp_conn,  sender_addr);
	etimer_set(&timer, CLOCK_SECOND*10);
	pingreq(&udp_conn,  sender_addr);
	
  	PROCESS_BEGIN(); 
  	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer)); 
 
		pingreq(&udp_conn,  sender_addr);
		
    		etimer_reset(&timer);
    		
	}
  	PROCESS_END();


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
  
	//process_exit(&wait_ping_process);
  bool pingSend=true;
  LOG_INFO("Received response '%.*s' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO("\n");
  struct Packet* received_struct_ptr;
    received_struct_ptr = (struct Packet*) data;
    struct Packet packetRcv;
    packetRcv = *received_struct_ptr;
    handleMessage(packetRcv,&udp_conn,sender_addr, &pingSend);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n"); 
   
  if(pingSend == true){ 
  
  	LOG_INFO("ping OK ");
	LOG_INFO_6ADDR(sender_addr); 
	LOG_INFO("\n"); 	
  	struct PingReq envir = { .dest_ipaddr = *receiver_addr };
  	//envir.dest_ipaddr =*receiver_addr; 
	process_start(&wait_ping_process,&envir);
	
		
  }else{
  
	LOG_INFO("Process STOP wait_ping_process " );
	process_exit(&wait_ping_process);
  }
   
  
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  static unsigned count; 
  
  //static char str[32];
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  while(1) {
     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer)); 
     LOG_INFO("is first connexion : %u\n", *isFirstConnection);
     if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
     
      if((*isFirstConnection) == true){
      /* Send to DAG root */
		LOG_INFO("Sending hello request %u to ", count);
	      	LOG_INFO_6ADDR(&dest_ipaddr);
	      	LOG_INFO_("\n");
	      	hello(&udp_conn,&dest_ipaddr,1);  
	      	*isFirstConnection = false;
      //snprintf(str, sizeof(str), "hello %d", count);
      //simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
      
     } else {
     /**
     	static struct  etimer  timer;
	etimer_set(&timer, 1 * CLOCK_SECOND);
	pingreq(&udp_conn,&dest_ipaddr); 
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

	if(++count > 25){
		count=0;		//isFirstConnection=true;
	}
	*/
     }
     
    } else {
      LOG_INFO("Not reachable yet \n");
    }
    

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
