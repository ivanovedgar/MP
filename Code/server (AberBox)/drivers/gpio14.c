/**
RPi routines for the GPIO14
*/
#include <stdio.h>
#include <wiringPiI2C.h>
#include "wiringPi.h"
#include "hp_sleep.h"
#include "gpio14.h"

/* Global Variables */
static uint8_t address=GPIO14_UNIT_0;
static uint8_t porta_mask;
static uint8_t portb_mask;
static uint8_t porta;
static uint8_t portb;
static int fd;

void gpio14_init(void)
{
    fd = wiringPiI2CSetup(address);
    porta = wiringPiI2CReadReg8(fd,GPIO14_PORTA);
    portb = wiringPiI2CReadReg8(fd,GPIO14_PORTB);
}

/*
execute a command on the gpio14's command register
*/
void gpio14_command(uint8_t command)
{
    wiringPiI2CWriteReg8(fd,GPIO14_COMMAND,command);
}

/*
set's a given pin to be an output
*/
void gpio14_enable_output(uint8_t port,uint8_t pin)
{
    switch(port)
    {
        case 'A':
            //clear the appropriate bit
            porta_mask &= ~(1<<pin);
            wiringPiI2CWriteReg8(fd,GPIO14_PORTA_MASK,porta_mask);
            break;
        case 'B':
            //clear the appropriate bit
            portb_mask &= ~(1<<pin);
            wiringPiI2CWriteReg8(fd,GPIO14_PORTB_MASK,portb_mask);
            break;
    }
}

/*
set's a given pin to be an input
*/
void gpio14_enable_input(uint8_t port,uint8_t pin)
{
    switch(port)
    {
        case 'A':
            //set the appropriate bit
            porta_mask |= (1<<pin);
            wiringPiI2CWriteReg8(fd,GPIO14_PORTA_MASK,porta_mask);
            break;
        case 'B':
            //set the appropriate bit
            portb_mask |= (1<<pin);
            wiringPiI2CWriteReg8(fd,GPIO14_PORTB_MASK,portb_mask);
            break;
    }
}

/*
    set a given pin on one of the ports
    note that some pins are not useable as they are used for I2C or are input only
*/
void gpio14_set_pin(uint8_t port,uint8_t pin)
{
    switch(port)
    {
        case 'A':
            porta |= (1<<pin);
            wiringPiI2CWriteReg8(fd,GPIO14_PORTA,porta);
            break;
        case 'B':
            portb |= (1<<pin);
            wiringPiI2CWriteReg8(fd,GPIO14_PORTB,portb);
            break;
    }
}

/*
    clear a given pin on one of the ports
    note that some pins are not useable as they are used for I2C or are input only
*/
void gpio14_clear_pin(uint8_t port,uint8_t pin)
{
    switch(port)
    {
        case 'A':
            porta &= ~(1<<pin);
            wiringPiI2CWriteReg8(fd,GPIO14_PORTA,porta);
            break;
        case 'B':
            portb &= ~(1<<pin);
            wiringPiI2CWriteReg8(fd,GPIO14_PORTB,portb);
            break;
    }
}



/*
    read a given pin from one of the ports. Returns 0 or 1 depending on the value of the pin.
    note that some pins are not useable as they are used for I2C
*/
uint8_t gpio14_read_pin(uint8_t port,uint8_t pin)
{
    uint8_t value=0;


    switch(port)
    {
        case 'A':
            value=wiringPiI2CReadReg8(fd,GPIO14_PORTA);
            break;
        case 'B':
            value=wiringPiI2CReadReg8(fd,GPIO14_PORTB);
            break;
    }

    if(value & (1<<pin))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
sets the value for an entire port
note: some pins aren't actually writeable
*/
void gpio14_set_port(uint8_t port,uint8_t value)
{
    switch(port)
    {
        case 'A':
            //clear the appropriate bit
            wiringPiI2CWriteReg8(fd,GPIO14_PORTA,value);
            break;
        case 'B':
            //clear the appropriate bit
            wiringPiI2CWriteReg8(fd,GPIO14_PORTB,value);
            break;
    }
}

/* sets up the a2d control register, use one of the masks in gpio14.h and one of the 4 configuration options*/
void gpio14_setup_a2d(uint8_t justify_mask,uint8_t config)
{
    config = config + justify_mask;
    wiringPiI2CWriteReg8(fd,GPIO14_A2D_CONTROL,config);
}

/*
    reads from an a2d line, the line must be setup appropriately using gpio14_setup_a2d
    the justify mask used in gpio14_setup_a2d must agree with the choice of whether or not
    to take a 10 bit reading.
*/
unsigned int gpio14_read_a2d(uint8_t use_10_bits, uint8_t channel)
{
    unsigned int value;
    switch(channel)
    {
	case 0:
	    gpio14_command(GET_AD0);
	    break;
	case 1:
	    gpio14_command(GET_AD1);
	    break;
	case 2:
	    gpio14_command(GET_AD2);
	    break;
	case 3:
	    gpio14_command(GET_AD3);
	    break;
	case 4:
	    gpio14_command(GET_AD4);
	    break;
    }
    hp_sleep(0.05);
    /*if we want a 10 bit reading we need to get the lower 8 bits and shift what we've already read to the 
    most signifcant byte*/
    if(use_10_bits)
    {
       value = wiringPiI2CReadReg8(fd,GPIO14_PORTA_MASK);
       //shift the bits to make them the MSB
       value = value << 8;

       //porta mask register is also result high byte
       value = value + wiringPiI2CReadReg8(fd,GPIO14_PORTB_MASK);
    }
    else
    {
        value = wiringPiI2CReadReg8(fd,GPIO14_PORTA_MASK);
    } 
    return value;
}

/*
changes the i2c address of a gpio14
*/
void gpio14_change_i2c_address(uint8_t new_i2c_address)
{
    /* Start the I2C address changing procedure */
    wiringPiI2CWriteReg8(fd,GPIO14_CHANGE_ADDR,0xA0);
    wiringPiI2CWriteReg8(fd,GPIO14_CHANGE_ADDR,0xAA);
    wiringPiI2CWriteReg8(fd,GPIO14_CHANGE_ADDR,0xA5);

    wiringPiI2CWriteReg8(fd,GPIO14_CHANGE_ADDR,new_i2c_address);

    /* Make the new i2c address the active one. */
    address=new_i2c_address;
    gpio14_init();
}

void gpio14_select_unit(uint8_t gpio14_address)
{
    /* New address validity check */

    if( (gpio14_address<0x20 || gpio14_address>0X27) && gpio14_address != 0 )  { return; }
    //if(gpio14_address%2) { return; }

    /* Make the new i2c address the active one. */
    address=gpio14_address;
    gpio14_init();
}

void gpio14_set_pwm(uint8_t width)
{
    wiringPiI2CWriteReg8(fd,GPIO14_PWM,width);
}
