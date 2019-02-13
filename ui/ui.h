/*
 * ui.h
 *
 *  Created on: Oct 27, 2016
 *      Author: Wes
 */

#ifndef UI_UI_H_
#define UI_UI_H_

#include "../config.h"
#include "../devices/max31856.h"

typedef enum UiMode {
	RECIPIE_ENTRY,
	HEATING, // Get to temperature (hot liquor tank)
	MASH_IN, // Get to volume (break beam)
	MASH, // Timed
	HERMS, // Temperature (mash tin)
	DECOCTION, // Timed
	MASH_OUT, // Move volume (break beam 2)
	BOIL_HEAT, // Temperature (boil kettle)
	BOIL_TIMER, // Timed
	CHILL_WORT, // Temperature (boil kettle)
	BREW_COMPLETE // Done - user moves to fermenting tank
} UiMode;

void ui_init();
void ui_update();

#endif /* UI_UI_H_ */
