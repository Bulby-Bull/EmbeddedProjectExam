#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <core/net/uip.h>
#include "sys/log.h"
#include <stdio.h>
#include <string.h>
  
static void getMessageType(unsigned char type, const uint8_t * data);
static uip_ipaddr_t connect(const uint8_t *, uint16_t, uint16_t datalen);
static void disconnect();
static void send();
static void receive();
static void subscribe();
static void publish(struct simple_udp_connection * udp_conn, const uip_ipaddr_t * dest_ipaddr, const char * topic, const char * value);
static void ping();


static void connect() {
  uip_ipaddr_t dest_ipaddr;
  static struct etimer periodic_timer;
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);
                      
  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
       LOG_INFO("Connexion stablished");
       break;
    } else {
      LOG_INFO("Not reachable yet\n");
    }

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }
  return dest_ipaddr;
}

static void publish(struct simple_udp_connection * udp_conn, const uip_ipaddr_t * dest_ipaddr, const char * topic, const char * value){
  unsigned char type = 0x04; 
  strncat(type, &value, strlen(value));
  
  LOG_INFO("publishing value %u in link %s/%s ", value, dest_ipaddr,topic);
  LOG_INFO_6ADDR(&dest_ipaddr);
  LOG_INFO_("\n"); 
  simple_udp_sendto(&udp_conn, type, strlen(type), &dest_ipaddr/* + topic?*/ );
}


static void getMessageType(unsigned char type, const uint8_t * data, uint16_t datalen){

	switch (type){
		case 1://publish();
			break;
		case 2://subscribe();
			break;
		case 3://disconnect();
			break;
		case 4: //publish();
			break;
		case 5://PUBACK();
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

