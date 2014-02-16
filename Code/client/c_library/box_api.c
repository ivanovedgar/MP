#include "box_api.h"
#include "box_api_private.h"

static int box_socket;  /* an internal private global to hold the socket descriptor */

int box_connect(char * hostname, int portnumber) {
    struct sockaddr_in sa;
    struct hostent *hp;
    int s;

    if (( hp = gethostbyname(hostname)) == NULL) {
        errno = ECONNREFUSED;
        return (0);
    }

    /*Set Address*/

    memset( &sa, 0, sizeof (struct sockaddr_in) );
    memcpy(&sa.sin_addr, * hp->h_addr_list, sizeof (sa.sin_addr));

    sa.sin_family = hp->h_addrtype;
    sa.sin_port = htons( (u_short)portnumber);

    if (( box_socket = socket(hp->h_addrtype, SOCK_STREAM, 0) ) <0)
        return(0);
    if( connect(box_socket,(struct sockaddr *)&sa, sizeof sa) <0)
        return(0);
    return(1);
}


/**
  * Disconnects from control box
  */
void box_disconnect() {
    if (0 < box_socket) {
        
        sendText(EXIT);
        close(box_socket);
    }
}

/**
 * Shutdown the whole control box
 */
void box_shutdown() {
    if (0 < box_socket) {
        sendText(SHUTDOWN);
        close(box_socket);
    }
}

/**
 * Set all the relay on the control box
 * return 1/0 for success/failure
 */
int setRelays(int value) {
    char reply[2];
    sendText(SET_RELAY_SETTINGS);
    writeInt(value);
    readText(2, reply);
    if (strncmp(reply, OK, 2)!= 0) {
        printf("Unable to set relays");
        return 0;
    }
    return 1;
}

/**
 * Set a single relay on the control box
 * return 1/0 for success/failure
 */
int setRelayN(int relay) {
    char reply[2];

    sendText(SET_RELAY_N);
    writeInt(relay);

    readText(2, reply);
    if (strncmp(reply, OK, 2)!= 0) {
        printf("Unable to set relay %d\n", relay);
        return 0;
    }
    return 1;
}

/**
 * Reset a single relay on the control box
 * return 1/0 for success/failure
 */
int resetRelayN(int relay) {
    char reply[2];
    sendText(RESET_RELAY_N);
    writeInt(relay);
    readText(2, reply);
    if (strncmp(reply, OK, 2)!= 0) {
        printf("Unable to reset relay %d\n", relay);
        return 0;
    }
    return 1;
}

/**
 * Get the current relay settings from the control box
 * return The relay settings
 */
int getRelays() {
    int settings;
    sendText(GET_RELAY_SETTINGS);
    settings = readInt();
    return settings;
}

/**
 * Get the current relay settings from the control box
 * return The relay settings
 */
int getRelayN(int relay) {
    int settings;
    sendText(GET_RELAY_N);
    writeInt(relay);
    settings = readInt();
    return settings;
}

/**
 * This does nothing useful at present.  It's just a place
 * holder to prove we can send floats both ways.
 *
 * Sets the pan and the tilt values of the PanCam.
 * @param pan The pan between 0 and 360, with 0 looking backwards over the
 * left of the rover.
 * @param tilt The tilt between 0 and 180, with 0 looking down.
 * @return The actual pan and tilt values respectively in the array.
 */
void setPanTilt(float pan, float tilt, float * retVal) {
    sendText(SET_PAN_TILT_POSITIONS);
    writeFloat(pan);
    writeFloat(tilt);
    retVal[0] = readFloat();
    retVal[1] = readFloat();
    sendText(OK);
}

/**
 * Get the current values from the inclinometer
 * @return The actual side-side and fore-aft values respectively in the array.
 */
void getInclinometer(float * retVal) {
    sendText(GET_INCLINOMETER);
    retVal[0] = readFloat();
    retVal[1] = readFloat();
}

/**
 * Sends some text to the agent interface.
 * @param text The text to send.
 */
static void sendText(char * text) {
    /* --- send text to socket --- */
    if (send(box_socket, text , strlen(text), 0) != strlen(text)){
        perror("send() of text failed");
    }
}

/**
 * Sends some text to the agent interface.
 * @param text The text to send.
 */
static void sendString(char * text) {
    int string_length = strlen(text);
    string_length = htonl(string_length);
    /* --- send length to socket --- */
    if (send(box_socket, &string_length , sizeof(int), 0) != sizeof(int)){
        perror("send() of text failed");
    }
    sendText(text);
}

/**
 * Reads a given number of characters from the channel.
 * @param length The number of characters to read.
 * @return The resultant string.
 */
static char * readText(int length, char * message) {
     int size;

     /* --- Receive text string of known length from control box --- */

     if ((size = recv(box_socket, message, length, 0)) < 0){
         perror("recv() of known length string failed");
         return (char *) 0;
     }
     return message;
}

/**
 * Reads a int from the channel.
 * @return The resultant int.
 */
static int readInt() {
    char buffer[4];
    int value;
    /* --- Receive four bytes from control box to use as an integer --- */
    if (recv(box_socket, buffer, 4, 0) != 4){
        perror("recv() of integer failed");
        return INT_MIN;
    }
    value = ntohl(* (int *) buffer);
    return value;
}

/**
 * Reads a float from the channel.
 * @return The resultant float.
 */
static float readFloat() {
    char buffer[4];
    int ivalue;
    float fvalue;

    /* --- Receive four bytes from control box to use as a float --- */

    if (recv(box_socket, buffer, 4, 0) != 4){
        perror("recv() of float failed");
        return FLT_MIN;
    }

    ivalue = ntohl(* (int *) buffer);
    fvalue = * ((float *) &ivalue);
    return fvalue;
}

/**
 * Writes a int into the buffer and writes it to the channel.
 * @param value The int.
 */
static void writeInt(int value) {
    int i;
    i = htonl(value);

    /* --- send 4 bytes to socket --- */
    if (send(box_socket, &i , sizeof(int), 0) != sizeof(int)){
        perror("send() of integer failed");
    }
}

/**
 * Writes a float into the buffer and writes it to the channel.
 * @param value The float.
 */
static void writeFloat(float value) {
    int * i_ptr;
    int i;
    i_ptr = (int *) &value;
    i = htonl(*i_ptr);

    /* --- send 4 bytes to socket --- */
    if (send(box_socket, &i , sizeof(int), 0) != sizeof(int)){
        perror("send() of float failed");
    }
}
