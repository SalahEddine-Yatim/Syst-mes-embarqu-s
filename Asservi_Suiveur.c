#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "macros.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>


void right_input(void) { clear_bit(DDRB , DDB2); }
void left_input(void) { clear_bit(DDRB , DDB1); }
void PCICR_set(void) { set_bit(PCICR,0);}
void PCMSK0_set(void) { write_field( PCMSK0 , 1 , 2 , 0b11 );}




void initialise(void)
{
    sei();
    right_input();
    left_input();
    PCICR_set();
    PCMSK0_set();
}

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

    set_bit(DDRD , DDD4);
    set_bit(DDRD , DDD7);
}



void avancer(float vitesse){
    //Valeur Minimal OCR0A 71 

    OCR0A = (int) vitesse ;
    OCR0B = (int) vitesse ;

    //Inverser sens OC0B
    set_bit(PORTD,PD4);

    clear_bit(PORTD,PD7);           
}


void reculer(float vitesse){
    //Valeur Minimal OCR0A 71 
    OCR0A = (int) vitesse ;
    OCR0B = (int) vitesse ;

    set_bit(PORTD,PD7);  

    clear_bit(PORTD,PD4);
}

void tourner_droite(float vitesse){
    //Valeur Minimal OCR0A 71
    OCR0A = vitesse / 4.5;
    OCR0B = (int) vitesse ;

    clear_bit(PORTD,PD7);  

    set_bit(PORTD,PD4);
}

void tourner_guache(float vitesse){
    //Valeur Minimal OCR0A 71 
    OCR0A = (int) vitesse ;
    OCR0B = vitesse / 4.5 ;

    clear_bit(PORTD,PD7);  

    set_bit(PORTD,PD4);
}

void demi_tourne(float vitesse){
    //Valeur Minimal OCR0A 71 
    OCR0A = (int) vitesse ;
    OCR0B = vitesse ;

    set_bit(PORTD,PD7);  

    set_bit(PORTD,PD4);
}

ISR(PCINT0_vect)
{

        if (read_field(PINB , 1 , 2)== 0b10)
        {   
            
            while(read_field(PINB , 1 , 2) != 0b00)
            {
               //Tourner vers la droite
                tourner_droite(150);
            
            }
        }

        if (read_field(PINB , 1 , 2)== 0b01)
        {
            //Tourner vers la gauche
            while(read_field(PINB , 1 , 2) != 0b00)
            {
                tourner_guache(150);
            
            }
        }

        if (read_field(PINB , 1 , 2)== 0b00)
        {
            avancer(150);
        }

        if (read_field(PINB , 1 , 2)== 0b11)
        {
             while(read_field(PINB , 1 , 2) == 0b11)
            {
               //Tourner vers la droite
                demi_tourne(150);
            
            }
        }
}



int main (void)
{   
    PWM();
    initialise();

    while(true)
    {    
    sleep_mode();
    }
}

