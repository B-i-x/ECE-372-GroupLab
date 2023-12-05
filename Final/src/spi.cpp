#include <Arduino.h>
#include "SPI.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <timer.h>

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


void write_Image(uint64_t hexValue){
    for (int i = 0; i < 8; i++) {
        // Extract two hex characters
        unsigned char hexPair = (hexValue >> (i * 8)) & 0xFF;
        write_execute(0x00 + i + 1, hexPair); // row 8 LEDS       
    }
}
void write_Image_Scroll(uint64_t hexValue) {
    // Number of shifts to create the scrolling effect
    const int shifts = 9;

    // Outer loop for presenting the image
    for (int shiftCount = 0; shiftCount < shifts; shiftCount++) {
        // Inner loop to shift bits of each row
        for (int i = 0; i < 8; i++) {
            // Calculate the shifted hexPair for each row
            unsigned char hexPair = ((hexValue >> (i * 8 + shiftCount)) | (hexValue << (8 - shiftCount))) & 0xFF;
            write_execute(0x00 + i + 1, hexPair); // row 8 LEDS
        }

        // Delay to create a scrolling effect
        delayMs(250);
    }
}


void write_Image_Scroll_Up(uint64_t hexValue) {
    for (int scrollStep = 0; scrollStep < 8; scrollStep++) {
        // Shift hexValue up by 8 bits for each scroll step
        uint64_t scrolledValue = hexValue >> (scrollStep * 8);

        // Write the scrolled image
        for (int i = 0; i < 8; i++) {
            // Extract two hex characters
            unsigned char hexPair = (scrolledValue >> (i * 8)) & 0xFF;
            write_execute(0x00 + i + 1, hexPair); // row 8 LEDS       
        }

        // Add a delay or other synchronization method to control scrolling speed
        // You may need to adjust the delay based on your specific requirements
        _delay_ms(100); // Adjust the delay time as needed
    }
}

void write_Image_Scroll_Up_In(uint64_t hexValue, int delay) {
    const int shifts = 17;

    for (int shiftCount = 0; shiftCount < shifts; shiftCount++) {
        for (int row = 7; row >= 0; row--) {  // Initialize rows in reverse order
            // Calculate the shifted hexPair for each row with circular wrapping
            unsigned char hexPair = ((hexValue >> (row * 8 + (8 - shiftCount) * 8)) | (hexValue << ((shiftCount) * 8))) & 0xFF;
            
            // Reverse the bits within each byte
            hexPair = ((hexPair * 0x0802LU & 0x22110LU) | (hexPair * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;

            write_execute(0x00 + (7 - row) + 1, hexPair); // row 8 LEDS
        }

        delayMs(delay);
    }

}

void write_Image_Scroll_Down(uint64_t hexValue) {
    int scrollCount = 8; // Number of times to scroll (adjust as needed)

    for (int scroll = 0; scroll < scrollCount; scroll++) {
        for (int i = 0; i < 8; i++) {
            // Extract one hex character for each row
            unsigned char hexPair = (hexValue >> (i * 8 - scroll * 8)) & 0xFF;
            write_execute(0x00 + i + 1, hexPair); // row 8 LEDS
        }

        delayMs(100); // Delay for 250 milliseconds before the next scroll
    }
}

void write_Image_Scroll_Down_Inf(uint64_t hexValue) {
    for (int scrollCount = 0; scrollCount < 8; scrollCount++) {
        // Shift the hexValue to the right to create a circular scroll effect
        hexValue = (hexValue << 8) | (hexValue >> 56);

        // Write and display the scrolled image
        write_Image(hexValue);

        // Delay for 250ms
        delayMs(100);
    }
}

void write_Image_Scroll_Up_Inf(uint64_t hexValue, int scroll) {

    for (int i = 0; i < 8; i++) {
        // Extract one hex character for each row
        unsigned char hexPair = (hexValue >> ((i + scroll) % 8 * 8)) & 0xFF;
        write_execute(0x00 + i + 1, hexPair); // row 8 LEDS
    }

    scroll++; // Increment the scroll count

    // Introduce a small delay if needed to control the speed
    delayMs(50); // Adjust the delay time as needed
    
}

void write_Image_Scroll_In(uint64_t hexValue) {
    const int shifts = 9;

    for (int shiftCount = 0; shiftCount < shifts; shiftCount++) {
        for (int row = 0; row < 8; row++) {
            // Calculate the shifted hexPair for each row with circular wrapping
            unsigned char hexPair = ((hexValue >> (row * 8 + (8 - shiftCount) * 8)) | (hexValue << ((shiftCount) * 8))) & 0xFF;
            write_execute(0x00 + row + 1, hexPair); // row 8 LEDS
        }

        delayMs(100);
    }
}


