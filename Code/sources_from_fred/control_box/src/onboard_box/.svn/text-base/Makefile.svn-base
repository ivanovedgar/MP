#############################################################################
#	Makefile for boat control systems
#############################################################################

#setup debugging
ifeq ($(DEBUG),y)
	CFLAGS += -O -g # -O is need to expand inlines
else
	CFLAGS += -O2
endif

#configure envionrment variables
PWD       := $(shell pwd)

# GUMSTIX_BUILDROOT	= /home/colin/gumstix-buildroot
# GUMSTIX_BUILDROOT	= /home/cjs06/gumstix-buildroot
GUMSTIX_BUILDROOT	= /home/cjs06/gumstix-1161
BUILD_ARM		= $(GUMSTIX_BUILDROOT)/build_arm_nofpu
CROSS_COMPILE		= $(BUILD_ARM)/staging_dir/bin/arm-linux-
TARGET_ARCH=-Os -march=armv5te -mtune=xscale -Wa,-mcpu=xscale -lm -lpthread
# TARGET_ARCH=-Os -march=armv5te -mtune=xscale -Wa,-mcpu=xscale -lm -lpthread -static -static-libgcc
#TARGET_ARCH=-lm -g -lpthread
CC = $(CROSS_COMPILE)gcc
CFLAGS += -Wall
CFLAGS += -D GUMSTIX
#CFLAGS += -D TAZ

I2C	= $(PWD)/i2c
COMMON = $(PWD)/common
UART	= $(PWD)/uart
DRIVERS	= $(PWD)/drivers
THREADS	= $(PWD)/threads
#TAZ	= $(PWD)/taz
#ARC	= $(PWD)/arc

#vpath %.c $(I2C) $(COMMON) $(DRIVERS)/lcd03 $(DRIVERS)/gpio14 $(DRIVERS)/ds1621 $(DRIVERS)/cmps03 $(DRIVERS)/pg500 $(DRIVERS)/rowind $(DRIVERS)/nmea $(DRIVERS)/md03 $(DRIVERS)/gm862_gps $(DRIVERS)/taz_rudder $(DRIVERS)/taz_sail  $(DRIVERS)/gumstix_gpio $(UART)/uart $(PWD)/arc $(TAZ) $(THREADS)
vpath %.c $(COMMON) $(I2C) $(DRIVERS)/gpio14

#CPPFLAGS += -I . -I $(COMMON) -I $(I2C) -I $(DRIVERS)/lcd03 -I $(DRIVERS)/gpio14 -I $(DRIVERS)/gumstix_gpio -I $(DRIVERS)/ds1621 -I $(DRIVERS)/cmps03 -I $(DRIVERS)/gps -I $(DRIVERS)/taz_rudder -I $(DRIVERS)/taz_sail -I $(DRIVERS)/md03 -I $(UART) -I $(DRIVERS)/pg500 -I $(DRIVERS)/rowind -I $(DRIVERS)/nmea -I $(THREADS) -I $(TAZ) -I $(ARC) 
CPPFLAGS += -I . -I $(COMMON) -I $(I2C) -I $(DRIVERS)/gpio14



#target definitions

all : box_interface

#calibrate: $(PWD)/udp_send.o $(I2C)/i2c-api.o $(DRIVERS)/rowind/rowind.o $(DRIVERS)/pg500/pg500.o $(DRIVERS)/gpio14/gpio14.o  $(DRIVERS)/gumstix_gpio/gumstix_gpio.o $(DRIVERS)/md03/md03.o $(DRIVERS)/taz_rudder/taz_rudder_lowlevel.o $(DRIVERS)/taz_sail/taz_sail_lowlevel.o $(COMMON)/hp_sleep.o $(UART)/uart.o $(DRIVERS)/gm862_gps/great_circle.o $(DRIVERS)/gm862_gps/waypoint.o $(DRIVERS)/gm862_gps/gm862_gps.o $(DRIVERS)/gps/gps.o  $(DRIVERS)/gm862_gps/at_command.o $(DRIVERS)/nmea/read_sentence.o $(TAZ)/taz_init.o $(THREADS)/thread_lock.o $(THREADS)/gps_thread.o $(THREADS)/init_mutexes.o

