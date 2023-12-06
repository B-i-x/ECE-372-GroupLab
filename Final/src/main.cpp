#include <Arduino.h>
#include <avr/io.h>
#include <Wire.h>


#include "pwm.h"
#include "timer.h"
#include "switch.h"
#include "relay.h"
#include "spi.h"
#include "i2c.h"

#define LOOP_DELAY_MS 10

#define BALL_MAX_CUTOFF_DISTANCE_VALUE 200
#define BALL_MIN_CUTOFF_DISTANCE_VALUE 11

#define SLA 0x60
#define SENSOR_DATA_REG 0x08
#define SENSOR_CONFIGURATION 0x03

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
  Serial.begin(9600);
  sei();

  initTimer0(); //milliseconds

  initPWMTimer3(); //pwm 2
  setServoAngle(0);

  initRelaySwitch(); //servo out, digital pin 21

  initPortB6(); //relay out digital pin 12
  turnOffMotor();


  SPI_MASTER_Init(); // initialize SPI module and set the data rate
  // initialize 8 x 8 LED array (info from MAX7219 datasheet)
  write_execute(0x0A, 0x03);  // brightness control
  write_execute(0x0B, 0x07); // scanning all rows and columns
  write_execute(0x0C, 0x01); // set shutdown register to normal operation (0x01)
  write_execute(0x0F, 0x00);


  Wire.begin();
  //turns on proximity sensor
  Wire.beginTransmission(SLA);
  Wire.write(SENSOR_CONFIGURATION);
  Wire.write(0x00);
  Wire.write(0x08);
  Wire.endTransmission();
  delayMs(100);


  while (1)
  {
    Serial.println(application_state);
    

    switch (application_state)
    {
    case wait_press:
      ////////////////////////////////////////
      //write to screen the waiting state
      ////////////////////////////////////////
      //Idle animation

      sensor_distance = readWithWire(SLA, SENSOR_DATA_REG);
      
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
      ////////////////////////////////////////
      //write to screen the open state
      ////////////////////////////////////////
      //Open animation
      write_OPEN_screen();
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
      delayMs(10);
      relay_state = 1;

      application_state = write_on_to_screen;
      break;

    case write_on_to_screen: 
      ////////////////////////////////////////
      //write to screen the on state
      //On animation
      ////////////////////////////////////////
      write_ON_to_screen();
      application_state = wait_press;
      break;

    case turn_off_relay:
      turnOffMotor();
      delayMs(10);

      relay_state = 0;

      application_state = write_off_to_screen;
      break;
    
    case write_off_to_screen:
      ////////////////////////////////////////
      //write to screen the off state
      ////////////////////////////////////////
      // //Off animation
      write_OFF_screen();
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