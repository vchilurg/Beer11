/*
 * outputs.c
 *
 *  Created on: Nov 29, 2016
 *      Author: matt
 */
#include "outputs.h"
#include "max31856.h"

void pwm_out_init()
{
	//TB0 will be used to controll the PWM to the pumps and heating elements
	//The default cycle time when using SSRs is 2 seconds,
	//so the element would fire for 0.2 seconds every 2 seconds. When on, the element is on at full power.

	TB0CTL = P_E_CLOCK_STOP;								// 32 Khz, up mode, clear timer, stop
	TB0CCR0 = MAX_PUMP_DUTY;								// (32678 / 8) = 4085, *2s = 8170
	TB0CCR1 = 0;											// Pump 1
	TB0CCR2 = 0;											// Pump 2
	TB0CCR3 = 0;											// Element 1
	TB0CCR4 = 0;											// Element 2
	TB0CCTL1 = TB0CCTL2 = TB0CCTL3 = TB0CCTL4 = OUTMOD_7;

	P3DIR  |= BIT5 | BIT6 | BIT7;							//3.5 = pump 1, 3.6 = pump 2, 3.7 = element 1
	P3SEL0 |= ~(BIT5 | BIT6 | BIT7);						//mode 2 is TB0.X out
	P3SEL1 |= BIT5 | BIT6 | BIT7;

	P2DIR  |=  BIT5;										//2.5 = element 2
	P2SEL0 |=  BIT5;										//mode 1 is TB0.4 out
	P2SEL1 &= ~BIT5;										//make sure bit 5 is cleared in sel 1

	TA0CTL = TA1CTL  = SERVO_CLOCK_STOP;					// 1 MHz, Up mode
	TA0CCR0 = TA1CCR0 = 20000;								// 1 MHz / 20000 = 2ms period
	TA0CCR2 = TA1CCR2 = 0;
/*
	TA1CTL  = SERVO_CLOCK_STOP;		// 1 MHz, Up mode
	TA1CCR0 = 20000;				// 1 MHz / 20000 = 2ms period
	TA1CCR2 = 0;
*/
	P1DIR  |= BIT3 | BIT7;									// P1.3 is TA1.2 out, P1.7 is TA0.2 out
	P1SEL0 |= BIT3 | BIT7;									// Mode 1 for P1.3 is TA1.2, Mode 3 for P1.7 is TA1.2
	P1SEL1 |= BIT7;
}
void update_element_duty()
{
	switch (ui_mode)
	{
		case HEATING:
			if(float_temps[HLT] >= watch_temperature)
				ui_mode_next = ui_mode + 1;
		break;

		case MASH:
			if(float_temps[HLT] >= decoction_temp + 5)
				ELEM_1_DUTY = ELEM_MAINTAIN;
			else if(float_temps[HLT] <= decoction_temp + 5)
				ELEM_1_DUTY =MAX_ELEM_DUTY;

			if(float_temps[MASH_TUN] <= watch_temperature - 5)	//we dont want to recirculate durring mashing if we dont have too
			{
				PUMP_1_DUTY = MAX_PUMP_DUTY;					//so 5* below what we want is enough off to warrant recirculating
			}
		break;

		case HERMS:											//turn down duty cycle as we near temp to prevent overshooting
			if(float_temps[MASH_TUN] >= watch_temperature)
				ui_mode_next = ui_mode + 1;
			else if(float_temps[MASH_TUN] >= watch_temperature - 5)
				ELEM_1_DUTY = ELEM_PERCENT * 80;
			else if(float_temps[MASH_TUN] >= watch_temperature - 3)
				ELEM_1_DUTY = ELEM_PERCENT * 60;
		break;

		case BOIL_HEAT:
			if(float_temps[KETTLE] >= watch_temperature)
				ui_mode_next = ui_mode + 1;
		break;

		case CHILL_WORT:
			if(float_temps[KETTLE] <= watch_temperature)
				ui_mode_next = ui_mode + 1;
		break;
	}
}
