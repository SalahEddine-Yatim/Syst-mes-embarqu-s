#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "macros.h"


void right_in(void) { DDRB  &=  ~(1 << DDB2); }
void left_in(void) { DDRB  &=  ~(1 << DDB1); }


bool left_is_on_track()  
{
    return read_field(PINB , 1 , 1) == 0b1 ;
}


bool right_is_on_track() 
{

    return read_field(PINB , 2 , 1) == 0b1 ;
}


int main (void)
{
        right_in();
        left_in();
    while (true) {

        _delay_ms(500);

        if (left_is_on_track())
        {
            printf("left is on track \n");
        }

        _delay_ms(500);

        if (right_is_on_track())
        {
            printf("right is on track \n");
        }

        
    }   
}