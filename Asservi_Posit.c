#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "macros.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>

void PWM(void)
{
    // Minimizing Power Consumption”
    clear_bit(PRR, PRTIM0);

    // PD6 ET PD6 Output
    set_bit(DDRD, DDD6);
    set_bit(DDRD, DDD5);

    // the Clock Select
    write_field(TCCR0B, CS00, 3, 0b100);

    // Choix du mode
    write_field(TCCR0A, COM0A0, 2, 0b10);
    write_field(TCCR0A, COM0B0, 2, 0b10);

    // Choix du FAST PWM
    write_field(TCCR0A, WGM00, 2, 0b11);
    clear_bit(TCCR0B, WGM02);

    set_bit(DDRD, DDD4);
    set_bit(DDRD, DDD7);
}

void impulse(void)
{

    // TIMER
    write_field(TCCR1B, CS10, 3, 0b011);

    clear_bit(PORTB, PORTB4);
    set_bit(DDRB, DDB4);

    _delay_us(2);

    set_bit(PORTB, PORTB4);

    _delay_us(10);

    clear_bit(PORTB, PORTB4);

    clear_bit(DDRB, DDB4);
}

float mesure_distance(void)
{
    int boucle = 0;

    impulse();

    while (read_field(PINB, PINB4, 1) == 0b0)
    {
        // Rien
    }

    int time0 = TCNT1;

    while (read_field(PINB, PINB4, 1) == 0b1)
    {
        //Rien
    }

    int timer = TCNT1 - time0;

    float dist = (float)timer * 64.0 * 170.0 / 16.0e4;

    _delay_us(200);
    _delay_ms(50);

    return dist;
}

void avancer(float vitesse)
{
    // Valeur Minimal OCR0A 71

    OCR0A = (int)vitesse;
    OCR0B = (int)vitesse;

    // Inverser sens OC0B
    set_bit(PORTD, PD4);

    clear_bit(PORTD, PD7);
}

void reculer(float vitesse)
{
    // Valeur Minimal OCR0A 71
    OCR0A = (int)vitesse;
    OCR0B = (int)vitesse;

    set_bit(PORTD, PD7);

    clear_bit(PORTD, PD4);
}

int main(void)
{
    PWM();
    // Gain proportionnel
    float Kp = 20.0;

    // Gain derivateur
    float Kd = 60.0;
    
    float dist = mesure_distance();
    float dist_0 = 0.0;
    float consigne ;
    float Consigne[] = {20.0 , 25.0 , 30.0 , 25.0 , 20.0} ;
    float vitesse;
    float dt = 0.0;

    for (int i = 0 ; i < 5 ; i++){

        consigne = Consigne[i] ;
    
        dist = mesure_distance();

        printf(" %d , %d , %d \n", (int)(consigne*10.0), (int)(dist * 10.0), (int)dt);
        
        while ((dist < consigne - 0.5) || (dist > consigne + 0.5))
        {
            // dt en microseconde/10
            dt = TCNT1 * 0.4;
            TCNT1 = 0;

            dist = mesure_distance();

            
            if ( dist_0 - dist < 0){

                 vitesse = (dist - dist_0)*1.0e3/dt ;
            }
            
            else{
                 vitesse = (dist_0 - dist)*1.0e3/dt ;
            }
            

            printf(" %d , %d , %d \n", (int)(consigne*10.0), (int)(dist * 10.0), (int)dt);

            if (consigne - dist < 0)
            {
                // Saturation
                if (Kp * (dist - consigne) + Kd * vitesse > 255)
                {
                    avancer(255);
                }
                // Zone morte
                else if ((Kp * (dist - consigne) + Kd * vitesse < 50))
                {
                    avancer(75);
                }
                else
                {
                    avancer(Kp * (dist - consigne) + Kd * vitesse);
                }
            }

            else if (consigne - dist > 0)
            {
                // Saturation
                if ((Kp * (consigne - dist) + Kd * vitesse) > 255)
                {
                    reculer(255);
                }
                // Zone morte
                else if (((Kp * (consigne - dist) + Kd * vitesse) < 50))
                {
                    reculer(75);
                }

                else
                {
                    reculer(Kp * (consigne - dist) + Kd * vitesse);
                }
            }

            dist_0 = dist;
        }


        // Stoper hors de la boucle
        OCR0A = 0;
        OCR0B = 0;

        _delay_ms(600);

    }
    
}