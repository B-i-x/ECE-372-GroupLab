#include "pwm.h"
#include "timer.h"
// Author: 
//  Alex Romero
//  Victor Oviedo
//  Mason Marrero
//  Omar Ramos
// Date: 11/7
// Assignment: lab 4
//
// Description: get two PWM timers and function to analyze ADC
//----------------------------------------------------------------------//
#include <avr/io.h>


// function that produces an output PWM signal with a variable frequency and duty cycle. For this example
// I am generating a PWM frequency of 1MHz with a 75% duty cycle.
// I will use Timer 1 in the fast PWM mode #14 in Table 17-2, where my TOP value is ICR1
// In this mode I have more flexibility is setting the top value which in turn
// gives me more flexibility in setting the PWM frequency (compared to a fixed TOP value in
// modes 5, 6, and 7 Fast PWM).
// I will have a prescaler of 1.  The calculation of ICR1 and OCR1A are shown below:
void initPWMTimer3()  {

  //set header pin  to output
  DDRE |= (1 << DDE4);
  // set non-inverting mode - output starts high and then is low, 
  //COM1A0 bit = 0
  //COM1A1 bit =1
  TCCR3A |= (1 << COM3B1);
  TCCR3A &= ~(1 << COM3B0);

  //  Use fast PWM mode 10 bit, top value is determined by Table 17-2 of 0x3FF (1023) 
  //  which determines the PWM frequency.
  // for Fast PWM 10bit mode # 15:
  // WGM10 = 1
  // WGM11 = 1
  // WGM12 = 1
  // WGM13 = 1
  TCCR3A |= (1 << WGM30); 

  TCCR3A |= (1 << WGM31);

  TCCR3B |= (1 << WGM32);
  TCCR3B |= (1 << WGM33);

  // set prescalar CSBits to prescaler of 1
  //CS10 =1
  //CS11 =0
  //CS12 =0
  TCCR3B |= (1 << CS31);
  TCCR3B &= ~((1 << CS30)  | (1 << CS32));


  // the last thing is to set the duty cycle.     
  // duty cycle is set by dividing output compare OCR1A value by 1 + TOP value
  // the top value is (1 + ICR1) = 1024
  //  calculate OCR1A value => OCR1A = duty cycle(fractional number) * (1 + TOP) 
  // we want a duty cycle = 60%
  // OCR1A = 0.60 * 1024
}

void changer(int i){
  TCCR3B |= (1 << CS31);
  TCCR3B &= ~((1 << CS30)  | (1 << CS32));
  OCR3A = 1600000 / i;
  OCR3B = OCR3A * .5;
}

void turnOff(){
  TCCR3B &= ~(1 << CS31);
  TCCR3B &= ~((1 << CS30)  | (1 << CS32));
}

