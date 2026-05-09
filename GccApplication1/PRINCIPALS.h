	#ifndef _DEF_PRINCIPALS
#define _DEF_PRINCIPALS

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define setBit(PORTx, PINx) ((PORTx) |= (1<<PINx))
#define clearBit(PORTx, PINx) ((PORTx) &= ~(1<<(PINx)))
#define toggleBit(PORTx, PINx) ((PORTx) ^= (1<<(PINx)))
#define readBit(PORTx, PINx) ((PORTx & (1<<PINx)) >> PINx)

#endif