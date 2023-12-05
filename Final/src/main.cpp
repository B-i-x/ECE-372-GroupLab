#include <Arduino.h>
#include <avr/io.h>

#include "pwm.h"
#include "timer.h"
#include "switch.h"
#include "relay.h"

#define LOOP_DELAY_MS 10

typedef enum stateType_enum {
  wait_press, 
  activate_servo, 
  write_open_to_screen,
  change_relay,
  turn_on_relay,
  write_on_to_screen,
  turn_off_relay,
  write_off_to_screen,
} stateType;

volatile stateType application_state = wait_press;

volatile int relay_state = 0; //0 means that the relay is off, 1 means that the relay is on


int main () {
  Serial.begin(9600);
  sei();

  initTimer0(); //milliseconds

  initPWMTimer3(); //pwm 2

  initRelaySwitch(); //servo out, digital pin 21

  initServoSwitch(); //relay button, digital pin 20

  initPortB6(); //relay out
  turnOffMotor();


  while (1)
  {
    Serial.println(application_state);

    switch (application_state)
    {
    case wait_press:
      ////////////////////////////////////////
      //write to screen the waiting state
      ////////////////////////////////////////

      break;
    
    case activate_servo:
      letBallThrough();

      application_state = write_open_to_screen;

      break;

    case write_open_to_screen:
      ////////////////////////////////////////
      //write to screen the open state
      ////////////////////////////////////////

      application_state = wait_press;
      break;

    case change_relay:
      if (relay_state == 0) {
        application_state = turn_on_relay;
      }
      else {
        application_state = turn_off_relay;
      }
      break;
    
    case turn_on_relay:
      turnOnMotor();
      relay_state = 1;

      application_state = write_on_to_screen;
      break;

    case write_on_to_screen: 
      ////////////////////////////////////////
      //write to screen the on state
      ////////////////////////////////////////

      application_state = wait_press;
      break;

    case turn_off_relay:
      turnOffMotor();
      relay_state = 0;

      application_state = write_off_to_screen;
      break;
    
    case write_off_to_screen:
      ////////////////////////////////////////
      //write to screen the off state
      ////////////////////////////////////////

      application_state = wait_press;
      break;
    
    }

    delayMs(LOOP_DELAY_MS);
  }
  
}

// ISR for PCINT0
ISR(INT1_vect){
  application_state = activate_servo;

}

ISR(INT0_vect){
  application_state = change_relay;
}