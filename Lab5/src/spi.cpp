#include <Arduino.h>
#include "spi.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#define DDR_SPI DDRB // Data Direction Register on ATMEGA2560 for SPI is DDRB
#define DD_SS DDB0  // SS Chip Select data direction bit B0 of ATMEGA2560 is DDB0
#define DD_SCK DDB1  // Clock pin connection data direction bit B1 on ATMEGA2560 is DDB1
#define DD_MOSI DDB2 // MOSI pin datadirection on ATMEGA2560 is DDB2
#define SPI_PORT PORTB // PortB for SPI on ATMEGA2560 is PORTB
#define SPI_SS_BIT PORTB0 // Port B register Bit B0 of Chip Select on ATMEGA2560 is PORTB0

#define wait_for_complete while(!(SPSR & (1 << SPIF)));


void SPI_MASTER_Init() {
    // set MOSI,SCK,and SS direction to outputs
    DDR_SPI = (1 << DD_MOSI)| (1 << DD_SCK) | (1 << DD_SS);

    // note this program does not use MISO line. IT only writes to device

    // set SS high initially (chip select off)
    SPI_PORT  |= (1 << SPI_SS_BIT);

    // enable SPI, master mode, CPOL, CPHA, default clock and fosc/128
    // datasheet says sample on rising edge CPOL = 1 CPHA =1
    SPCR  |= (1 <<SPE)| (1 << MSTR) | (1 << CPOL) | (1 << CPHA ) | (1 << SPR1) | (1 << SPR0);

    }

void write_execute(unsigned char CMD, unsigned char data) {
    SPI_PORT &= ~(1 << SPI_SS_BIT);  // enable chip select bit to begin SPI frame
    SPDR = CMD; // load the CMD address into register
    wait_for_complete; // wait for flag to raise
    SPDR = data; // load the data into register
    wait_for_complete; // wait for flag to raise
    SPI_PORT |= (1 << SPI_SS_BIT); // disable chip select to end SPI frame
}

void write_happy_face() {

    write_execute(0x01, 0b00000000); // all LEDS in Row 1 are off
    write_execute(0x02, 0b00100100); // row 2 LEDS 
    write_execute(0x03, 0b00100100); // row 3 LEDS
    write_execute(0x04, 0b00000000); // row 4 LEDS
    write_execute(0x05, 0b00000000); // row 5 LEDS
    write_execute(0x06, 0b01000010); // row 6 LEDS
    write_execute(0x07, 0b00111100); // row 7 LEDS
    write_execute(0x08, 0b00000000); // row 8 LEDS
}

void write_sad_face() {
    write_execute(0x01, 0b00000000); // all LEDS in Row 1 are off
    write_execute(0x02, 0b00100100); // row 2 LEDS 
    write_execute(0x03, 0b00100100); // row 3 LEDS
    write_execute(0x04, 0b00000010); // row 4 LEDS
    write_execute(0x05, 0b00000000); // row 5 LEDS
    write_execute(0x06, 0b00111100);
    write_execute(0x07, 0b01000010);
    write_execute(0x08, 0b00000000);
}