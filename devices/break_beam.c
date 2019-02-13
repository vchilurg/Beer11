/*
 * break_beam.c
 *
 *  Created on: Nov 29, 2016
 *      Author: matt
 */
#include "break_beam.h"

uint8_t inter_flag;

void init_bb()
{
//	P3IN = 0;
	P2DIR &= ~(MASH_TUN_BB | KETTLE_BB);
	P2OUT |= MASH_TUN_BB | KETTLE_BB;
//	P3REN = MASH_TUN_BB | KETTLE_BB;
	P2IES = MASH_TUN_BB | KETTLE_BB;
	P2IFG = 0;
}

void set_bb_state()
{
	switch(ui_mode)
	{
		case MASH_IN:
			P2IE = MASH_TUN_BB;
		break;

		case MASH_OUT:
			P2IE = KETTLE_BB;
		break;

		default: break;
	}
	P2IFG = 0;
	P2IV = 0;
}
__isr(PORT2_VECTOR, p2_ISR){
	inter_flag = P2IV;
	switch(inter_flag)					  // algorithm to find which row press was in
	{
		case P2IV_P2IFG6:
			if(ui_mode == MASH_IN) ui_mode_next = ui_mode + 1;
			break;
		case P2IV_P2IFG7:
			if(ui_mode == MASH_OUT) ui_mode_next = ui_mode + 1;
			break;
		default: break;
	}
	P2IFG = 0;
//	P2IE = 0;
	__low_power_mode_off_on_exit();
}
