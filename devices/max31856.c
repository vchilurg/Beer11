/*
 * max31856.c
 *
 *  Created on: Dec 1, 2016
 *      Author: matt
 */
#include "max31856.h"

uint8_t temp_reads[3];
uint16_t conv_temps[NUM_SENSORS];
float float_temps[NUM_SENSORS];

void tmp_init(){

	//2.2 - UCACLK, 2.0 - MOSI, 2.1 - MISO
	P2SEL0 |= BIT0 | BIT1 | BIT2;
	P2SEL1 &= ~(BIT0 | BIT1 | BIT2);

	TC0_PDIR |= TC0_BIT;					//CS port/pin directions output
	TC1_PDIR |= TC1_BIT;
	TC2_PDIR |= TC2_BIT;
	TC0_POUT |= TC0_BIT;					//CS idle HIGH to disable
	TC1_POUT |= TC1_BIT;
	TC2_POUT |= TC2_BIT;

	//200ms sampling clock for temperature senors
	TA3CTL = SAMPLE_TIMER_STOP;
	TA3CCR0 = 817;
	TA3CCTL0 = CCIE;

	//SPI Config
	UCA0CTLW0 |= UCSWRST;
	UCA0CTLW0 |= UCMSB | UCMST | UCSYNC | UCSSEL__SMCLK; // UCCKPL //UCMST| UCMODE_0 | UCMSB | UCSYNC;				// 3-pin, 8-bit SPI mstr, MSb 1st,  Synchronous mode	//UCCKPH (set for WRITE, 0 for READ)?
	UCA0CTLW0 &= ~UCSWRST;

}
void tmp_conf()
{
	//Config registers on chip for k-type thermocouple, 8x average sampling
	int a;
	for(a = 0; a < NUM_SENSORS; a++)
	{
		writeData_tmp((uint8_t)a, 0x81, 0x03);		//configure for 8x averaging and k-type
		writeData_tmp((uint8_t)a, 0x80, 0x80);		//configure for contiuous conversion
	}
}

//********************internal functions***********************************

//returns float of thermocouple temperature, needs byte2, byte1, and byte0
float convert_temp_float(uint8_t b2, uint8_t b1, uint8_t b0){
	uint16_t temp = (b2 <<4) + ((b1 & 0xF0) >>4);
	temp = ( ((b2 & 0x80) !=0)? ((~temp)+1)*(-1) : temp);		//checks for 2's complement
	float ret = temp * 1.0;
	int i=0;

	//b1
	for(i=1;i<5;i++){
		if ((b1 & (0x10 >>i)) != 0){
			ret += powf(2, -i);
		}
	}

	//b0
	for(i=5;i<8;i++){
		if ((b0 & (0x80 >>(i-5))) != 0){
			ret += powf(2, -i);
		}
	}

	return (ret * 1.8) + 32;
}

//returns truncated int of thermocouple temperature, needs byte2 and byte1
uint8_t convert_temp_int(uint8_t b2, uint8_t b1){
	uint16_t ret =  (b2 <<4) + ((b1 & 0xF0) >>4);

	if(b2 & 0x80 != 0)				//checks for 2's complement
		ret = (~(ret) + 1) * -1;
	ret = (ret * 2) + ((-1)*(ret) + 160) / 5;
	return ret;
}




//************SPI Comm. functions for MAX31856*********************

void writeData_tmp(uint8_t tc_sel, uint8_t addr, uint8_t data) {

	//~CS0
	switch(tc_sel){
		case TC0:
			TC0_POUT &= ~TC0_BIT; break;
		case TC1:
			TC1_POUT &= ~TC1_BIT; break;
		default:
			TC2_POUT &= ~TC2_BIT; break;
	}
	__delay_cycles(10);
	UCA0TXBUF = addr;
	while(UCA0STATW & UCBUSY);			//busy wait

	UCA0TXBUF = data;
	while(UCA0STATW & UCBUSY);			//busy wait


	//CS
	switch(tc_sel){
		case TC0:
			TC0_POUT |= TC0_BIT; break;
		case TC1:
			TC1_POUT |= TC1_BIT; break;
		default:
			TC2_POUT |= TC2_BIT; break;
	}

}

uint8_t readData_single(uint8_t tc_sel, uint8_t addr){

	uint8_t recv_buf = 0;

	//~CS0
	switch(tc_sel){
		case TC0:
			TC0_POUT &= ~TC0_BIT; break;
		case TC1:
			TC1_POUT &= ~TC1_BIT; break;
		default:
			TC2_POUT &= ~TC2_BIT; break;
	}

	UCA0TXBUF = addr;
	while(UCA0STATW & UCBUSY);			// busy wait

	UCA0TXBUF = addr;					// write addr. again for response
	while(UCA0STATW & UCBUSY);			// busy wait
	recv_buf = UCA0RXBUF;

	//CS
	switch(tc_sel){
		case TC0:
			TC0_POUT |= TC0_BIT; break;
		case TC1:
			TC1_POUT |= TC1_BIT; break;
		default:
			TC2_POUT |= TC2_BIT; break;
	}

	return recv_buf;

}

void read_temp_regs(uint8_t tc_sel){

	int a;
	uint8_t addr = 0x0C;

	//~CS0
	switch(tc_sel){
		case TC0:
			TC0_POUT &= ~TC0_BIT; break;
		case TC1:
			TC1_POUT &= ~TC1_BIT; break;
		default:
			TC2_POUT &= ~TC2_BIT; break;
	}

	for(a=3; a>=0; a--)
	{
		UCA0TXBUF = addr;
	//	while(UCA0STATW & UCBUSY);			// busy wait
	//	UCA0TXBUF = 0xFF;
		while(UCA0STATW & UCBUSY);			// busy wait
		if(a!=3) temp_reads[a] = UCA0RXBUF;
	}
	//CS
	switch(tc_sel){
		case TC0:
			TC0_POUT |= TC0_BIT; break;
		case TC1:
			TC1_POUT |= TC1_BIT; break;
		default:
			TC2_POUT |= TC2_BIT; break;
	}
}
void sample_routine()
{

	int a;
	uint8_t start_sensor = TC0;

	TA3CTL = SAMPLE_TIMER_STOP;
	TA3CCTL0 = 0;

	for(a = 0; a<NUM_SENSORS; a++)
	{
		read_temp_regs(start_sensor++);
//		conv_temps[a] = convert_temp_int(temp_reads[2], temp_reads[1]);
		float_temps[a] = convert_temp_float(temp_reads[2], temp_reads[1], temp_reads[0]);
		BIN2BCD = (uint16_t) lround(float_temps[a]);
		conv_temps[a] = BIN2BCD; // convert to BCD for display and comparison
//		float test = convert_temp_float(temp_reads[2], temp_reads[1], temp_reads[0]);

	}
	TA3CTL = SAMPLE_TIMER_START;
	TA3CCTL0 = CCIE;
}
__isr(TIMER3_A0_VECTOR, sample_temp_isr)
{
	__low_power_mode_off_on_exit();
}
