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
  DDRE |= (1 << DDE3);
  // set non-inverting mode - output starts high and then is low, 
  //COM1A0 bit = 0
  //COM1A1 bit =1
  TCCR3A |= (1 << COM3A1);
  TCCR3A &= ~(1 << COM3A0);

  //  Use fast PWM mode 10 bit, top value is determined by Table 17-2 of 0x3FF (1023) 
  //  which determines the PWM frequency.
  // for Fast PWM 10bit mode # 14:
  // WGM10 = 0
  // WGM11 = 1
  // WGM12 = 1
  // WGM13 = 1
  TCCR3A &= ~(1 << WGM30); 

  TCCR3A |= (1 << WGM31);

  TCCR3B |= (1 << WGM32);
  TCCR3B |= (1 << WGM33);

  // PWM frequency calculation for FAST PWM mode on page 148 of datasheet
  //frequency of PWM = (F_clk)/((Prescaler)* (1 +TOP))
  // frequency of PWM = 16Mhz
 
  ICR3 = 39999;

  // set prescalar CSBits to prescaler of 8
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
  // OCR1A = 0.60 * 39999
  OCR3A =  8000;
}

void changeDutyCycle(double dutycycle1){
  OCR3A = int(dutycycle1 * (39999));
}



void setServoAngle(int angle) {
    // Check if the angle is within the valid range
    if (angle < 0 || angle > 180) {
        return; // Exit the function if the angle is not valid
    }

    // Map the angle to the corresponding OCR3A value
    // Pulse width ranges from 1ms (1000us) to 2ms (2000us)
    // OCR3A value for 1ms pulse width at 16MHz with prescaler of 8 and TOP 39999
    int ocrValueFor1ms = 2000; // Adjust this value based on your PWM frequency and prescaler settings

    // OCR3A value for 2ms pulse width
    int ocrValueFor2ms = 4000; // Adjust this value based on your PWM frequency and prescaler settings

    // Calculate the OCR3A value for the given angle
    int ocrValue = ocrValueFor1ms + (angle / 180) * (ocrValueFor2ms - ocrValueFor1ms);

    // Set OCR3A to the calculated value
    OCR3A = ocrValue;
}

void letBallThrough() {
  // for (double i = 0; i < 0.5; i = i + 0.01) {
  //     // setServoAngle(i);
  //     changeDutyCycle(i);
  //     delayMs(50);
  //   }

  setServoAngle(0); 

  delayMs(100);  
  
  setServoAngle(180); 

  delayMs(100); 

  setServoAngle(0); 
 
  delayMs(100); 

}
// void analyzeADC() {

//   while(! ((1 << 4) & ADCSRA)) {//waiting for ADC to be ready
//   }

//   unsigned int adcResult = ADCL; //getting adc result p1
//   adcResult += ((unsigned int) ADCH) << 8;  //getting adc result p2

//   double percentage = adcResult / 1024.0; //getting the adc percentage
//   double dc1, dc2;

//   if (percentage < 0.5) {
//     dc1 = 0.0;
//     dc2 = (percentage * 2.0);
//   }
//   else {
//     dc1 = ((percentage - 0.5) * 2.0);
//     dc2 = 1.0; //for some reason, putting this at 100 makes it go to 0
//   }

//   // changeDutyCycle(dc1, dc2);
//   return;
// }
