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
#include "relay_control.h"

typedef struct relay_control {
	uint8_t reg;
	uint8_t pin;
} relay_control;


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

void set_relay(int i) 
{
	      gpio14_set_pin(relays[i].reg, relays[i].pin);
}

void clear_relay(int i)
{
	      gpio14_clear_pin(relays[i].reg, relays[i].pin);
}

int get_relay(int i)
{
	      return gpio14_read_pin(relays[i].reg, relays[i].pin);
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
