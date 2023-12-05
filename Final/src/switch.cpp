#include "switch.h"
// Author: 
//  Alex Romero
//  Victor Oviedo
//  Mason Marrero
//  Omar Ramos
// Date: 11/7
// Assignment: lab 4
//
// Description: use PD0 as a button, an INT button
//----------------------------------------------------------------------//

#include <avr/io.h>

/*
 * Initializes pull-up resistor on PB3 and sets it into output mode
 */
void initRelaySwitch(){
    DDRD &= ~(1<< DDD0); // set direction for input
    PORTD |= (1 << PORTD0);  // enable the pullup resistor for stable input

    EICRA |= ((1 << ISC01) | (1 << ISC00)); // set INT0 to trigger on rising edge
    
    EIMSK |= (1 << INT0); // enable INT0
}

void initServoSwitch(){
    DDRD &= ~(1<< DDD1); // set direction for input
    PORTD |= (1 << PORTD1);  // enable the pullup resistor for stable input


    EICRA |= ((1 << ISC11) | (1 << ISC10)); // set INT0 to trigger on rising edge
    
    EIMSK |= (1 << INT1); // enable INT0
}

