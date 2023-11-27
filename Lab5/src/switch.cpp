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
 * Initializes pull-up resistor on PD0 and sets it into input mode
 */
void initSwitchPD0(){
    DDRD &= ~(1 << PD0); // set PD0 to input
    PORTD &= ~(1 << PORTD0); // set pull-up resistor on PD0

    // SET Pin PDO to call interrupt on pin change
    EICRA |= (1<<ISC00);
    EICRA &= ~(1<<ISC01);
    // Set pin mask to PD0 (INT0)
    EIMSK |= (1<<INT0);
}