calibrate : $(DRIVERS)/gpio14/gpio14.o $(I2C)/i2c-api.o $(COMMON)/hp_sleep.o

#inclinometer: $(PWD)/udp_send.o $(I2C)/i2c-api.o $(DRIVERS)/rowind/rowind.o $(DRIVERS)/pg500/pg500.o $(DRIVERS)/gpio14/gpio14.o  $(DRIVERS)/gumstix_gpio/gumstix_gpio.o $(DRIVERS)/md03/md03.o $(DRIVERS)/taz_rudder/taz_rudder_lowlevel.o $(DRIVERS)/taz_sail/taz_sail_lowlevel.o $(COMMON)/hp_sleep.o $(UART)/uart.o $(DRIVERS)/gm862_gps/great_circle.o $(DRIVERS)/gm862_gps/waypoint.o $(DRIVERS)/gm862_gps/gm862_gps.o $(DRIVERS)/gps/gps.o  $(DRIVERS)/gm862_gps/at_command.o $(DRIVERS)/nmea/read_sentence.o $(TAZ)/taz_init.o $(THREADS)/thread_lock.o $(THREADS)/gps_thread.o $(THREADS)/init_mutexes.o
#inclinometer :  $(PWD)/relay_control.o $(PWD)/inclinometer_control.o $(DRIVERS)/gpio14/gpio14.o $(I2C)/i2c-api.o $(COMMON)/hp_sleep.o

#switches: $(PWD)/udp_send.o $(I2C)/i2c-api.o $(DRIVERS)/rowind/rowind.o $(DRIVERS)/pg500/pg500.o $(DRIVERS)/gpio14/gpio14.o  $(DRIVERS)/gumstix_gpio/gumstix_gpio.o $(DRIVERS)/md03/md03.o $(DRIVERS)/taz_rudder/taz_rudder_lowlevel.o $(DRIVERS)/taz_sail/taz_sail_lowlevel.o $(COMMON)/hp_sleep.o $(UART)/uart.o $(DRIVERS)/gm862_gps/great_circle.o $(DRIVERS)/gm862_gps/waypoint.o $(DRIVERS)/gm862_gps/gm862_gps.o $(DRIVERS)/gps/gps.o  $(DRIVERS)/gm862_gps/at_command.o $(DRIVERS)/nmea/read_sentence.o $(TAZ)/taz_init.o $(THREADS)/thread_lock.o $(THREADS)/gps_thread.o $(THREADS)/init_mutexes.o
switches :  $(PWD)/inclinometer_control.o $(DRIVERS)/gpio14/gpio14.o $(I2C)/i2c-api.o $(COMMON)/hp_sleep.o

#testbytes: $(PWD)/udp_send.o $(I2C)/i2c-api.o $(DRIVERS)/rowind/rowind.o $(DRIVERS)/pg500/pg500.o $(DRIVERS)/gpio14/gpio14.o  $(DRIVERS)/gumstix_gpio/gumstix_gpio.o $(DRIVERS)/md03/md03.o $(DRIVERS)/taz_rudder/taz_rudder_lowlevel.o $(DRIVERS)/taz_sail/taz_sail_lowlevel.o $(COMMON)/hp_sleep.o $(UART)/uart.o $(DRIVERS)/gm862_gps/great_circle.o $(DRIVERS)/gm862_gps/waypoint.o $(DRIVERS)/gm862_gps/gm862_gps.o $(DRIVERS)/gps/gps.o  $(DRIVERS)/gm862_gps/at_command.o $(DRIVERS)/nmea/read_sentence.o $(TAZ)/taz_init.o $(THREADS)/thread_lock.o $(THREADS)/gps_thread.o $(THREADS)/init_mutexes.o
testbytes :  $(PWD)/relay_control.o $(PWD)/inclinometer_control.o $(DRIVERS)/gpio14/gpio14.o $(I2C)/i2c-api.o $(COMMON)/hp_sleep.o

