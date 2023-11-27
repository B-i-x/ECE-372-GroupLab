#ifndef I2C_H
#define I2C_H

void initI2C();
void sendDataI2C(unsigned char i2c_address, unsigned char data_to_send);

void StartI2C_Trans(unsigned char SLA);
void StopI2C_Trans();
void Read_from(unsigned char SLA, unsigned char MEMADDRESS);
unsigned char Read_data();
#endif
