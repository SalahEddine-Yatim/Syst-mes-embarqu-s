#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "macros.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>

void led_on(void)     { set_bit(PORTB , PORTB5); }
void led_off(void)    { clear_bit(PORTB , PORTB5); }
void led_enable(void) { set_bit(DDRB , DDB5); }
void right_input(void) { clear_bit(DDRB , DDB2); }
void left_input(void) { clear_bit(DDRB , DDB1); }
void PCICR_set(void) { set_bit(PCICR,0);}
void PCMSK0_set(void) { write_field( PCMSK0 , 1 , 2 , 0b11 );}


ISR(PCINT0_vect)
{
    led_on();

}

void initialise(void)
{
    sei();
    right_input();
    left_input();
    led_enable();
    PCICR_set();
    PCMSK0_set();
}

int main (void)
{   
    initialise();

    while (true) 
    {
        sleep_mode() ;
        _delay_ms(800);
        led_off();
        

    }   

}