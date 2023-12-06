#include "pwm.h"
#include "timer.h"
#include "switch.h"
#include "relay_out.h"
#include "spi.h"
#include "i2c.h"

#include <Arduino.h>
#include <avr/io.h>
#include <Wire.h>


#define LOOP_DELAY_MS 10

#define BALL_MAX_CUTOFF_DISTANCE_VALUE 200
#define BALL_MIN_CUTOFF_DISTANCE_VALUE 11

#define SENSOR_DATA_REG 0x08

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

int scroll_counter = 0;

int sensor_distance;

int main () {
  sei();

  Serial.begin(9600);

  initTimer0(); //milliseconds

  SPI_MASTER_Init(); // initialize SPI module and set the data rate
  // initialize 8 x 8 LED array (info from MAX7219 datasheet)
  initializeScreen();
  delayMs(100);

  Wire.begin();
  initializeDistanceSensorWithWire();
  delayMs(100);

  
  initPWMTimer3(); //pwm 2
  setServoAngle(0);

  //dont know why but you have to put it here
  initRelaySwitch();
  initRelay(); //relay out, digital pin 12
  turnOffMotor();


  while (true)
  {
    // Serial.println(application_state);

    switch (application_state)
    {
    case wait_press:

      sensor_distance = readWithWire(SENSOR_DATA_REG);
      // Serial.println(sensor_distance);
      // delayMs(100);
      if (sensor_distance < BALL_MAX_CUTOFF_DISTANCE_VALUE && sensor_distance  > BALL_MIN_CUTOFF_DISTANCE_VALUE) {
        application_state = activate_servo;
      }

      write_waveform_to_screen(scroll_counter);
      scroll_counter++;

      break;
    
    case activate_servo:
      letBallThrough();

      application_state = write_open_to_screen;

      break;

    case write_open_to_screen:
      write_OPEN_screen();
      application_state = wait_press;
      break;

    case change_relay:
      // Serial.println("change relay");
      // delayMs(100);
      if (relay_state == 0) {
        application_state = turn_on_relay;
      }
      else {
        application_state = turn_off_relay;
      }

      break;
    
    case turn_on_relay:
      turnOnMotor();
      delayMs(10);

      relay_state = 1;

      application_state = write_on_to_screen;

      break;

    case write_on_to_screen: 
      write_ON_to_screen();

      application_state = wait_press;

      break;

    case turn_off_relay:
      turnOffMotor();
      
      relay_state = 0;

      application_state = write_off_to_screen;
      break;
    
    case write_off_to_screen:
      write_OFF_screen();
      delayMs(10);

      application_state = wait_press;

      break;
    }

    delayMs(LOOP_DELAY_MS);

  }
  
}

// ISR for relay
ISR(INT5_vect){
  application_state = change_relay;

}