#include <Arduino.h>
#include <avr/io.h>

#include "pwm.h"
#include "adc.h"
#include "timer.h"

// put function declarations here:
int main () {
  sei();

  initTimer0(); //milliseconds

  initPWMTimer3(); //pwm 2

  while (1) {
    

  }
}