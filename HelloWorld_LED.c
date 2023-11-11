#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
 
#define MORSE_DELAY_MS 250

void led_on(void)     { PORTB |=  (1 << PORTB5); }
void led_off(void)    { PORTB &= ~(1 << PORTB5); }
void led_enable(void) { DDRB  |=  (1 << DDB5); }

void morse_pulse(char dot_or_dash)
{
    led_on();
    _delay_ms(MORSE_DELAY_MS);
    if (dot_or_dash=='-')
        _delay_ms(MORSE_DELAY_MS * 2); 
    led_off();
    _delay_ms(MORSE_DELAY_MS);
}

int main (void)
{
    char msg[] = "......-...-..---";
    led_enable();
    while (true) {
        for (int i=0; msg[i]; i++)
            morse_pulse(msg[i]);

        _delay_ms(5000);
    }   
}