#box_interface: $(PWD)/box_interface.c $(PWD)/box_interface.h $(PWD)/relay_control.o $(PWD)/inclinometer_control.o $(PWD)/udp_send.o $(I2C)/i2c-api.o $(DRIVERS)/rowind/rowind.o $(DRIVERS)/pg500/pg500.o $(DRIVERS)/gpio14/gpio14.o  $(DRIVERS)/gumstix_gpio/gumstix_gpio.o $(DRIVERS)/md03/md03.o $(DRIVERS)/taz_rudder/taz_rudder_lowlevel.o $(DRIVERS)/taz_sail/taz_sail_lowlevel.o $(COMMON)/hp_sleep.o $(UART)/uart.o $(DRIVERS)/gm862_gps/great_circle.o $(DRIVERS)/gm862_gps/waypoint.o $(DRIVERS)/gm862_gps/gm862_gps.o $(DRIVERS)/gps/gps.o  $(DRIVERS)/gm862_gps/at_command.o $(DRIVERS)/nmea/read_sentence.o $(TAZ)/taz_init.o $(THREADS)/thread_lock.o $(THREADS)/gps_thread.o $(THREADS)/init_mutexes.o
box_interface : $(PWD)/relay_control.o $(PWD)/inclinometer_control.o $(DRIVERS)/gpio14/gpio14.o $(I2C)/i2c-api.o $(COMMON)/hp_sleep.o

clean:
	rm -rf *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions calibrate switches box_interface
	find . -name "*.o" -exec rm {} \;

depend .depend dep:
	$(CC) $(CFLAGS) $(CPPFLAGS) -lm -M  $(PWD)/inclinometer.c > .depend
#	$(CC) $(CFLAGS) $(CPPFLAGS) -lm -M $(UART)/uart.c $(I2C)/i2c-api.c $(COMMON)/hp_sleep.c  $(DRIVERS)/gpio14/gpio14.c $(DRIVERS)/gumstix_gpio/gumstix_gpio.c $(DRIVERS)/pg500/pg500.c $(DRIVERS)/rowind/rowind.c $(DRIVERS)/nmea/read_sentence.c $(DRIVERS)/gm862_gps/gm862_gps.c $(DRIVERS)/gm862_gps/at_command.c $(DRIVERS)/gm862_gps/great_circle.c $(DRIVERS)/gm862_gps/waypoint.c $(DRIVERS)/taz_rudder/taz_rudder_lowlevel.c $(DRIVERS)/taz_sail/taz_sail_lowlevel.c $(THREADS)/compass_thread.c $(THREADS)/gps_thread.c $(THREADS)/rudder_thread.c $(THREADS)/sailing_thread.c $(THREADS)/telemetry_thread.c $(THREADS)/init_mutexes.c $(THREADS)/thread_lock.c $(TAZ)/taz_compass.c  $(TAZ)/taz_init.c $(TAZ)/taz_rudder.c $(TAZ)/taz_sail.c $(TAZ)/taz_sailing.c $(TAZ)/taz_wind.c $(TAZ)/taz_init.c $(PWD)/taz_controller.c $(PWD)/simple_remote.c $(PWD)/set_rudder_raw.c $(PWD)/inclinometer.c $(PWD)/whole_thing.c $(PWD)/heading_hold_with_tack.c $(PWD)/udp_send.c $(PWD)/austria.c > .depend


FORCE:

.PHONY: FORCE

PREPROCESS.c = $(CC) $(CPPFLAGS) $(TARGET_ARCH) -E -Wp,-C,-dD,-dI 

%.pp : %.c FORCE
	$(PREPROCESS.c) $< > $@

	
ifeq ($(strip $(filter clean, $(MAKECMDGOALS))),)
-include .depend
endif

