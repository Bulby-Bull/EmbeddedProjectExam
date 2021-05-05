#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"


static char * topics;



static void connect();
static void disconnect();
static void send();
static void receive();
static void subscribe();
static void publish();
static void ping();
