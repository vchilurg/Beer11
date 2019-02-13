/*
 * ui_brew.c
 *
 *  Created on: Nov 29, 2016
 *      Author: Wes
 */

#include "ui_internal.h"
#include "../devices/keypad.h"
#include "../devices/outputs.h"
#include "../devices/break_beam.h"
#include "../devices/rtc.h"
#include "../lcd/color.h"
#include "../lcd/graphics.h"

static unsigned add_time_index = 0;
unsigned watch_temperature = 0;

static bool key_handler(KeypadKeycode key) {
	if (ui_mode != MASH && ui_mode != DECOCTION && ui_mode != BOIL_TIMER && ui_mode != BREW_COMPLETE
			&& key == KEYPAD_KEY_HASH) {
		ui_mode_next = ui_mode + 1; // Button advance through non-timed segments for now
		return true;
	}
	return false; // Everything else is on a timer
}

static bool alarm_handler() {
	if (ui_mode == MASH) ui_mode_next = HERMS;
	else if (ui_mode == DECOCTION) ui_mode_next = MASH_OUT;
	else if (ui_mode == BOIL_TIMER) {
		// Need to set alarms manually here since there isn't a state transition
		if (++add_time_index < num_adds) // Next addition
			rtc_set_alarm_min(add_times[add_time_index]);
		else if (add_time_index == num_adds) // Finish boil
			rtc_set_alarm_min(boil_time);
		else ui_mode_next = CHILL_WORT;
	} else return false;
	return true;
}

void ui_brew_setup() {
	keypad_set_handler(key_handler);
	rtc_set_alarm_fun(alarm_handler);
	rtc_clear_alarm();
	rtc_init();

	switch (ui_mode) {
		case HEATING:
			TA3CCTL0 = CCIE;
			TA3CTL =  SAMPLE_TIMER_START; 			// start the temperature sampling timer
			ELEM_1_DUTY = MAX_ELEM_DUTY;  			// set HLT heating element duty cycle to 100% to heat
			BCD2BIN = mash_temp;
			watch_temperature = BCD2BIN + 5;	  	// heat to desired mash temp + 5 to account for heat loss
													// durring transfer and absorbed by colder grains
			break;

		case MASH_IN:
			ELEM_1_DUTY = ELEM_MAINTAIN; 		//set HLT heating element duty cycle to 20% to heat
			//open valve sequence
			PUMP_1_DUTY = MAX_PUMP_DUTY; 		//set Pump 1 duty cycle to 100% transfer
			set_bb_state();				 		//enables interrupt on P1.3 for mash tun bb sensor
			break;

		case MASH:
			P2IE = 0;
			//close all valves
			ELEM_1_DUTY = MAX_ELEM_DUTY;		//once we begin the mash stage, start heating the HLT water to decoc temp;
			BCD2BIN = mash_temp;
			watch_temperature = BCD2BIN;
			rtc_set_alarm_min(mash_time);
			break;

		case HERMS:
			//open valve sequence for mash tun recirc
			PUMP_1_DUTY = MAX_PUMP_DUTY;	//set Pump 1 duty cycle to 100%
			BCD2BIN = decoction_temp;
			watch_temperature = BCD2BIN;
			break;

		case DECOCTION:	//protein rest
			//close recirc valve sequence
			PUMP_1_DUTY = 0;
			rtc_set_alarm_min(decoction_time);
			break;

		case MASH_OUT:
			//open valve sequence for mash out
			PUMP_1_DUTY = MAX_PUMP_DUTY;	//set Pump 1 duty cycle to 100%
			PUMP_2_DUTY = MAX_PUMP_DUTY;	//set Pump 2 duty cycle to 100%
			set_bb_state();
			break;

		case BOIL_HEAT:
			P2IE = 0;
			//close valve sequence for mash out;
			PUMP_1_DUTY = 0;
			PUMP_2_DUTY = 0;
			watch_temperature = 212;
			ELEM_2_DUTY = MAX_ELEM_DUTY;	//set Kettle heating element duty cycle to 100% for heating
			break;

		case BOIL_TIMER:
			ELEM_2_DUTY = ELEM_MAINTAIN;    //set Kettle heating element duty cycle to 20% to maintain
			rtc_set_alarm_min(num_adds == 0 ? boil_time : add_times[0]);
			break;

		case CHILL_WORT:
			//open valve sequence for chilling wort
			PUMP_2_DUTY = MAX_PUMP_DUTY;	//set Pump 2 dudty cycle to 100% for recirculation
			BCD2BIN = chill_temp;
			watch_temperature = BCD2BIN; //chill_temp;
			break;
		case BREW_COMPLETE:
			//close valve sequence for chilling
			PUMP_1_DUTY = PUMP_2_DUTY = ELEM_1_DUTY = ELEM_2_DUTY = 0;
			TB0CTL = P_E_CLOCK_STOP;
			break;


		default: break;

	}
}

// 18 chars max
static const char state_names[BREW_COMPLETE + 1][19] = {
	"Recipie Entry     ",
	"Heating HLT       ",
	"Adding to Mash    ",
	"Steeping Mash     ",
	"H.E.R.M.S.        ",
	"De-cocting Mash   ",
	"Moving to Boiler  ",
	"Heating Boiler    ",
	"Boiling Wort      ",
	"Chilling Wort     ",
	"Brew Complete!    ",
};

static char cur_hlt_temp_fmt [5] = "   0";
static char cur_mash_temp_fmt[5] = "   0";
static char cur_boil_temp_fmt[5] = "   0";
static bool repaint_prompt = true;

void ui_brew_paint() {

	setColor(COLOR_16_RED); setBackgroundColor(COLOR_16_BLACK);
	if (repaint_prompt) {
		repaint_prompt = false;
		drawString(10, 25, FONT_MD_BKG, "Cur. Time: ");
		drawString(10, 40, FONT_MD_BKG, "Next Time: ");
		drawString(10, 55, FONT_MD_BKG, "HLT  Temp  :     F"); drawCharSm(140, 55, 0x7f);
		drawString(10, 70, FONT_MD_BKG, "Mash Temp  :     F"); drawCharSm(140, 70, 0x7f);
		drawString(10, 85, FONT_MD_BKG, "Kettle Temp:     F"); drawCharSm(140, 85, 0x7f);
	}

	bcd16toa_s(cur_hlt_temp_fmt , conv_temps[0]);
	bcd16toa_s(cur_mash_temp_fmt, conv_temps[1]);
	bcd16toa_s(cur_boil_temp_fmt, conv_temps[2]);

	drawString(10, 10, FONT_MD_BKG, state_names[ui_mode]);
	drawString(10 + 11*8, 25, FONT_MD_BKG, time_display);
	drawString(10 + 11*8, 40, FONT_MD_BKG, alarm_display);
	drawString(10 + 12*8, 55, FONT_MD_BKG, cur_hlt_temp_fmt  + 1);
	drawString(10 + 12*8, 70, FONT_MD_BKG, cur_mash_temp_fmt + 1);
	drawString(10 + 12*8, 85, FONT_MD_BKG, cur_boil_temp_fmt + 1);
}

