#include <Arduino.h>
#include <avr/io.h>

#include "pwm.h"
#include "adc.h"
#include "timer.h"
#include "switch.h"

// put function declarations here:
int main () {
  sei();

  initTimer0(); //milliseconds

  initPWMTimer3(); //pwm 2

  initSwitchPB3(); //switch

  while (1) {
    
    turnOffSwitchPB3();

    delayMs(1000);

    turnOnSwitchPB3();
  }
}