/*
 * outputs.h
 *
 *  Created on: Nov 29, 2016
 *      Author: matt
 */

#ifndef DEVICES_OUTPUTS_H_
#define DEVICES_OUTPUTS_H_

#include "../config.h"
#include "../ui/ui_internal.h"

//pump and element
#define P_E_CLOCK_STOP		TASSEL__ACLK | MC__STOP | ID__8 | TACLR;
#define P_E_CLOCK_START		TASSEL__ACLK | MC__UP   | ID__8 | TACLR;

//servos
#define SERVO_CLOCK_STOP	TASSEL__SMCLK | MC__STOP | TACLR;
#define SERVO_CLOCK_START	TASSEL__SMCLK | MC__UP   | TACLR;

#define PUMP_1_DUTY     TB0CCR1
#define PUMP_2_DUTY     TB0CCR2
#define ELEM_1_DUTY 	TB0CCR3
#define ELEM_2_DUTY 	TB0CCR4

#define	MAX_PUMP_DUTY  	8170
#define MAX_ELEM_DUTY  	8170

#define ELEM_PERCENT	82
#define ELEM_MAINTAIN 	1634

#define HLT				0
#define MASH_TUN		1
#define KETTLE			2

void pwm_out_init();
void update_element_duty();

#endif /* DEVICES_OUTPUTS_H_ */
