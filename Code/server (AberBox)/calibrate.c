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
#include "hp_sleep.h"
#include "i2c.h"
#include "gpio14.h"


/* chan0 is side-side inclinometer Y-axis*/
#define HALF_RANGE_CHAN_0 (411.0)
#define CENTRE_CHAN_0 (512.0)
/* chan1 is fore-aft inclinometer X-axis*/
#define HALF_RANGE_CHAN_1 (408.0)
#define CENTRE_CHAN_1 (518.0)

#define DEBUG

typedef struct {
       double fore_aft;
       double side_side;
       } attitude ;

typedef struct {
	double half_range;
	double offset;
	} calibration;

void set_options(calibration *);

attitude get_inclinometer(int count, calibration * calib_array)
{
	double chan0 = 0.0;
	double chan1 = 0.0;
	attitude temp_inclino= {0.0, 0.0};
	int i;
	double temp0, temp1;

	chan0 = 0.0;
	chan1 = 0.0;

	if (count >0 ) {
	  for (i = 0; i < count; i++)
	  {
		chan0 += gpio14_read_a2d(1, 0);
		chan1 += gpio14_read_a2d(1, 1);
#ifdef DEBUG
	  	printf("Raw a2d chan0 = %8.2f:: chan1 = %8.2f\n", chan0, chan1);
	  	temp0 = 1.0;
#endif
	  }
	  chan0 /= (double) count;
	  chan1 /= (double) count;

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
	printf("temp0: %f temp1: %f", temp0, temp1);
#endif
	  temp_inclino.side_side = (180/3.1415926) * asin(temp0);
	  temp_inclino.fore_aft = (180/3.1415926) * asin(temp1);
	} else {
	/* just do nothing and allow the temp_inclino to remain as initialized */
	}

	return temp_inclino;
}

int main(int argc,char **argv)
{
	attitude inclinometer;
	int inclino_capture=1;

	calibration channel_calibration[2]= { {HALF_RANGE_CHAN_0, CENTRE_CHAN_0},
					      {HALF_RANGE_CHAN_1, CENTRE_CHAN_1}};

#ifdef DEBUG
	printf("Dave's version, argc = %d\n", argc);
#endif

	if (argc != 1) {
	  printf("Usage: calibrate\n");
	  return -1;
	}
	
	set_options(channel_calibration);

	gpio14_select_unit(GPIO14_UNIT_1);
	gpio14_setup_a2d(RIGHT_JUSTIFY_MASK, THREE_ANALOUGE);

     while (1) { /* temp loop so I can watch inclinometer */
	inclinometer = get_inclinometer(inclino_capture,channel_calibration); /* and then use these */

#ifdef DEBUG
	printf("\nside_side: %f fore_aft: %f\n", inclinometer.side_side, inclinometer.fore_aft);
#endif

	 sleep(1);
     } /* end of temp loop */

	return 0;
}
/***************************************************************************
 *   Some code here based Upon Code Copyright (C) 2007 by Andrew Shaw      *
 *   ajs@patajs   *
 *                                                                         *
 *   Extra Code by Dave Price, 2009                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/



void set_options(calibration * calib_array)
{
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
		}
		fclose(openfile);
	}    else printf("Failed to open \"configuration.txt\"\n");
	

}
