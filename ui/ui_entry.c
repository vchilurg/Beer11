/*
 * ui_entry.c
 *
 *  Created on: Nov 13, 2016
 *      Author: Wes
 */

#include "ui_internal.h"
#include "../devices/keypad.h"
#include "../lcd/color.h"
#include "../lcd/graphics.h"

enum UiEntryCategory {
	MASH_TEMP, MASH_LBS, MASH_TIME, MO_TIME, MO_TEMP, BOIL_TIME, BOIL_VOL, BOIL_ADDS, ADD_TIMES, CHILL_TEMP
} entry_category = MASH_TEMP;

unsigned mash_temp = 0;
unsigned mash_weight = 0;
unsigned mash_time = 0;
unsigned decoction_time = 0;
unsigned decoction_temp = 0;
unsigned boil_time = 0;
unsigned boil_vol = 0;
unsigned num_adds = 0;
uint16_t add_times[10] = {0};
unsigned chill_temp = 0;

static unsigned num_adds_index = 0;
static bool repaint_prompt = true;

static unsigned* entry_pointers[CHILL_TEMP + 1] = {
	&mash_temp, &mash_weight, &mash_time, &decoction_time, &decoction_temp,
	&boil_time, &boil_vol, &num_adds, &add_times[0], &chill_temp
};

static uint16_t* get_entry_value() {
	return (entry_category != ADD_TIMES) ?
			entry_pointers[entry_category] : (add_times + num_adds_index);
}

static bool key_handler(KeypadKeycode key) {
	uint16_t* number_bcd = get_entry_value();
	if (key == KEYPAD_KEY_STAR) *number_bcd >>= 4;
	else if (key == KEYPAD_KEY_HASH) {
		repaint_prompt = true;
		if (entry_category < ADD_TIMES)
			entry_category++;
		else if (entry_category == CHILL_TEMP)
			ui_mode_next = HEATING;
		else if (++num_adds_index >= num_adds)
			entry_category = CHILL_TEMP;
	} else if (key < KEYPAD_KEY_A && *number_bcd < 0x100U) {
		*number_bcd <<= 4;
		*number_bcd |= key;
	} else return false;
	return true;
}

void ui_entry_begin() {
	keypad_set_handler(key_handler);
}

static const char* entry_prompts[2*CHILL_TEMP + 2] = {
	"Enter your", "Mash temp.  ",
	"Enter your", "Mash lbs.   ",
	"Enter your", "Mash time   ",
	"Enter your", "Mashout time",
	"Enter your", "Mashout temp",
	"Enter your", "Boil time   ",
	"Enter your", "Boil volume ",
	"Enter the ", "# of adds   ",
	"Enter time", "of add.     ",
	"Enter your", "Chill temp. ",
};

static char entry_fmt[5] = "   0";

void ui_entry_paint() {
	setColor(COLOR_16_RED); setBackgroundColor(COLOR_16_BLACK);
	if (repaint_prompt) {
		repaint_prompt = false;
		drawString(10, 10, FONT_LG_BKG, entry_prompts[(entry_category << 1)]);
		drawString(10, 45, FONT_LG_BKG, entry_prompts[(entry_category << 1) + 1]);
		if (entry_category == ADD_TIMES)
			drawCharLg(10 + 8*11, 45, digits[num_adds_index + 1]);
	}
	uint16_t value = *get_entry_value();
	bcd16toa_s(entry_fmt, value);
	drawString(10, 80, FONT_LG_BKG, entry_fmt);
}
