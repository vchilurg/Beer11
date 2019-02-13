/*
 * ui.c
 *
 *  Created on: Nov 13, 2016
 *      Author: Wes
 */

#include "ui_internal.h"
#include "../devices/rtc.h"
#include "../lcd/graphics.h"
#include "../devices/outputs.h"
UiMode ui_mode = RECIPIE_ENTRY;
UiMode ui_mode_next = RECIPIE_ENTRY;

void ui_init() {
	clearScreen(1);
	// ui_mode = RECPIPIE_ENTRY
	ui_entry_begin();
}

void ui_update() {
	if (ui_mode != ui_mode_next) {
		if (ui_mode == RECIPIE_ENTRY) clearScreen(1);
		ui_mode = ui_mode_next;
		ui_brew_setup();
	}
	if (ui_mode == RECIPIE_ENTRY) ui_entry_paint();
	else{
		update_element_duty();
		ui_brew_paint();
	}
}
