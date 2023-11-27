#include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "SPI.h"
#include "i2c.h"
#include "switch.h"
#include "timer.h"
#include "spi.h"
#include "pwm.h"

#define SLA 0x68  // MPU_6050 address with PIN AD0 grounded
#define PWR_MGMT  0x6B
#define WAKEUP 0x00
#define SL_MEMA_XAX_HIGH  0x3B
#define SL_MEMA_XAX_LOW   0x3C
#define SL_MEMA_YAX_HIGH  0x3D
#define SL_MEMA_YAX_LOW   0x3E
#define SL_MEMA_ZAX_HIGH  0x3F
#define SL_MEMA_ZAX_LOW   0x40
#define SL_TEMP_HIGH      0x41
#define SL_TEMP_LOW       0x42


typedef enum stateEnum {
  wait_press, 
  debounce_press, 
  wait_release, 
  debounce_release, 
  smile, 
  frown_w_noise, 
  wait_smile, 
  wait_frown
} stateType;

volatile stateType lightState;
volatile stateType debounceState;
bool alarm = false;

int main () {
  Serial.begin(9600); // using serial port to print values from I2C bus

  sei();

  initI2C();  // initialize I2C and set bit rate

  SPI_MASTER_Init(); // initialize SPI module and set the data rate

  _delay_ms(100);  // delay for 1 s to display "HI"
  // initialize 8 x 8 LED array (info from MAX7219 datasheet)
  write_execute(0x0A, 0x03);  // brightness control
  write_execute(0x0B, 0x07); // scanning all rows and columns
  write_execute(0x0C, 0x01); // set shutdown register to normal operation (0x01)
  write_execute(0x0F, 0x00); // display test register - set to normal operation (0x01)

  StartI2C_Trans(SLA);
  write(PWR_MGMT);// address on SLA for Power Management
  write(WAKEUP); // send data to Wake up from sleep mode
  StopI2C_Trans();

  //state machine for switch 
  switch (debounceState){
    case wait_press:
    Serial.print("wait press");
    break;

    case debounce_press:
    delayMs(1);
    Serial.print("debounce press");
    debounceState = wait_release;
    break;

    case wait_release:
    Serial.print("wait release");
    break;

    case debounce_release:
    delayMs(1);
    Serial.print("debounce release");
    alarm = false;
    debounceState = wait_press;
    break;
  }

    //Serial.print("Pitch = ");
    //Serial.println(pitch);
    //Serial.print("Yaw = ");
    //Serial.println(yaw);
    //Serial.println(" ");

  StopI2C_Trans();
/*
while (1) {
 _delay_ms(1000);
  
  
  Read_from(SLA,SL_MEMA_YAX_HIGH);
  
 // status = TWSR & 0xF8;

  T_val= Read_data(); // read upper value
 
  Read_from(SLA,SL_MEMA_YAX_LOW);
  T_val = (T_val << 8 )| Read_data(); // append lower value
  
  //Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
  
  T_y = T_val;

 _delay_ms(1000);
  
  
  Read_from(SLA,SL_MEMA_XAX_HIGH);
  
 // status = TWSR & 0xF8;

  T_val= Read_data(); // read upper value
 
  Read_from(SLA,SL_MEMA_XAX_LOW);
  T_val = (T_val << 8 )| Read_data(); // append lower value
  
  //Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
  
  T_x = T_val;
   _delay_ms(1000);
  
  
  Read_from(SLA,SL_MEMA_ZAX_HIGH);
  
 // status = TWSR & 0xF8;

  T_val= Read_data(); // read upper value
 
  Read_from(SLA,SL_MEMA_ZAX_LOW);
  T_val = (T_val << 8 )| Read_data(); // append lower value
  
  //Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
  
  T_z = T_val;
  if(T_z < 0){
    Serial.print("BUZZ");
  }
  //Serial.print("Zaxis =  ");
  //Serial.println(T_x);
  StopI2C_Trans();
}
*/
}


ISR(PCINT0_vect){
  if ((debounceState == wait_press)){
    debounceState = debounce_press;
  }
  else if (debounceState == wait_release){
    debounceState = debounce_release;
  }

}