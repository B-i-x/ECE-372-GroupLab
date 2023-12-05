#include <Arduino.h>
#include <avr/io.h>
#include <Wire.h>


#include "pwm.h"
#include "timer.h"
#include "switch.h"
#include "relay.h"
#include "spi.h"

#define LOOP_DELAY_MS 50

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

int main () {
  Serial.begin(9600);
  sei();

  initTimer0(); //milliseconds

  initPWMTimer3(); //pwm 2
  setServoAngle(0);

  initRelaySwitch(); //servo out, digital pin 21

  initServoSwitch(); //relay button, digital pin 20

  initPortB6(); //relay out digital pin 12
  turnOffMotor();

  SPI_MASTER_Init(); // initialize SPI module and set the data rate
  delayMs(100);  // delay for 1 s to display "HI"
  // initialize 8 x 8 LED array (info from MAX7219 datasheet)
  write_execute(0x0A, 0x03);  // brightness control
  write_execute(0x0B, 0x07); // scanning all rows and columns
  write_execute(0x0C, 0x01); // set shutdown register to normal operation (0x01)
  write_execute(0x0F, 0x00);

  uint64_t oImage = 0x00003C4242423C00;
  uint64_t nImage = 0x00007E20100C7E00;
  //uint64_t uImage = 0x00003E4040403E00;
  uint64_t fImage = 0x00007E4848484000;
  uint64_t idleImage = 0x40403C02023C4040;
  uint64_t pImage = 0x00007E4848300000;
  uint64_t eImage = 0x00007E4A4A4A4200;

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
      write_Image_Scroll_Up_Inf(idleImage, scroll_counter);
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
      write_Image_Scroll_Up_In(oImage, 40);
      write_Image_Scroll_Up_In(pImage, 40);
      write_Image_Scroll_Up_In(eImage, 40);
      write_Image_Scroll_Up_In(nImage, 40);

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
      //On animation
      write_Image_Scroll_Up_In(oImage, 40);
       write_Image_Scroll_Up_In(nImage, 35);    
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
      // //Off animation
      write_Image_Scroll_Up_In(oImage, 40);
       write_Image_Scroll_Up_In(fImage, 40);
      // write_Image_Scroll_Up_In(fImage, 40);
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