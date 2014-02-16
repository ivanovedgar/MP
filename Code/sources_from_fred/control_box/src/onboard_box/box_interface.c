#include "box_interface.h"
#include "relay_control.h"
#include "inclinometer_control.h"

int main(int argc, char *argv[])
{
	int recvMsgSize;                    	/* Size of received message */
	char Received_buf[RCV_CMD_LENGTH];     	/* Buffer for echo string */
 	unsigned int client_length;            	/* Length of client address data structure */
	char c[2], *message;
    	float fmessage;
    	int imessage;
	int stringLength;			/* length of next string to follow this int */

	COM_STRUCT comms;
	int status;

	float f_value_1, f_value_2;
	int relay, relay_settings, relay_state;

	attitude inclinometer;

	/* initialize relays etc */
	relay_init();

	inclinometer_init();

	message = malloc(sizeof (char) * RCVMESSAGE);
	/* --- Malloc the space to the required data --- */

	/* --- Set up the communication port information --- */
    	comms.server_ip_address = "192.168.0.1";          	/* server IP address  */
    	comms.port = 4675;	/* server PORT address */
	printf("Port %d\n", comms.port);
	/* --- Create socket for incoming connections --- */
	if ((comms.serversocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		ERROR_Message("socket() failed");
	}
	
	/* --- Construct local address structure --- */
	memset(&comms.server_address, 0, sizeof(comms.server_address));   /* Zero out structure */
	comms.server_address.sin_family = AF_INET;                	/* Internet address family */
	comms.server_address.sin_addr.s_addr = htonl(INADDR_ANY); 	/* Any incoming interface */
	comms.server_address.sin_port = htons(comms.port);     /* Local port */
	
	/* --- Bind to the local address --- */
	if (bind(comms.serversocket, (struct sockaddr *) &comms.server_address, sizeof(comms.server_address)) < 0){
		ERROR_Message("bind() failed");
		return EXIT_FAILURE;
	}

	/* --- Mark the socket so it will listen for incoming connections --- */
	if (listen(comms.serversocket, MAXPENDING) < 0){
		ERROR_Message("listen() failed");
		return EXIT_FAILURE;
	}

	/* --- Set the size of the in-out parameter --- */
        client_length = sizeof(comms.client_address);

   while (1) { /* start of loop for client requests */

	printf("Waiting for connection ..........\n");
	/* --- Wait for a client to connect --- */
	if ((comms.clientsocket = accept(comms.serversocket,
            (struct sockaddr *) &comms.client_address, &client_length)) < 0){
		ERROR_Message("accept() failed");
		return EXIT_FAILURE;
	}

	printf("Handling client %s\n", inet_ntoa(comms.client_address.sin_addr));
	
	while (1){ /* Run forever */
		printf("At start of receive loop\n");
		sprintf(Received_buf, "    ");
		/* --- Receive message from client --- */
		if ((recvMsgSize = recv(comms.clientsocket, Received_buf, RCV_CMD_LENGTH, 0)) <= 0){
			ERROR_Message("recv() failed");
                        break;
        	}
		printf("Receive loop - recv returned %d\n", recvMsgSize);
		if (strncmp(Received_buf, "EEEE", RCV_CMD_LENGTH) == 0){
			break;
        	}
    		printf("Command received :: %s :\n", Received_buf);
		if (strncmp(Received_buf, "EXIT", RCV_CMD_LENGTH) == 0){
    			printf("Exit Command received :: %s :\n", Received_buf);
			break;
        	}
		/* --- Check what the request is, and return the desired info --- */
		if (strncmp(&Received_buf[0],"SPTP",RCV_CMD_LENGTH) == 0){
			/* --- Receive Pan value from client --- */
			if ((recvMsgSize = recv(comms.clientsocket, message, RCV_CMD_LENGTH, 0)) < 0){
				ERROR_Message("recv() failed");
			}
			/* convert from network byte order */
			f_value_1 = ConvertCharFloat(message);
			printf("Pan value %f\n", f_value_1);
			/* --- Receive Tilt value from client --- */
			if ((recvMsgSize = recv(comms.clientsocket, message, RCV_CMD_LENGTH, 0)) < 0){
				ERROR_Message("recv() failed");
			}
			/* convert from network byte order */
			f_value_2 = ConvertCharFloat(message);
			printf("Tilt value %f\n", f_value_2);

			fmessage = ConvertFloatChar(f_value_1);
			/* --- Now return the actual angle values --- */
			if (send(comms.clientsocket, &fmessage, RCV_CMD_LENGTH, 0) != RCV_CMD_LENGTH){
				ERROR_Message("send() Pan value failed");
			}
			printf("Pan value sent %f\n", f_value_1);

			/* convert to network byte order */
			fmessage = ConvertFloatChar(f_value_2);
			/* --- Now return the actual angle values --- */
			if (send(comms.clientsocket, &fmessage, RCV_CMD_LENGTH, 0) != RCV_CMD_LENGTH){
				ERROR_Message("send() Tilt value failed");
			}
			printf("Tilt value sent %f\n", f_value_2);

			/* --- Receive OK value from client if the message has been received --- */
			if ((recvMsgSize = recv(comms.clientsocket, message, REPLY_LENGTH, 0)) < 0){
				ERROR_Message("recv() failed");
			}
			if (strncmp(message, "OK", REPLY_LENGTH) != 0){
				printf("Pan and Tilt values not received :(\n");
			} else {
                	printf("Pan tilt OK\n");
            		}
		}
		if (strncmp(&Received_buf[0],"GRLS",RCV_CMD_LENGTH) == 0){
			/* --- client has asked for status of all relays --- */

			relay_settings = 0;
			for (relay = 0; relay < NUM_RELAYS ; relay++) {
				relay_state = get_relay(relay);
				relay_settings |= (relay_state << relay);
			}

			/* convert to network byte order */
			imessage = htonl(relay_settings);
			if (send(comms.clientsocket, &imessage, RCV_CMD_LENGTH, 0) != RCV_CMD_LENGTH){
				ERROR_Message("send() Tilt value failed");
			}
			printf("Relay status sent %d\n", relay_settings);
                }

		if (strncmp(&Received_buf[0],"GRLN",RCV_CMD_LENGTH) == 0){
			/* --- client has asked for status of a specified relay --- */

			/* --- Receive relay number value from client --- */
			if ((recvMsgSize = recv(comms.clientsocket, message, RCV_CMD_LENGTH, 0)) < 0){
				ERROR_Message("recv() failed");
			}

			/* convert from network byte order */
			relay = ConvertCharInt(message);

			printf("Client asked about relay %d status\n", relay);

			/* get relay state and convert to network byte order */
			relay_state = get_relay(relay);
			imessage = htonl( relay_state );

			if (send(comms.clientsocket, &imessage, RCV_CMD_LENGTH, 0) != RCV_CMD_LENGTH){
				ERROR_Message("send() relay value failed");
			}
			printf("Relay status sent %d\n", relay_state);
                }

		if (strncmp(&Received_buf[0],"SRLS",RCV_CMD_LENGTH) == 0){
			/* --- client has asked to set value of all relays --- */

			/* --- Receive relay settings value from client --- */
			if ((recvMsgSize = recv(comms.clientsocket, message, RCV_CMD_LENGTH, 0)) < 0){
				ERROR_Message("recv() failed");
			}
			/* convert from network byte order */
			relay_settings = ConvertCharInt(message);
			printf("Asked to set relays to %d\n", relay_settings);

			for (relay = 0; relay < NUM_RELAYS ; relay++) {
				if (relay_settings&0x1) set_relay(relay);
						else    clear_relay(relay);
				relay_settings >>= 1;
			}

                        /* --- Now return an ok process done --- */
                        if (send(comms.clientsocket, "OK", strlen("OK"), 0) != strlen("OK")){
                                ERROR_Message("send() relay settings OK failed");
                        }
                        printf("ok returned\n");

                }

		if (strncmp(&Received_buf[0],"SRLN",RCV_CMD_LENGTH) == 0){
			/* --- client has asked to set relay N --- */

			/* --- Receive relay number from client --- */
			if ((recvMsgSize = recv(comms.clientsocket, message, RCV_CMD_LENGTH, 0)) < 0){
				ERROR_Message("recv() failed");
			}
			/* convert from network byte order */
			relay = ConvertCharInt(message);

			printf("Client asked to set relay %d\n", relay);

			set_relay(relay);

                        /* --- Now return an ok process done --- */
                        if (send(comms.clientsocket, "OK", strlen("OK"), 0) != strlen("OK")){
                                ERROR_Message("send() relay settings OK failed");
                        }
                        printf("ok returned\n");

                }

		if (strncmp(&Received_buf[0],"RRLN",RCV_CMD_LENGTH) == 0){
			/* --- client has asked to reset relay N --- */

			/* --- Receive relay number from client --- */
			if ((recvMsgSize = recv(comms.clientsocket, message, RCV_CMD_LENGTH, 0)) < 0){
				ERROR_Message("recv() failed");
			}
			/* convert from network byte order */
			relay = ConvertCharInt(message);

			printf("Client asked to reset relay %d\n", relay);

			clear_relay(relay);

                        /* --- Now return an ok process done --- */
                        if (send(comms.clientsocket, "OK", strlen("OK"), 0) != strlen("OK")){
                                ERROR_Message("send() relay settings OK failed");
                        }
                        printf("ok returned\n");

                }

		if (strncmp(&Received_buf[0],"GINC",RCV_CMD_LENGTH) == 0){
			/* --- client has asked for state of inclinometer --- */

			inclinometer = get_inclinometer();

			f_value_1 = inclinometer.side_side;
                        f_value_2 = inclinometer.fore_aft ;

			/* convert to network byte order */
			fmessage = ConvertFloatChar(f_value_1);
			if (send(comms.clientsocket, &fmessage, RCV_CMD_LENGTH, 0) != RCV_CMD_LENGTH){
				ERROR_Message("send() Pan value failed");
			}

			/* convert to network byte order */
			fmessage = ConvertFloatChar(f_value_2);
			/* --- Now return the actual angle values as the sevos are not exact --- */
			if (send(comms.clientsocket, &fmessage, RCV_CMD_LENGTH, 0) != RCV_CMD_LENGTH){
				ERROR_Message("send() Tilt value failed");
			}

			printf("Inlinometer sent %f %f\n", f_value_1, f_value_2);
                }

	}

	status = close(comms.clientsocket);    /* Close client socket */

	printf("Closing client socket %d\n", status);

	if (strncmp(Received_buf, "EEEE", RCV_CMD_LENGTH) == 0){
    		printf("Command received :: %s :\n", Received_buf);
    		printf("Shutting Down Control Box System\n");
		break;
       	}

   } /* start of loop for client requests */

	status = close(comms.serversocket);    /* Close server socket */

	printf("Closing server socket %d\n", status);
	
	return EXIT_SUCCESS;
}

/* --- Function to process the command line options --- */
int process_commandline(int argc, char *argv[])
{
	int i, port;
printf("Argc %d\n", argc);
  	if((argc > 3) || (argc == 2)){
		printf("*************************************************************************************\n");
		printf("*   camera_interface -port  [connection port number]				    *\n");
		printf("*************************************************************************************\n");
		return 0;
	} else if (argc == 1){
		printf("*************************************************************************************\n");
		printf("*   WARNING YOU HAVE NOT ENTERED A PORT NUMBER ::: DEFAULT 4675   WILL BE USED      *\n");
		printf("*************************************************************************************\n");
		return COMM_PORT;
	}
	for (i=1; i < argc; i++)
	{
		if (strcmp("-port",argv[i]) == 0){
			i++;
			port = atoi(argv[i]);
		}
	}
	return port;
}

float ConvertFloatChar(float comfloat){
    uint32_t l = htonl(*((uint32_t*)&comfloat));
    float f = *((float*)&l);
    return f;
}

float ConvertCharFloat(char* mess){
    uint32_t l = ntohl(*((uint32_t*)mess));
    float f = *((float*)&l);
    return f;
}

/* Next function added at Aberystwyth */
int ConvertCharInt(char* mess){
    int i = ntohl(*((int*)mess));
    return i;
}

/* --- Function to output any error messages --- */
void ERROR_Message(char *error)
{
	printf("ERROR Command :: %s\n",error);
}
