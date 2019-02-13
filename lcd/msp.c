/*
 * msp.h
 *
 * USCI and other port definitions.
 */
#include "msp.h"
#include <msp430.h>

void initMSP430() {
	P2OUT |= LCD_CS_PIN; // Make sure CS pin starts high
	P2DIR |= LCD_CS_PIN | LCD_DC_PIN;
	P1SEL0 |=  (LCD_MOSI_PIN | LCD_MISO_PIN | LCD_SCLK_PIN);
	P1SEL1 &= ~(LCD_MOSI_PIN | LCD_MISO_PIN | LCD_SCLK_PIN);

	UCB0CTLW0 |= UCSWRST;
	UCB0CTLW0 |= UCCKPL | UCMSB | UCMST | UCMODE_0 | UCSYNC | UCSSEL__SMCLK;
	UCB0BRW = 0;
	UCB0CTLW0 &= ~UCSWRST;
}

void writeData(uint8_t data) {
    /**** Code to write a data byte to the LCD over SPI ****/
	LCD_DC_PORT |=  LCD_DC_PIN;
	LCD_CS_PORT &= ~LCD_CS_PIN;
	UCB0TXBUF = data;
	while (UCB0STAT & UCBUSY); // finish transmitting
	LCD_CS_PORT |= LCD_CS_PIN;
}

void writeCommand(uint8_t command) {
    /**** Code to write a command byte to the LCD over SPI ****/
	LCD_DC_PORT &= ~LCD_DC_PIN;
	LCD_CS_PORT &= ~LCD_CS_PIN;
	UCB0TXBUF = command;
	while (UCB0STAT & UCBUSY); // finish transmitting
	LCD_CS_PORT |= LCD_CS_PIN;
}
