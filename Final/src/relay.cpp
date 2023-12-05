#include <avr/io.h>

void initPortB6() {
    DDRB |= (1 << DDB6);
}

void turnOffMotor() {
    PORTB |= (1 << PB6);
}

void turnOnMotor() {
    PORTB &= ~(1 << PB6);
}
