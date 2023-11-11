#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "macros.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>


    
void initialize_CAN(void){

    //disabel PRR REDUCTION D'ENERGIE  
    clear_bit(PRR , PRADC);
    //Utiliser CANAL ADC6
    write_field(ADMUX , MUX0 , 4 , 0b0110);
    //Utiliser 0 jusqua 5V plage 
    write_field(ADMUX , REFS0 , 2 , 0b01);
    //Autoriser la converssion 
    set_bit(ADCSRA , ADEN);
    //Indique le deviseur d'hiorloge
    write_field(ADCSRA , ADPS0 , 3 , 0b111);
    //Start Conversion 
    set_bit(ADCSRA , ADSC);

    
}

int main (void)
{  
    while (true) 
    {
        initialize_CAN();

        int resultat = ADC ;

        printf("LA VALEUR DU VOLTAGE SUR 1023 EST : %d \n", resultat);

        //SHUT down ADC (PRR)
        set_bit(PRR , PRADC);
        _delay_ms(500);

    }   

}