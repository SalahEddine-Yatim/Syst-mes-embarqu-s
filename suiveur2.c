#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "macros.h"



void led_on(void)     { PORTB |=  (1 << PORTB5); }
void led_off(void)    { PORTB &= ~(1 << PORTB5); }
void led_enable(void) { DDRB  |=  (1 << DDB5); }
void right_in(void) { DDRB  &=  ~(1 << DDB2); }
void left_in(void) { DDRB  &=  ~(1 << DDB1); }

int main (void)
{
        right_in();
        left_in();
        led_enable();

    while (true) {

        _delay_ms(500);
        
        if (read_field(PINB , 1 , 2)== 0b11)
        {
            printf("OUT \n");
            led_off();
        }

        if (read_field(PINB , 1 , 2)== 0b10)
        {   
            led_off();
            printf("LEFT \n");
        }

        if (read_field(PINB , 1 , 2)== 0b01)
        {
            led_off();
            printf("RIGHT \n");
        }

        if (read_field(PINB , 1 , 2)== 0b00)
        {
            led_on();();
            printf("IN \n");
        }
        
    }   
}