#include "i2c.h"
#include "pwm.h"
#include "timer.h"
#include "switch.h"
#include <avr/io.h>
#include "Arduino.h"

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


typedef enum stateType_enum {
  wait_press, debounce_press, wait_release, debounce_release
} stateType;

// set pushbutton state to initial position of wait_press
// make state variable volatile type since it can be changed in the ISR
// 

volatile stateType pbstate = wait_press;
int godothing = 0;

int main() {

Serial.begin(9600); // using serial port to print values from I2C bus
sei(); 
initI2C(); 
initSwitchPD2(); // initialize I2C and set bit rate

signed int T_val = 0;
float T_y = 0;	
float T_z = 0;	
float T_x = 0;	
//char status;

  

StartI2C_Trans(SLA);

//status = TWSR & 0xF8;

write(PWR_MGMT);// address on SLA for Power Management
write(WAKEUP); // send data to Wake up from sleep mode


//status = TWSR & 0xF8;


StopI2C_Trans();


initPWMTimer3();
while (1) {
   switch(pbstate) {
   case wait_press:
        break;
   case debounce_press:
        delayMs(1);
        pbstate = wait_release;
        break;
    case wait_release:
        break;
    case debounce_release:
        delayMs(1);
        pbstate = wait_press;
        break;          
    }

  _delay_ms(1000);
  Read_from(SLA,SL_MEMA_YAX_HIGH);
  
 // status = TWSR & 0xF8;

  T_val= Read_data(); // read upper value
 
  Read_from(SLA,SL_MEMA_YAX_LOW);
  T_val = (T_val << 8 )| Read_data(); // append lower value
  
  //Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
  
  T_y = T_val;

  
  
  Read_from(SLA,SL_MEMA_XAX_HIGH);
  
 // status = TWSR & 0xF8;

  T_val= Read_data(); // read upper value
 
  Read_from(SLA,SL_MEMA_XAX_LOW);
  T_val = (T_val << 8 )| Read_data(); // append lower value
  
  //Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
  
  T_x = T_val;
  
  
  Read_from(SLA,SL_MEMA_ZAX_HIGH);
  
 // status = TWSR & 0xF8;

  T_val= Read_data(); // read upper value
 
  Read_from(SLA,SL_MEMA_ZAX_LOW);
  T_val = (T_val << 8)| Read_data(); // append lower value
  
  //Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
  godothing = 0; 
  T_z = T_val;
  if((T_z < 12500) || ((T_y < 0) || (T_y > 7000))){
    int  i = 0;
    while(godothing == 0){
      OCR3A = i;
      delayMs(1);
      i = i+4;
      if(i == 1024){
        
        i = 0;
      }
      Serial.print(godothing);
    }
  }
  Serial.print("Zaxis =  ");
  Serial.println(T_y);
  StopI2C_Trans();
}
  return 0;

}

ISR(INT2_vect){
  // if the interrupt was triggered when state was waiting for press then we are going
  // debounce the press action  so set pbstate to debounce press
  
  if( pbstate == wait_press) {
    pbstate = debounce_press;
  }
  else if (pbstate == wait_release) {
 // else if interrupt was triggered when we were in wait_release, then we are going to 
 // debounce the release action so set the pbstate to debounce release but first
 // check the ledspeed and change it
 if (godothing == 0) {
    // if the led_speed is fast then change it to slow
   godothing = 1;
 }
 else {
   godothing = 0; // else led_speed was slow so change it to fast
 }
 pbstate = debounce_release;
 }  
}