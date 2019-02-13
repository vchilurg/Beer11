/*
 * ui_internal.h
 *
 *  Created on: Nov 13, 2016
 *      Author: Wes
 */

#ifndef UI_UI_INTERNAL_H_
#define UI_UI_INTERNAL_H_

#include "ui.h"

extern UiMode ui_mode, ui_mode_next;

extern unsigned mash_temp;
extern unsigned mash_weight;
extern unsigned mash_time;
extern unsigned mash_vol;
extern unsigned decoction_time;
extern unsigned decoction_temp;
extern unsigned boil_time;
extern unsigned boil_vol;
extern unsigned num_adds;
extern uint16_t add_times[10];
extern unsigned chill_temp;
extern unsigned watch_temperature;
// each UI state has a begin and paint methods
// it is assumed that on a state transition, the screen is cleared

void ui_entry_begin();
void ui_entry_paint();

void ui_brew_setup();
void ui_brew_paint();

#endif /* UI_UI_INTERNAL_H_ */
