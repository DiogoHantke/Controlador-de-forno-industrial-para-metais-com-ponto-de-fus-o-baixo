#include "PRINCIPALS.h"
#include "LCDPRINCIPALS.h"

void initLCD(){
	DDRi |= (0x0F << x);          // dados (PB2~PB5)
	DDRi |= (1 << E) | (1 << RS); // controle (PB0, PB1)
	
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
	
	displayON_OFF(1);//ligar
	entryMode();
	clearDisplay();
	homePosition();
}

void sendNibble(uint8_t DATA, uint8_t XSB) {

	if (XSB) PORTi = (PORTi & ~(0xF0 >> x)) | ((DATA & 0xF0) >> x); else PORTi = (PORTi & ~(0xF0 >> x)) | ((DATA & 0x0F) << (4 - x));
	pulseEnable();
}

void sendData(unsigned char DATA, uint8_t command){
	
	(command) ? setBit(PORTi, RS) : clearBit(PORTi, RS);
	
	sendNibble(DATA, 1); // MSB
	sendNibble(DATA, 0); // LSB
}

void displayON_OFF(uint8_t state) {
	(state) ? sendData(0x0C, 0) : sendData(0x08, 0);
}

void sendString(char* msg, uint8_t row, uint8_t colum) {
	int16_t spaceIndex = -1;
	uint8_t i, state = 0;

	for(i = 0; msg[i] != '\0'; i++) {
		if(msg[i] == ' ')
		spaceIndex = i;
		if(i >= (16-colum))break;
	}
	
	if(i<(16-colum))spaceIndex = -1;
	
	setLine(row, colum);
	
	for(i = 0; msg[i] != '\0'; i++) {
		if((i == spaceIndex+1) || (i >= (16-colum)) && !state) {
			setLine(1, 0);
			state = 1;
		}
		sendData(msg[i], 1);
	}
	_delay_us(50);
}

void setLine(uint8_t row, uint8_t colum) {
	uint8_t address = (row == 0) ? 0x00 : 0x40;
	address+=colum;
	sendData(0x80 | address, 0);
	_delay_us(50);
}

void clearDisplay() {sendData(0x01, 0); _delay_ms(2);}
void homePosition() {sendData(0x02, 0); _delay_ms(2);}
void entryMode() {sendData(0x06, 0);}