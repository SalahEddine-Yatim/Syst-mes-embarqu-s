#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "macros.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>



void PWM(void){
    //Minimizing Power Consumption” 
    clear_bit(PRR,PRTIM0);

    //PD6 ET PD6 Output 
    set_bit(DDRD , DDD6);
    set_bit(DDRD , DDD5);

    //the Clock Select
    write_field(TCCR0B , CS00 , 3 , 0b100);

    //Choix du mode 
    write_field(TCCR0A , COM0A0 , 2 , 0b10 );
    write_field(TCCR0A , COM0B0 , 2 , 0b10 );

    //Choix du FAST PWM
    write_field(TCCR0A , WGM00 , 2 , 0b11 );
    clear_bit(TCCR0B , WGM02);
}


int main (void)
{  
    PWM();
   
    //Valeur Minimal OCR0A 71 
    int vitesse = 50;
    OCR0A = vitesse ;
    OCR0B = vitesse ;

    //Inverser sens OC0B
    set_bit(DDRD , DDD4);
    set_bit(PORTD,PD4);

    
    while (true)
    {
        
    }

}