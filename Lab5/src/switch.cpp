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
 * Initializes pull-up resistor on PB3 and sets it into input mode
 */
void initSwitchPB3(){
    DDRB &= ~(1 << DDB3); //set direction for input
    PORTB |= (1 << PORTB3); //enable pull up resistor
    //set up for pin change interrupt
    //SET THIS PE0 FOR PIN change interrupt PCINT8
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT3);
}