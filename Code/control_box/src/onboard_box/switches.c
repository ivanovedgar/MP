#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <math.h>
#include "hp_sleep.h"
#include "i2c.h"
#include "gps.h"
#include "gpio14.h"
#include "read_sentence.h"

void relay_init();

typedef struct relay_control {
	uint8_t reg;
	uint8_t pin;
} relay_control;

#define NUM_RELAYS 8

#define DEBUG 

static relay_control relays[NUM_RELAYS] = {
  	{'A',4},
	{'A',6},
	{'A',7},
	{'B',0},
	{'B',2},
	{'B',5},
	{'B',6},
	{'B',7}
  };

int main(int argc,char **argv)
{
  int i,j;

#ifdef DEBUG
	printf("Dave's Switches Program, argc = %d\n", argc);
#endif

	if (argc != 1) {
	  printf("Usage: switches\n");
	  return -1;
	}

	relay_init();

	while (1) {
	    printf("which relay would you like to change (0-7, others exit):");
	    scanf("%d",&i);
	    if ( (i < 0) || (i > 7) ) break;
	    printf("clear (=0) or set (=1):");
	    scanf("%d",&j);
	    if (j == 0) {
	      gpio14_clear_pin(relays[i].reg, relays[i].pin);
            } else if (j == 1) {
	      gpio14_set_pin(relays[i].reg, relays[i].pin);
            };
        }

	return 0;
}

void relay_init()
{
	int i;

	gpio14_init();
	gpio14_select_unit(GPIO14_UNIT_1);
	gpio14_setup_a2d(RIGHT_JUSTIFY_MASK, THREE_ANALOUGE);

	for (i=0 ; i< NUM_RELAYS ; i++ ) {
	    gpio14_enable_output(relays[i].reg, relays[i].pin);
	}

	/*
	for (i=0 ; i< NUM_RELAYS ; i++ ) {
	    gpio14_clear_pin(relays[i].reg, relays[i].pin);
	}
	*/
}
