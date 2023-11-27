#include "i2c.h"
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





int main() {

Serial.begin(9600); // using serial port to print values from I2C bus
sei(); 
initI2C();  // initialize I2C and set bit rate

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

return 0;

}