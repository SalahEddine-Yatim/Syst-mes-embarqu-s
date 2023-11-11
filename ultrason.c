#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "macros.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>


void impulse(void){

    //TIMER
    write_field(TCCR1B , CS10 , 3 , 0b011);


    clear_bit(PORTB , PORTB4) ;
    set_bit(DDRB , DDB4);  
    
    _delay_us(2);

    set_bit(PORTB , PORTB4);

    _delay_us(10);

    clear_bit(PORTB , PORTB4) ;
        
    clear_bit(DDRB , DDB4) ;

}

int main (void)
{  
    while (true) 
    {
        int boucle = 0 ;

        impulse();

        while(read_field(PINB , PINB4 , 1) == 0b0)
        {
            //Rien 
        }

        TCNT1 = 0 ;

        while(read_field(PINB , PINB4 , 1) == 0b1)
        {
            boucle++;
        }

        int timer = TCNT1 ; 

        float dist = ( timer/64 ) * 340 *0.0001 ;

        _delay_us(200);
        _delay_ms(50);

        printf("%d , %f \n" ,timer , dist );

        _delay_ms(500);

    }   

}