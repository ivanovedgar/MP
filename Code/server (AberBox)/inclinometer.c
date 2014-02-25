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
//#include "hp_sleep.h"
//#include "i2c.h"
//#include "gps.h"
#include "gpio14.h"
//#include "read_sentence.h"


/* chan0 is side-side inclinometer Y-axis*/
#define HALF_RANGE_CHAN_0 (411.0)
#define CENTRE_CHAN_0 (511.5)
/* chan1 is fore-aft inclinometer X-axis*/
#define HALF_RANGE_CHAN_1 (408.0)
#define CENTRE_CHAN_1 (516.0)

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
	int i = 0;
	attitude inclinometer;
	int inclino_skip=50;
	int inclino_capture=50;

	char *currstr;
	gps_GGA *fix=NULL;
	int gps_capture=10;
	int gps_dataindex=0;
	int gps_speed=0;

	FILE * log;

	double average_northing=0.0, average_easting=0.0;
	double average_altitude=0.0;

	calibration channel_calibration[2]= { {HALF_RANGE_CHAN_0, CENTRE_CHAN_0},
					      {HALF_RANGE_CHAN_1, CENTRE_CHAN_1}};

#ifdef DEBUG
	printf("Dave's version, argc = %d\n", argc);
#endif

	if (argc != 7) {
	  printf("Usage: inclinometer filename inclino_skip inclino_capture gps_speed gps_capture index_number\n");
	  return -1;
	}
	
	set_options(channel_calibration);

	if((log = fopen(argv[1],"a"))==NULL) return -2;

	inclino_skip=atoi(argv[2]);
	inclino_capture=atoi(argv[3]);
	gps_speed=atoi(argv[4]);
	gps_capture=atoi(argv[5]);
	gps_dataindex=atoi(argv[6]);

	/* gpio14_init(); */
	gpio14_select_unit(GPIO14_UNIT_1);
// 	gpio14_select_unit(GPIO14_UNIT_0);
	/* gpio14_setup_a2d(RIGHT_JUSTIFY_MASK, ALL_ANALOUGE); */
	gpio14_setup_a2d(RIGHT_JUSTIFY_MASK, THREE_ANALOUGE);

	inclinometer = get_inclinometer(inclino_skip,channel_calibration); /* throw these away */

     while (1) { /* temp loop so I can watch inclinometer */
	inclinometer = get_inclinometer(inclino_capture,channel_calibration); /* and then use these */

#ifdef DEBUG
	printf("\nside_side: %f fore_aft: %f\n", inclinometer.side_side, inclinometer.fore_aft);
#endif

	fprintf(log,"%5d %+07.3f %+07.3f ", gps_dataindex, inclinometer.side_side, inclinometer.fore_aft);
	 sleep(1);
     } /* end of temp loop */

	if (gps_capture > 0 ) {
	  /* setup_GPS(speed,device); */
	  setup_GPS( gps_speed, "/dev/ttyS2");
#ifdef DEBUG
			printf("GPS setup: gps_fd = %d\n", gps_fd);
#endif

	  for (i = 0; i < gps_capture; i++)
	  {
		while (((fix = process_gps_sentence( currstr = read_nmea_sentence(gps_fd)))==NULL) || (fix->valid ==0))
#ifdef DEBUG
			 { if( fix != NULL )
			          printf("still awaiting GPS fix, valid = %d\n", fix->valid);
			   else
			          printf("still awaiting GPS fix, process_gps_sentence returned null\n");
#endif
	 	            /*do nothing */ ;
#ifdef DEBUG
			 }
#endif
#ifdef DEBUG
			printf("northing=%f easting=%f time %s altitude %f altitude units %c fix validity %c\n",
			fix->degrees_north, fix->degrees_east, fix->time, fix->altitude, fix->AUNITS, fix->valid);
#endif
			average_northing += fix->degrees_north;
			average_easting += fix->degrees_east;
			average_altitude += fix->altitude;
	  }

	  average_northing /= gps_capture;
	  average_easting /= gps_capture;
	  average_altitude /= gps_capture;
	} else {
	  /* They asked for no GPS values so leave averages as initialized */
	  /* Need to fill in a couple of fields in a dummy fix though */
	  fix = calloc(1, sizeof(gps_GGA));
	  fix->AUNITS = 'Z';
	  strcpy(fix ->time, "000.0000");
        }

	fprintf(log,"%+013.8f %+013.8f %+013.8f %c %s\n",
	average_northing, average_easting, average_altitude,fix->AUNITS, fix->time);
	
	fclose(log);
	
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
