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
void initSwitchPB3(){
    DDRB |= (1 << DDB3); //set direction for output
}

void turnOnSwitchPB3(){
    PORTB |= (1 << PORTB3); //set output to high
}

void turnOffSwitchPB3(){
    PORTB &= ~(1 << PORTB3); //set output to low
}