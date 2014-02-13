#include <errno.h>	/* Various Include files ... */
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#define SET_PAN_TILT_POSITIONS "SPTP"
#define GET_RELAY_SETTINGS "GRLS"
#define SET_RELAY_SETTINGS "SRLS"
#define GET_RELAY_N "GRLN"
#define SET_RELAY_N "SRLN"
#define RESET_RELAY_N "RRLN"
#define GET_INCLINOMETER "GINC"
#define OK "OK"
#define EXIT "EXIT"
#define SHUTDOWN "EEEE"

static void sendText(char * text);
static void sendString(char * text);
static char * readText(int length, char * message);
static int readInt();
static float readFloat();
static void writeInt(int value);
static void writeFloat(float value);
