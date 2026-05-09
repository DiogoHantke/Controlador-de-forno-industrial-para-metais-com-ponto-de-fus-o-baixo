#include "LCDPRINCIPALS.h"
#include <avr/pgmspace.h>

#define PORTi PORTB
#define DDRi  DDRB
#define E     0
#define RS    1

static void pulseEnable(void)
{
	setBit(PORTi, E);
	_delay_us(1);

	clearBit(PORTi, E);
	_delay_us(1);
}

void initLCD(void)
{
	DDRi |= 0x3F;

	clearBit(PORTi, E);
	clearBit(PORTi, RS);

	_delay_ms(15);

	sendData(0x03, 0);
	_delay_ms(5);

	sendData(0x03, 0);
	_delay_us(150);

	sendData(0x03, 0);
	_delay_us(150);

	sendData(0x02, 0);
	_delay_us(150);

	sendData(0x28, 0);
	displayON_OFF(1);
	entryMode();
	clearDisplay();
	homePosition();
}

void sendData(unsigned char DATA, uint8_t command)
{
	if (command) {
		setBit(PORTi, RS);
		} else {
		clearBit(PORTi, RS);
	}

	PORTi = (PORTi & 0x03) | ((DATA & 0xF0) >> 2);
	pulseEnable();

	PORTi = (PORTi & 0x03) | ((DATA & 0x0F) << 2);
	pulseEnable();

	_delay_us(50);
}

void displayON_OFF(uint8_t state)
{
	if (state) {
		sendData(0x0C, 0);
		} else {
		sendData(0x08, 0);
	}
}

void sendString(const char *string, uint8_t row)
{
	setLine(row);

	while (*string != '\0') {
		sendData(*string, 1);
		string++;
	}
}

void sendStringFlash(const char *stringFlash, uint8_t row)
{
	char c;

	setLine(row);

	while ((c = pgm_read_byte(stringFlash)) != '\0') {
		sendData(c, 1);
		stringFlash++;
	}
}

void lcdPrintFlash(uint8_t row, const char *stringFlash)
{
	uint8_t i;
	uint8_t endText = 0;

	setLine(row);

	for (i = 0; i < LCD_COLUMNS; i++) {
		char c = ' ';

		if (!endText) {
			c = pgm_read_byte(&stringFlash[i]);

			if (c == '\0') {
				c = ' ';
				endText = 1;
			}
		}

		sendData(c, 1);
	}
}

void setLine(uint8_t row)
{
	uint8_t address;

	if (row == 0) {
		address = 0x00;
		} else {
		address = 0x40;
	}

	sendData(0x80 | address, 0);
	_delay_us(50);
}

void clearDisplay(void)
{
	sendData(0x01, 0);
	_delay_ms(2);
}

void homePosition(void)
{
	sendData(0x02, 0);
	_delay_ms(2);
}

void entryMode(void)
{
	sendData(0x06, 0);
}