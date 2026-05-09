#ifndef _LCD_PRINCIPALS_
#define _LCD_PRINCIPALS_

#include "PRINCIPALS.h"

#define PORTi PORTB
#define DDRi  DDRB

#define E   PB0
#define RS  PB1
#define x   2   // dados em PB2–PB5

#define pulseEnable() do { \
	setBit(PORTi, E); \
	_delay_us(2); \
	clearBit(PORTi, E); \
	_delay_us(50); \
} while(0)

void initLCD(void);
void sendData(uint8_t DATA, uint8_t command);
void sendNibble(uint8_t DATA, uint8_t XSB);

void sendString(char* msg, uint8_t row, uint8_t colum);
void setLine(uint8_t row, uint8_t colum);

void clearDisplay(void);
void homePosition(void);
void entryMode(void);
void displayON_OFF(uint8_t state);

#endif