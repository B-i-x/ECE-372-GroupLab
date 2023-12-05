#include <Arduino.h>
#include <avr/io.h>

#include "pwm.h"
#include "adc.h"
#include "timer.h"

// put function declarations here:
int main () {
  sei();

  initPWMTimer3(); //pwm 2

}