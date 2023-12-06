#include <avr/io.h>

void initRelay() {    
    DDRB |= (1 << DDB6);
}

void turnOffMotor() {
    PORTB |= (1 << PORTB6);
}

void turnOnMotor() {
    PORTB &= ~(1 << PORTB6);
}
