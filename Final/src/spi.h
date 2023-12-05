#ifndef SPI_H
#define SPI_H

void SPI_MASTER_Init();
void write_execute(unsigned char CMD, unsigned char data);
void write_Image(uint64_t hexValue);
void write_Image_Scroll(uint64_t hexValue);
void write_Image_Scroll_Up(uint64_t hexValue);
void write_Image_Scroll_Down(uint64_t hexValue);
void write_Image_Scroll_Up_Inf(uint64_t hexValue, int scroll);
void write_Image_Scroll_In(uint64_t hexValue);
void write_Image_Scroll_Left(uint64_t hexValue);
void write_Image_Scroll_Up_In(uint64_t hexValue, int delay);
#endif