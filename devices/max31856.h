/*
 * max31856.h
 *
 *  Created on: Dec 1, 2016
 *      Author: matt
 */

#ifndef MAX31856_H_
#define MAX31856_H_

#include <msp430.h>
#include <intrinsics.h>
#include <stdint.h>
#include <math.h>
#include "../config.h"

#define SAMPLE_TIMER_STOP	  	TASSEL__ACLK | MC__STOP | ID__8 | TACLR;
#define SAMPLE_TIMER_START		TASSEL__ACLK | MC__UP   | ID__8 | TACLR;
//**************DEFINES*********************************
#define TC0	0
#define TC1	1
#define TC2	2

#define TC0_PDIR	P8DIR
#define TC1_PDIR	P8DIR
#define TC2_PDIR	P8DIR

#define TC0_BIT	BIT5
#define TC1_BIT	BIT6
#define TC2_BIT	BIT7

#define TC0_POUT	P8OUT
#define TC1_POUT	P8OUT
#define TC2_POUT	P8OUT

#define NUM_SENSORS	3

extern uint8_t temp_reads[3];
extern uint16_t conv_temps[NUM_SENSORS];
extern float float_temps[NUM_SENSORS];
//***********function prototypes**************************
void tmp_init();
void tmp_conf();
void sample_routine();
void writeData_tmp(uint8_t tc_sel, uint8_t addr, uint8_t data);
void read_temp_regs(uint8_t tc_sel);
uint8_t read_single(uint8_t tc_sel, uint8_t addr);

uint8_t readData_single(uint8_t tc_sel, uint8_t addr);
uint8_t convert_temp_int(uint8_t b2, uint8_t b1);

float convert_temp_float(uint8_t b2, uint8_t b1, uint8_t b0);


#endif /* MAX31856_H_ */
