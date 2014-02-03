#include <stdio.h>
#include <stdlib.h>

#include "box_api.h"

#define DEBUG 

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

	if ( box_connect("192.168.0.1",4675) == -1 ) {
	   printf("Failed to connect to box");
	   exit(EXIT_FAILURE);
	 }

	while (1) {
	    printf("which relay would you like to change (0-7, others exit):");
	    scanf("%d",&i);
	    if ( (i < 0) || (i > 7) ) break;
	    printf("clear (=0) or set (=1):");
	    scanf("%d",&j);
	    if (j == 0) {
	      resetRelayN(i);
            } else if (j == 1) {
	      setRelayN(i);
            };
        }

	return 0;
}
