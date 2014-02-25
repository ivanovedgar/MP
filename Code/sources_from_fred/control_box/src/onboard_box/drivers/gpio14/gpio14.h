#ifndef GPIO14_H
#define GPIO14_H     1

#include <stdint.h>

//address ranges
#define GPIO14_UNIT_0   0x20
#define GPIO14_UNIT_1   0x21
#define GPIO14_UNIT_2   0x22
#define GPIO14_UNIT_3   0x23
#define GPIO14_UNIT_4   0x24
#define GPIO14_UNIT_5   0x25
#define GPIO14_UNIT_6   0x26
#define GPIO14_UNIT_7   0x27

/* register positions */
#define GPIO14_COMMAND 0
#define GPIO14_PORTA_MASK 1
#define GPIO14_PORTB_MASK 2
#define GPIO14_A2D_CONTROL 3
#define GPIO14_PORTA 4
#define GPIO14_PORTB 5
#define GPIO14_PWM 6
#define GPIO14_CHANGE_ADDR 7

#define RA0_SET 16
#define RA0_CLEAR 32
#define RA0_TOGGLE 48
#define RA1_SET 17
#define RA1_CLEAR 33
#define RA1_TOGGLE 49
#define RA2_SET 18
#define RA2_CLEAR 34
#define RA2_TOGGLE 50
#define RA3_SET 19
#define RA3_CLEAR 35
#define RA3_TOGGLE 51
#define RA4_SET 20
#define RA4_CLEAR 36
#define RA4_TOGGLE 52
#define RA6_SET 22
#define RA6_CLEAR 38
#define RA6_TOGGLE 54
#define RA7_SET 23
#define RA7_CLEAR 39
#define RA7_TOGGLE 55

#define RB0_SET 24
#define RB0_CLEAR 40
#define RB0_TOGGLE 56
#define RB1_SET 25
#define RB1_CLEAR 41
#define RB1_TOGGLE 57
#define RB2_SET 26
#define RB2_CLEAR 42
#define RB2_TOGGLE 58
#define RB3_SET 27
#define RB3_CLEAR 43
#define RB3_TOGGLE 59
#define RB5_SET 29
#define RB5_CLEAR 45
#define RB5_TOGGLE 61
#define RB6_SET 30
#define RB6_CLEAR 46
#define RB6_TOGGLE 62
#define RB7_SET 31
#define RB7_CLEAR 47
#define RB7_TOGGLE 63

#define NO_OP   0
#define PULLB_ON    1
#define PULLB_OFF   2
#define GET_AD0     3
#define GET_AD1     4
#define GET_AD2     5
#define GET_AD3     6
#define GET_AD4     7


//masks for bits 0-3 of the a2d control register

//AN0-4 are all A/D inputs
#define ALL_ANALOUGE 0
//AN3,AN1 and AN0 are analouge
#define THREE_ANALOUGE 4
//AN0-4 are all digital pins
#define ALL_DIGITAL 6
//AN0 is analouge
#define ONE_ANALOUGE 14

//masks for bit 6/7 of the a2d control register
#define RIGHT_JUSTIFY_MASK 192
#define LEFT_JUSTIFY_MASK 64

/* Function Declaration */
void gpio14_command(uint8_t command);
void gpio14_select_unit(uint8_t gpio14_address);
void gpio14_init(void);
void gpio14_change_i2c_address(uint8_t new_i2c_address);
uint8_t gpio14_read_pin(uint8_t port,uint8_t pin);
void gpio14_clear_pin(uint8_t port,uint8_t pin);
void gpio14_set_pin(uint8_t port,uint8_t pin);
void gpio14_enable_input(uint8_t port,uint8_t pin);
void gpio14_enable_output(uint8_t port,uint8_t pin);
void gpio14_set_port(uint8_t port,uint8_t value);
void gpio14_setup_a2d(uint8_t justify_mask,uint8_t config);
unsigned int gpio14_read_a2d(uint8_t use_10_bits, uint8_t channel);
void gpio14_set_pwm(uint8_t width);

#endif
