all:
	LC_ALL=en_US g++ box_interface.c inclinometer_control.c relay_control.c ./drivers/gpio14.c ./common/hp_sleep.c -o ControlBox ./ptuTASS/libPTUTASS.a -lwiringPi -lm -I ./drivers/ -I ./common/