
#include "structureapp.h" 
#include <sys/socket.h>

void handleMessage(struct Packet packetRcv,int sock, sockaddr *dest_addr)
