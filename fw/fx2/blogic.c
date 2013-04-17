/*
 * blogic.c -- Configure FX2 EEPROM. 
 * 
 * Copyright (c) 2009 by Wolfgang Wieser ] wwieser (a) gmx <*> de [ 
 * Copyright (c) 2013 by Fredrik Ahlberg <kongo@z80.se>
 * 
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 as published by the Free Software 
 * Foundation. (See COPYING.GPL for details.)
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 */

#include "fx2regs.h"
#define SYNCDELAY       _asm  nop; _endasm
        
#define I2CREAD     0x01
#define I2CWRITE    0x00
#define EEPROM_ADR  0xa0    /* 1010 A2 A1 A0 RW -> 1010 0000 */

typedef unsigned char uint8;
typedef unsigned short uint16;

#define I2CS_START   0x80
#define I2CS_STOP    0x40
#define I2CS_LASTRD  0x20
#define I2CS_ACK     0x02
#define I2CS_DONE    0x01

// Write one byte into EEPROM. 
// This takes about 3msec for the Microchip 24LC64. 
// Returns 0 on success and >0 on error. 
// Will wait for write operation to complete (ACK polling). 

static uint8 EEWrite(uint8 ee_adr, uint8 ee_data)
{
	I2CS = I2CS_START; 
	I2DAT = EEPROM_ADR | I2CWRITE;
	while (!(I2CS & I2CS_DONE)) ;
    
	// If ACK is set, the slave acknowledges (OK). 
	if (!(I2CS & I2CS_ACK)) {
        return 2;
    }
	
	// Write address 
	I2DAT = ee_adr;
	while (!(I2CS & I2CS_DONE)) ;
	
	// If ACK is set, the slave acknowledges (OK). 
	if (!(I2CS & I2CS_ACK)) {
        return 3;
    }
	
	// Write data. 
	I2DAT = ee_data;
	while (!(I2CS & I2CS_DONE)) ;
	
	I2CS = I2CS_STOP;
	// If ACK is set, the slave acknowledges (OK). 
	if (!(I2CS & I2CS_ACK)) {
        return 4;
    }
	
	// Wait for WRITE to complete (ACK polling). 
	for (;;) {
		I2CS = I2CS_START; 
		I2DAT = EEPROM_ADR | I2CWRITE;
		while (!(I2CS & I2CS_DONE)) ;
		
		if ((I2CS & I2CS_ACK)) {
            break;
        }
	}
	
	return 0;
}

void main(void)
{
	// Set fclk = 400kHz: 
	I2CTL = 0x01;
	
    EEWrite(0, 0xC0); // mode = valid vid/pid but don't boot cpu
    EEWrite(1, 0x25); // vid = 0x0925
    EEWrite(2, 0x09);
    EEWrite(3, 0x81); // pid = 0x3881
    EEWrite(4, 0x38);
    EEWrite(5, 0x00);
    EEWrite(6, 0x00);
    EEWrite(7, 0x00);
	
	for (;;) ;
}
