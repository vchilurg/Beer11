/*
 * msp.h
 *
 * USCI and other port definitions.
 */
#ifndef MSP_H_
#define MSP_H_

#include <stdint.h>

// Initialize ports and the SPI module
void initMSP430();

// SPI data/command transfer via USCI
void writeData(uint8_t data);
void writeCommand(uint8_t command);

// Notes: The LCD reset needs to be grounded once before use, but then needs to be held high.

/**** DEFINE YOUR PORTS BELOW ****/

// clock
#define LCD_SCLK_PORT P1OUT
#define LCD_SCLK_PIN BIT4

// data out
#define LCD_MOSI_PORT P1OUT
#define LCD_MOSI_PIN BIT6

// data in
#define LCD_MISO_PORT P1OUT
#define LCD_MISO_PIN BIT7

// chip select
#define LCD_CS_PORT P2OUT
#define LCD_CS_PIN BIT3

// data/command
#define LCD_DC_PORT P2OUT
#define LCD_DC_PIN BIT4

#endif /* MSP_H_ */

