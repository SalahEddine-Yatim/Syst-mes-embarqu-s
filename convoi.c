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

//Une variable globale pour stocker la PWM calculer en asservissement 
float PWM_val ;


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
    
    //Bit de direction
    set_bit(DDRD , DDD4);
    set_bit(DDRD , DDD7);
}

void impulse_ultason(void){

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

float mesure_distance(void){

        impulse_ultason();

        while(read_field(PINB , PINB4 , 1) == 0b0)
        {
            //Rien 
        }
        
        //la valeur initiale du TCNT1
        int time0 = TCNT1 ;

        while(read_field(PINB , PINB4 , 1) == 0b1)
        {
            //Rien
        }

        //Temps entre l'emission et la reception d'onde sonore 
        int timer = TCNT1 - time0 ; 

        //Distance mesurer 
        float dist = (float) timer*64.0*170.0/16.0e4 ;

        _delay_us(200);
        _delay_ms(50);

        return dist;

}

void avancer(float vitesse){

    OCR0A = (int) vitesse ;
    OCR0B = (int) vitesse ;

    //Inverser sens OC0B 
    set_bit(PORTD,PD4);

    clear_bit(PORTD,PD7);           
}


void reculer(float vitesse){
    
    OCR0A = (int) vitesse ;
    OCR0B = (int) vitesse ;
    
    //Inverser sens OC0A
    set_bit(PORTD,PD7);  

    clear_bit(PORTD,PD4);
}

void tourner_droite(float vitesse){
    
    OCR0A = vitesse / 1.5;
    OCR0B = (int) vitesse ;

    clear_bit(PORTD,PD7);  

    set_bit(PORTD,PD4);
}

void tourner_guache(float vitesse){
    
    OCR0A = (int) vitesse ;
    OCR0B = vitesse / 1.5;

    clear_bit(PORTD,PD7);  

    set_bit(PORTD,PD4);
}


void demi_tourne(float vitesse){
     
    OCR0A = (int) vitesse ;
    OCR0B = vitesse ;

    set_bit(PORTD,PD7);  
    set_bit(PORTD,PD4);
}

ISR(PCINT0_vect)
{       
    //ROBOT RIGHT OUT 
        if (read_field(PINB , 1 , 2)== 0b10)
        {   
            
            while(read_field(PINB , 1 , 2) != 0b00)
            {
               //Tourner vers la droite
                tourner_droite(PWM_val);
            
            }
        }

    //ROBOT LEFT OUT 
        if (read_field(PINB , 1 , 2)== 0b01)
        {
            //Tourner vers la gauche
            while(read_field(PINB , 1 , 2) != 0b00)
            {
                tourner_guache(PWM_val);
            
            }
        }
    //ROBOT IN 
        if (read_field(PINB , 1 , 2)== 0b00)
        {
            // Asservissement
        }

    //ROBOT OUT 
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
    
    //Gain proportionnel 
    float Kp = 20.0 ;

    //Gain derivateur 
    float Kd = 60.0 ; 
    
    float dist = mesure_distance();
    //Valeur mesurer dans la boucle d'avant 
    float dist_0 = 0.0 ;

    float consigne = 10.0 ;
    
    //vitesse entre deux boucle 
    float vitesse ;

    //Temps entre deux boucle
    float dt ;

   while(true)
   {    
        dist = mesure_distance();

        while ((dist < consigne-0.5) || (dist > consigne + 0.5) )
        {  
            //dt en 10*microsecondes 
            dt = TCNT1*0.4 ;

            //Initialisation du counter pour la nouvelle mesure de dt 
            TCNT1 = 0 ;

            dist = mesure_distance();

            //Calcul de la vitesse absolue 
            if ( dist_0 - dist < 0){

                 vitesse = (dist - dist_0)*1.0e3/dt ;
            }

            else{
                 vitesse = (dist_0 - dist)*1.0e3/dt ;
            }
            
            
           printf(" %d , %d , %d \n", (int)(consigne*10.0), (int)(dist * 10.0), (int)dt);
        

            if(consigne - dist < 0)
            {
                //Saturation
                if(Kp*(dist - consigne) + Kd*vitesse > 255 )
                {
                    avancer(255);
                }
                //Zone morte 
                else if((Kp*(dist - consigne) + Kd*vitesse < 50 ))
                {
                    avancer(50);
                }
                else{
                    avancer(Kp*(dist-consigne )+ Kd*vitesse);
                }
            }

            else if(consigne - dist > 0)
            {   
                //Saturation
                if((Kp*(consigne - dist)+ Kd*vitesse) > 255 )
                {
                    reculer(255);
                }
                //Zone morte 
                if(((Kp*(consigne - dist)+ Kd*vitesse) < 50))
                {
                    reculer(50);
                }

                else {      
                    reculer(Kp*(consigne - dist)+ Kd*vitesse);

                }
            }

            dist_0 = dist ;

            //la valeur du PWM utiliser 
            PWM_val = OCR0A;
        }

        //Stoper hors de la boucle 
        OCR0A = 0 ;
        OCR0B = 0 ;
    }
}





