#ifndef _LCD_PRINCIPALS_
#define _LCD_PRINCIPALS_

#include "PRINCIPALS.h"
#include <avr/pgmspace.h>
#include <stdint.h>

#define LCD_COLUMNS 16

void initLCD(void);
void sendData(unsigned char DATA, uint8_t command);

void sendString(const char *string, uint8_t row);
void sendStringFlash(const char *stringFlash, uint8_t row);
void lcdPrintFlash(uint8_t row, const char *stringFlash);

void clearDisplay(void);
void homePosition(void);
void displayON_OFF(uint8_t state);
void entryMode(void);
void setLine(uint8_t row);

#endif