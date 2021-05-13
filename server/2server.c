#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
  
#define PORT     60001
#define MAXLINE 1024

#define FROMPORT 5678
static const uint8_t udpFrom[16] = { 0xaa, 0xaa,0x00, 0x00,0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

static const uint8_t udpRemote[16] = { 0xbb, 0xbb,0x00, 0x00,0x00, 0x00,0x00, 0x00, 0xc3, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

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

  
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from client";
    struct sockaddr_in6     servaddr;
    struct sockaddr_in6     fromaddr;
  
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
  
    memset(&servaddr, 0, sizeof(servaddr));
      
    // Filling server information
    servaddr.sin6_family = AF_INET;
    servaddr.sin6_port = htons(PORT);
    //servaddr.sin_addr.s_addr = INADDR_ANY;
     memcpy(servaddr.sin6_addr.s6_addr, udpRemote, sizeof udpRemote);
 
 
    fromaddr.sin6_family = AF_INET;
    fromaddr.sin6_port = htons(PORT);
    //servaddr.sin_addr.s_addr = INADDR_ANY;
    memcpy(fromaddr.sin6_addr.s6_addr, udpFrom, sizeof udpFrom);
    bind(sockfd, (struct sockaddr *) &fromaddr, sizeof fromaddr);

    int n, len;
    while(1){  
    sendto(sockfd, (const char *)hello, strlen(hello),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr));
    printf("Hello message sent.\n");
    ipv6_expander(&servaddr.sin6_addr);
    //printf("Port = ");
    }      

    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
  
    close(sockfd);
    return 0;
}