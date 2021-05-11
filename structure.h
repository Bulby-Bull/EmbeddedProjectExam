//Structure of the header inside the packet
//Enumeration for message type (MST)
enum MESSAGE_TYPE { HELLO = 0, PUBLISH = 1, SUBSCRIBE=2, DISCONNECT = 3 , PUBACK = 4, CONNECT = 5, CONNACK = 6, SUBACK = 7,UNSUB = 8,UNSUBACK = 9, PINGREQ = 10, PINGRESP = 11,PUSH = 12 , PUSHACK = 13};

//Enumeration for both QoS
enum QOS { UNRELIABLE = 0, RELIABLE = 1};

struct Header
{
    unsigned int mst : 4;
    unsigned int qos : 1;
    unsigned int rl: 8;    
    unsigned int test: 3; 
    char headerOption[20];
};

//Structure of a packet
struct Packet
{
    struct Header header;
    char payload[50];
};
struct PingReq
{
     const uip_ipaddr_t dest_ipaddr;
};
