#include "i2c.h"
#include "pwm.h"
#include "timer.h"
#include "switch.h"
#include "spi.h"
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


//create states
typedef enum {read_data, buzz, turn_off_buzzer} state;
//declare the enum state variable
volatile state place_state = read_data;
int godothing = 0;

int main() {

Serial.begin(9600); // using serial port to print values from I2C bus
sei(); 
initI2C(); 
initSwitchPB3(); // initialize I2C and set bit rate
SPI_MASTER_Init(); // initialize SPI module and set the data rate
_delay_ms(100);  // delay for 1 s to display "HI"
// initialize 8 x 8 LED array (info from MAX7219 datasheet)
write_execute(0x0A, 0x03);  // brightness control
write_execute(0x0B, 0x07); // scanning all rows and columns
write_execute(0x0C, 0x01); // set shutdown register to normal operation (0x01)
write_execute(0x0F, 0x00); // display test register - set to normal operation (0x01)

signed int T_val = 0;
float T_y = 0;	
float T_z = 0;	
float T_x = 0;	
//char status;

  

StartI2C_Trans(SLA);
//status = TWSR & 0xF8;
write(PWR_MGMT);// address on SLA for Power Management
write(WAKEUP); // send data to Wake up from sleep mode

StopI2C_Trans();

int i = 1000;

int imu_at_zero = 1;

initPWMTimer3();
while (1) {

  delayMs(50);
  Serial.println(place_state);

  switch (place_state) {
    case read_data:
      _delay_ms(100);
      Read_from(SLA,SL_MEMA_YAX_HIGH);
      T_val= Read_data(); // read upper value
      Read_from(SLA,SL_MEMA_YAX_LOW);
      T_val = (T_val << 8 )| Read_data(); // append lower value
      T_y = T_val;
      Read_from(SLA,SL_MEMA_ZAX_HIGH); 
      // status = TWSR & 0xF8;
      T_val= Read_data(); // read upper value
      Read_from(SLA,SL_MEMA_ZAX_LOW);
      T_val = (T_val << 8)| Read_data(); // append lower value
      T_z = T_val;
      StopI2C_Trans();
      Serial.print(T_z);
      Serial.print(",");
      Serial.print(T_y);
      Serial.println();

      

      if((T_z < 12500) || ((T_y < 0) || (T_y > 7000))){
        write_sad_face();

        if (imu_at_zero) { 
          place_state = buzz; 
          imu_at_zero = 0;
        }
        else {
          place_state = read_data;
        }

      }
      else{
        write_happy_face();
      }

      break;

    case buzz:
      changer(i);
      i = i+1000;
      if(i > 4000){
        i = 1000;
      }

      _delay_ms(100);
      Read_from(SLA,SL_MEMA_YAX_HIGH);
      T_val= Read_data(); // read upper value
      Read_from(SLA,SL_MEMA_YAX_LOW);
      T_val = (T_val << 8 )| Read_data(); // append lower value
      T_y = T_val;
      Read_from(SLA,SL_MEMA_ZAX_HIGH); 
      // status = TWSR & 0xF8;
      T_val= Read_data(); // read upper value
      Read_from(SLA,SL_MEMA_ZAX_LOW);
      T_val = (T_val << 8)| Read_data(); // append lower value
      T_z = T_val;
      godothing = 0;
      StopI2C_Trans();

      if((T_z < 12500) || ((T_y < 0) || (T_y > 7000))){
        write_sad_face();    
      }
      else{
        write_happy_face();
      }



      break;

    case (turn_off_buzzer):
      turnOff();
      place_state = read_data;
      break;

    }
  
  }
return 0;
}

//Interrupt state machine
ISR(PCINT0_vect){
  //if pressed go to debounce state
  if(place_state == buzz){
      place_state = turn_off_buzzer;
  }
  
}