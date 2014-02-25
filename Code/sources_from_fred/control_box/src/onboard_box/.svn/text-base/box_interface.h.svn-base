#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>  /* close() */
#include <string.h>  /* memset() */
#include <stddef.h>
#include <time.h>
#include <stdlib.h>
#include <termios.h>
/* #include <bits/time.h> */
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <signal.h>

#define COMM_PORT       4675            /* port number used for communication */
#define RCV_CMD_LENGTH  4               /* string size used for the command in the comms */
#define REPLY_LENGTH    2               /* string size used for the replies OK/ER in the comms */
#define RCVMESSAGE      255             /* string size for messages */
#define MAXPENDING      5               /* Maximum outstanding connection requests */

/* Structure for communications details */
typedef struct {
	int serversocket;                    /* Socket descriptor for server */
	int clientsocket;                    /* Socket descriptor for client */
	int port;			/* port number for communications */
	char *server_ip_address; 			/* IP address of server */
	struct sockaddr_in server_address; 	/*  server address */
	struct sockaddr_in client_address; 	/*  server address */    
} COM_STRUCT;

// float ConvertFloatChar(float comfloat);
// float ConvertCharFloat(char* mess);
void ConvertFloatString(float comfloat, char* comstring);
void ConvertStringFloat(char* comstring, float* mess);
int ConvertCharInt(char* mess);
void ERROR_Message(char *error);
