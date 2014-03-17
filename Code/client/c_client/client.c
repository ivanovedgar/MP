#include <stdio.h>
#include <stdlib.h>

#include "box_api.h"
#include "client.h"

void do_thing();
void playWithRelays();
void menu();
int checkValidity(int,int,int);
float incl[2] =  {0.0,0.0};
int main()
{
   int s;
   char hostname[BUFFSIZE];
   int port_number;
/*
   printf("WHICH HOST COMPUTER ?:");
   scanf("%s",hostname);
   printf("WHICH port number ?:");
   scanf("%d", &port_number);

   box_connect(hostname,port_number);
*/
	box_connect("192.168.1.40",4675);
    while(1){
	menu();
	}
   box_disconnect();
   
   return EXIT_SUCCESS;
}

void menu(){
	int choice, relay;
	unsigned int status;
	
	printf("This is a menu for demo\n");
	printf("1 Set all relays\n");
	printf("2 Reset all relays\n");
	printf("3 Set relay N\n");
	printf("4 Reset relay N\n");
	printf("5 Play with relays\n");
	printf("6 Get relay N status\n");
	printf("7 Get status of all relays\n");
	printf("8 Get inclinometer values\n");
	scanf("%d", &choice);

	if(checkValidity(1,8, choice) == 1){	
		
		switch(choice){
		case 1:
				setRelays(255);
				break;
		case 2:
				setRelays(0);
				break;
		case 3:
				printf("Eneter relay number to set\n");
				scanf("%d", &relay);
				setRelayN(relay);
				break;
		case 4:
				printf("Eneter relay number to reset\n");
				scanf("%d", &relay);
				
				resetRelayN(relay);
				break;
		case 5:
				playWithRelays();
				break;
		case 6:
				printf("Enter relay number from 0 to 7\n");
				scanf("%d", &relay);
				status = getRelayN(relay);
				printf("The status is %d\n", status);
				break;
		case 7:
				status = getRelays();
				printf("The status of all relays is %d\n", status);
				break;
		case 8:
				getInclinometer(incl);
                printf("side-side= %f fore-aft = %f\n", incl[0], incl[1]);
		}
	}else{
	printf("Wrong entry, try again values from 0 to 7\n");
	choice = 0;
	}
}

int checkValidity(int bottomValue, int topVallue, int suppliedValue){
	int response;

	if((suppliedValue >= bottomValue) && (suppliedValue <= topVallue)){
	response = 1;
	}else{
	response = 0;
	}
	return response;
}

void playWithRelays(){
	int relayCount;
	int i =0;
		while(i < 3){
			for(relayCount = 0; relayCount <= 7; relayCount++){
			resetRelayN(relayCount);
		}
			for(relayCount = 0; relayCount <=7; relayCount++){
			setRelayN(relayCount);
			}
			i++;
		}
	setRelays(0);
}

void do_thing(){
	int choice;
	while(1){
		printf("Hello choose option\n");
		printf("1 Reset Relay N\n");
		printf("2 Set Relay N\n");
		scanf("%d", &choice);
		if(choice == 1){
			int relayToReset;
			printf("What relay you want to reset?");
			scanf("%d", &relayToReset);
			resetRelayN(relayToReset);
		}
		else if(choice == 2){
			int relayToSet;
			printf("What relay you want to set?");
			scanf("%d", &relayToSet);
			setRelayN(relayToSet);
		}
	}
}

void do_things() {
int temp, s;
        float values[2] =  {0.0,0.0};
		setPanTilt(2.7F,4.3F,values);
        printf("value 0 = %f value 1 = %f\n", values[0], values[1]);

        s = getRelays();
        printf("current relay settings = %d\n", s);
        if (setRelays(109)) printf("relays set o.k.\n");
                        else printf("relays NOT set\n");
		temp = 4;
                if (setRelayN(temp)) printf("relay %d set o.k.\n", temp);
                        else printf("relay %d NOT set\n", temp);
                s = getRelays();
                printf("current relay settings = %d\n", s);
		temp = 3;
                s = getRelayN(temp);
                printf("current relay %d settings = %d\n", temp, s);
                if (resetRelayN(5)) printf("relay N reset o.k.\n");
                        else printf("relay N NOT set\n");
		getInclinometer(values);
                printf("side-side= %f fore-aft = %f\n", values[0], values[1]);
		// box_disconnect();
		// box_shutdown();
}

