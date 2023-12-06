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
    DDRE &= ~(1<< DDE5); // set direction for input
    PORTE |= (1 << PORTE5);  // enable the pullup resistor for stable input

    EICRB |= ((1 << ISC51) | (1 << ISC50)); // set INT0 to trigger on rising edge
    
    EIMSK |= (1 << INT5); // enable INT0
}

