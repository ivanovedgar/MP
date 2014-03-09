#include <string.h>
#include <strings.h>
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
#include <wiringPiI2C.h>
#include "wiringPi.h"
#include "gpio14.h"
#include "inclinometer_control.h"


/* chan0 is side-side inclinometer Y-axis*/
#define HALF_RANGE_CHAN_0 (411.0)
#define CENTRE_CHAN_0 (511.5)
/* chan1 is fore-aft inclinometer X-axis*/
#define HALF_RANGE_CHAN_1 (408.0)
#define CENTRE_CHAN_1 (516.0)

 #define DEBUG

typedef struct {
	double half_range;
	double offset;
	} calibration;

static calibration calib_array[2]= { {HALF_RANGE_CHAN_0, CENTRE_CHAN_0},
					      {HALF_RANGE_CHAN_1, CENTRE_CHAN_1}};
static int number_to_average = 1;

attitude get_inclinometer()
{
	double chan0 = 0.0;
	double chan1 = 0.0;
	attitude temp_inclino= {0.0, 0.0};
	int i;
	double temp0, temp1;

	chan0 = 0.0;
	chan1 = 0.0;

	if (number_to_average >0 ) {
	  for (i = 0; i < number_to_average; i++)
	  {
		chan0 += gpio14_read_a2d(1, 0);
		chan1 += gpio14_read_a2d(1, 1);
	  }
	  
	  chan0 /= (double) number_to_average;
	  chan1 /= (double) number_to_average;

	  temp0 = (chan0 - calib_array[0].offset) / calib_array[0].half_range;
	  temp1 = (chan1 - calib_array[1].offset) / calib_array[1].half_range;
	  if (temp0 > 1.0) {
#ifdef DEBUG
	  	printf("fixing temp0 from %f to 1.0\n", temp0);
	  	temp0 = 1.0;
#endif
	  } else if (temp0 < -1.0) {
#ifdef DEBUG
	  	printf("fixing temp0 from %f to -1.0\n", temp0);
	  	temp0 = -1.0;
#endif
	  }
	  	

	  if (temp1 > 1.0) {
#ifdef DEBUG
	  	printf("fixing temp1 from %f to 1.0\n", temp1);
	  	temp1 = 1.0;
#endif
	  } else if (temp1 < -1.0) {
#ifdef DEBUG
	  	printf("fixing temp1 from %f to -1.0\n", temp1);
	  	temp1 = -1.0;
#endif
	  }

#ifdef DEBUG
	printf("chan0: %f chan1: %f  ", chan0, chan1);
	printf("temp0: %f temp1: %f\n", temp0, temp1);
#endif
	  temp_inclino.side_side = (180/3.1415926) * asin(temp0);
	  temp_inclino.fore_aft = (180/3.1415926) * asin(temp1);
	} else {
	/* just do nothing and allow the temp_inclino to remain as initialized */
	}

	return temp_inclino;
}

void inclinometer_init() {

	FILE *openfile = NULL;
	char line[200], func[20];
	double value;
	
#ifdef DEBUG
	printf("Loading configuration\n");
#endif
	
	openfile = fopen("configuration.txt","r");
		
	if (openfile != NULL){	
		while (fgets(line, 99, openfile) != NULL){
			sscanf(line,"%s %lf", func, &value);
			if (strcmp(func,"side_side_midpoint") == 0){
				/* set side to side midpoint */
				calib_array[0].offset = value;
#ifdef DEBUG
				printf("calib_array[0].offset=%f\n",
					calib_array[0].offset);
#endif
			}
			if (strcmp(func,"side_side_half_range") == 0){
				/* set side to side half range */
				calib_array[0].half_range = value;
#ifdef DEBUG
				printf("calib_array[0].half_range=%f\n",
					calib_array[0].half_range);
#endif
			}
			if (strcmp(func,"fore_aft_midpoint") == 0){
				/* set fore aft midpoint */
				calib_array[1].offset = value;
#ifdef DEBUG
				printf("calib_array[1].offset=%f\n",
					calib_array[1].offset);
#endif
			}
			if (strcmp(func,"fore_aft_half_range") == 0){
				/* set fore aft half range */
				calib_array[1].half_range = value;
#ifdef DEBUG
				printf("calib_array[1].half_range=%f\n",
					calib_array[1].half_range);
#endif
			}
			if (strcmp(func,"number_to_average") == 0){
				/* set the number of raw reading to average*/
				number_to_average = value;
#ifdef DEBUG
				printf("number to average = %f converted to %d\n",
					value, number_to_average);
#endif
			}
		}
		fclose(openfile);
	}    else printf("Failed to open \"configuration.txt\"\n");
	

}
