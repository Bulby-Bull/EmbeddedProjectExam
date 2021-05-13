//to compile with gcc use this : gcc -o control controlserv.c -lpthread

#include <stdio.h>
#include <stdlib.h>
#include "structureapp.h"
#include <pthread.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

///////////////////////kriki part inits + methods
#define PORT     60001
#define MAXLINE 1024

#define FROMPORT 5678
static const uint8_t udpFrom[16] = { 0xbb, 0xbb,0x00, 0x00,0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

static const uint8_t udpRemote[16] = { 0xbb, 0xbb,0x00, 0x00,0x00, 0x00,0x00, 0x00, 0xc3, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};

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
//////////////////////


/**
 * Clear the CLI for windows or linux
 *
 */
void clear(){
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    system("clear");
#endif

#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#endif
}

struct Packet createPacket(unsigned int mst, unsigned int qos, unsigned int rl, unsigned int test, char* headerOption, char* payload){
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

/**
* Set an UDP connection and send a packet to the border router
**/
void sendUDP(struct Packet packet) {
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
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(PORT);
    //servaddr.sin_addr.s_addr = INADDR_ANY;
    memcpy(servaddr.sin6_addr.s6_addr, udpRemote, sizeof udpRemote);


    fromaddr.sin6_family = AF_INET6;
    fromaddr.sin6_port = htons(FROMPORT);
    //servaddr.sin_addr.s_addr = INADDR_ANY;
    memcpy(fromaddr.sin6_addr.s6_addr, udpFrom, sizeof udpFrom);
    bind(sockfd, (struct sockaddr *) &fromaddr, sizeof fromaddr);
    connect(sockfd,(struct sockaddr *) &servaddr, sizeof servaddr);

    int n, len;

    while(1){
        //sendto(sockfd, &packet, sizeof(packet),
          //     MSG_CONFIRM, (const struct sockaddr *) &servaddr,
            //   sizeof(servaddr));
        //printf("Hello message sent.\n");
        //ipv6_expander(&servaddr.sin6_addr);
    

    n = recvfrom(sockfd, (struct Packet*)buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *) &servaddr,
                 &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
	}
    close(sockfd);
}


/**
* Set an UDP connection and send a packet to the border router
**/
void receiveUDP() {
    int sockfd;
    struct Packet buffer[MAXLINE];
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
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(PORT);
    servaddr.sin6_addr = in6addr_any;
    //memcpy(servaddr.sin6_addr.s6_addr, udpRemote, sizeof udpRemote);


    fromaddr.sin6_family = AF_INET6;
    fromaddr.sin6_port = htons(FROMPORT);
    //servaddr.sin_addr.s_addr = INADDR_ANY;
    memcpy(fromaddr.sin6_addr.s6_addr, udpFrom, sizeof udpFrom);
    bind(sockfd, (struct sockaddr *) &fromaddr, sizeof fromaddr);
    //connect(sockfd,(struct sockaddr *) &servaddr, sizeof servaddr);

    int n, len;

    while(1){
        //sendto(sockfd, &packet, sizeof(packet),
          //     MSG_CONFIRM, (const struct sockaddr *) &servaddr,
            //   sizeof(servaddr));
        //printf("Hello message sent.\n");
        //ipv6_expander(&servaddr.sin6_addr);
    

    n = recvfrom(sockfd, (struct Packet*)buffer, 72,
                 MSG_WAITALL, (struct sockaddr *) &servaddr,
                 &len);
    //buffer[n] = '\0';
    struct Packet packetRcv;
    packetRcv = *buffer;
    printf("Server : Receive packet msgtype = %i\n", packetRcv.header.mst);
    }
    close(sockfd);
}

/**
 * Main menu of the application
 * @return response - choice of menu
 */
int callMenu() {
    int response;
    printf("This is the menu, choose a device to manage (1-5) \n");
    printf("********************************************************** \n");
    printf("1. The connected light \n");
    printf("2. The connected washer \n");
    printf("3. Check the gaz sensor state \n");
    printf("4. Check the alarm state \n");
    printf("5. Stop the program \n");
    printf("********************************************************** \n");
    scanf("%d", &response);
    clear();
    return response;
}

/**
 * Call to send the command to the broker for the light
 * @param result on or off
 */
void sendCommandToLight(int result) {
    if(result == 1){
        printf("---------------------------- \n");
        printf("The light is switching ON \n");
        printf("---------------------------- \n \n");
        //Todo send packet in UDP to the server
        struct Packet packet;
        packet = createPacket(PUBLISH, RELIABLE, 0, 0, "Light", "ON");
        sendUDP(packet);

    }
    else if (result == 2) {
        printf("---------------------------- \n");
        printf("The light is switching OFF \n");
        printf("---------------------------- \n \n");
        //Todo send packet in UDP to the server
        struct Packet packet;
        packet = createPacket(PUBLISH, RELIABLE, 0, 0, "Light", "OFF");
        sendUDP(packet);
    }
}

/**
 * Light management
 */
void callLight() {
    int reset = 0;
    int result;
    while (reset == 0){
        printf("CONNECTED LIGHT \n");
        printf("Choose a action (1-3) : \n");
        printf("################################### \n");
        printf("1. ON \n");
        printf("2. OFF \n");
        printf("3. Return in menu \n");
        printf("################################### \n");
        scanf("%d", &result);
        clear();
        switch(result){
            case 1:
                sendCommandToLight(result);
                reset = 1;
                break;
            case 2:
                sendCommandToLight(result);
                reset = 1;
                break;
            case 3:
                reset = 1;
                break;
            default:
                printf("---------------------------- \n");
                printf("Enter a correct number \n");
                printf("---------------------------- \n \n");
                break;
        }
    }
}

/**
 *
 * @param result int of choice
 */
void sendCommandToWasher(int result) {
    if(result == 1){
        printf("---------------------------- \n");
        printf("A new cycle is sending... \n");
        printf("---------------------------- \n \n");
        //Todo send packet in UDP to the server
        struct Packet packet;
        packet = createPacket(PUBLISH, RELIABLE, 0, 0, "Washer", "ON");
        sendUDP(packet);
    }
    else if (result == 2) {
        printf("----------------------------------- \n");
        printf("The current cycle is stopping... \n");
        printf("----------------------------------- \n \n");
        //Todo send packet in UDP to the server
        struct Packet packet;
        packet = createPacket(PUBLISH, RELIABLE, 0, 0, "Washer", "OFF");
        sendUDP(packet);
    }
}

/**
 * Washer management
 */
void callWasher() {
    int reset = 0;
    int result;
    while (reset == 0){
        //Todo Recover information from broker on the state
        printf("CONNECTED WASHER \n");
        printf("----------------------------------------------------------- \n");
        printf("|The current state is : %s                                | \n", "ON");
        printf("----------------------------------------------------------- \n \n");
        printf("Choose a action (1-3) : \n");
        printf("################################### \n");
        printf("1. Run a new cycle \n");
        printf("2. Stop the current cycle \n");
        printf("3. Return in menu \n");
        printf("################################### \n");
        scanf("%d", &result);
        clear();
        switch(result){
            case 1:
                sendCommandToWasher(result);
                reset = 1;
                break;
            case 2:
                sendCommandToWasher(result);
                reset = 1;
                break;
            case 3:
                reset = 1;
                break;
            default:
                printf("---------------------------- \n");
                printf("Enter a correct number \n");
                printf("---------------------------- \n \n");
                break;
        }
    }
}

/**
 * Gaz Sensor management
 */
void callGazSensor() {
    printf("GAZ SENSOR STATUS \n");
    printf("The current value is represented between 200 to 10000ppm \n");
    printf("The threshold to not exceed is 400ppm: \n");
    printf("----------------------------------------------------------- \n");
    printf("|The current value is :  %d ppm                            | \n", 5);
    printf("----------------------------------------------------------- \n");
    // Todo the value is > than 400, go activate the alarm
    printf("--------------------------------------- \n");
    printf("Enter to return in the main menu... \n");
    printf("--------------------------------------- \n");
    system("pause");
}

/**
 * Alarm management
 */
void callAlarm() {
    printf("ALARM STATUS \n");
    printf("----------------------------------------------------------- \n");
    printf("|The current value is :  %s                              | \n", "OFF");
    printf("----------------------------------------------------------- \n");
    // Todo recover the alarm status from broker
    printf("--------------------------------------- \n");
    printf("Enter to return in the main menu... \n");
    printf("--------------------------------------- \n");
    system("pause");
}

void *handleReceiver(void *vargp)
{
    printf("Je reçois des packets \n");

    return NULL;
}

int main() {

    receiveUDP();

    //thread created to handle received packet from the border router
    //pthread_t thread_id;
    //pthread_create(&thread_id, NULL, handleReceiver, NULL);

    int launch = 0;

    printf("Welcome to the Control Server \n \n");
    while(launch == 0){
        int response = 0;
        response = callMenu();
        switch(response){
            case 1:
                callLight();
                break;
            case 2:
                callWasher();
                break;
            case 3:
                callGazSensor();
                clear();
                break;
            case 4:
                callAlarm();
                clear();
                break;
            case 5:
                printf("---------------------- \n");
                printf("End of program \n");
                printf("---------------------- \n");
                for(int i = 0 ; i < 4; i++){//Sleep 500ms*4
                    printf(".");
#ifdef _WIN32 //For windows
                    Sleep(500);
#else //For linux
                    usleep(500000);
#endif
                }
                launch = 1;
                break;
            default:
                printf("---------------------------------------- \n");
                printf("Pliz enter a number between 1 and 5 \n");
                printf("---------------------------------------- \n \n");
                break;
        }
    }
    return 0;
}




