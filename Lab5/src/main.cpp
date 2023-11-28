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
typedef enum {wait_press, debounce_p, wait_release, debounce_r} button_state;
//declare the enum state variable
volatile button_state my_button_state = wait_press;
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


//status = TWSR & 0xF8;


StopI2C_Trans();


initPWMTimer3();
while (1) {

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

  write_happy_face();
  if((T_z < 12500) || ((T_y < 0) || (T_y > 7000))){
    int  i = 0;
    write_sad_face();
    godothing = 1;
    while(godothing == 1){
      switch (my_button_state)
      {
      case  wait_press:
        break;

      case  debounce_p:
        //eventually go to wait_release
        //need to delay by ~1ms
        //Serial.print("Pressed");
        delayMs(1);
        my_button_state = wait_release;
        break;

      case  wait_release:
       //Serial.print("release");
        break;

      case  debounce_r:
        //eventually go to wait_release
        //need to delay by ~1ms
        //Serial.print("debounce");
        delayMs(1);
        my_button_state = wait_press;
        break;
      default:
        break;
      }
      OCR3A = i;
      delayMs(1);
      i = i+8;
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

//Interrupt state machine
ISR(PCINT0_vect){
  //if pressed go to debounce state
  if(my_button_state == wait_press){
      my_button_state = debounce_p;
  }
  //if its pressed and released change the flip speed
  else if(my_button_state == wait_release){
    if (godothing == 1){
      OCR3A = 0;
      godothing = 0;
      write_happy_face();
    }
    else{
      godothing = 0;
      //OCR3A = 0;
    }
    my_button_state = debounce_r;
  }
}