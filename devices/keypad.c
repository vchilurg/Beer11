/*
 * keypad.c
 *
 *  Created on: Sep 22, 2015
 *      Author: Wes
 */

#include "keypad.h"

// Rows are on P3.{0-3} from top to bottom, right half of keypad pins from left to right
// Cols are on P9.{0-3} from left to right, left half of keypad pins from left to right
#define KEYPAD_PIN_MASK 0x0F
#define PRESS_FIRST  0xE /* First  row/column pressed */
#define PRESS_SECOND 0xD /* Second row/column pressed */
#define PRESS_THIRD  0xB /* Third  row/column pressed */
#define PRESS_FOURTH 0x7 /* Fourth row/column pressed */

// Uses Timer A2.0 for debouncing
#define TIMER_CFG_STOP TASSEL__ACLK | MC__STOP | TACLR;
#define TIMER_CFG_UP   TASSEL__ACLK | MC__UP   | TACLR;

static bool null_handler(KeypadKeycode k) { return false; }
static bool (*handler)(KeypadKeycode) = null_handler;
void keypad_set_handler(bool (*fp)(KeypadKeycode)) { handler = fp ? fp : null_handler; }

static const KeypadKeycode decode_table[32] = {
		KEYPAD_KEY_1, KEYPAD_KEY_4, KEYPAD_KEY_7, KEYPAD_KEY_STAR,
		KEYPAD_KEY_2, KEYPAD_KEY_5, KEYPAD_KEY_8, KEYPAD_KEY_0,
		KEYPAD_KEY_3, KEYPAD_KEY_6, KEYPAD_KEY_9, KEYPAD_KEY_HASH,
		KEYPAD_KEY_A, KEYPAD_KEY_B, KEYPAD_KEY_C, KEYPAD_KEY_D,
		// if NONE bit is set return NONE
		KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE,
		KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE,
		KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE,
		KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE
};

// find first cleared ->              0x0  0x1  0x2  0x3  0x4  0x5  0x6  0x7  0x8  0x9  0xA  0xB  0xC  0xD  0xE  0xF
static const uint8_t find_row[16] = { 0x0, 0x1, 0x0, 0x2, 0x0, 0x1, 0x0, 0x3, 0x0, 0x1, 0x0, 0x2, 0x0, 0x1, 0x0, 0x10 };
static const uint8_t find_col[16] = { 0x0, 0x4, 0x0, 0x8, 0x0, 0x4, 0x0, 0xC, 0x0, 0x4, 0x0, 0x8, 0x0, 0x4, 0x0, 0x10 };

// switch sense direction
__force_inline void scan_rows() { P3DIR &= ~KEYPAD_PIN_MASK; P3OUT |=  KEYPAD_PIN_MASK; P9DIR |=  KEYPAD_PIN_MASK; P9OUT &= ~KEYPAD_PIN_MASK; }
__force_inline void scan_cols() { P3DIR |=  KEYPAD_PIN_MASK; P3OUT &= ~KEYPAD_PIN_MASK; P9DIR &= ~KEYPAD_PIN_MASK; P9OUT |=  KEYPAD_PIN_MASK; }

void keypad_init() {
	// Output mode automatically bypasses resistors, so leaving them on is fine
	P3REN |= KEYPAD_PIN_MASK;			// Enable pull-ups on P3.{0-3}
	P9REN |= KEYPAD_PIN_MASK;			// Enable pull-ups on P9.{0-3}
	scan_rows();						// Interrupts are on row pins

	P3IES |= KEYPAD_PIN_MASK;			// Select falling edge (keypress) on row pins
	P3IFG &= ~KEYPAD_PIN_MASK;			// Clear interrupt flags
	P3IE |= KEYPAD_PIN_MASK;			// Enable interrupts on row pins

	TA2CCR0 = 819;						// 25 ms * 32768 Hz = 819.2 ticks delay
	TA2CTL = TIMER_CFG_STOP;			// Configure debounce timer but don't start it
}

__isr(PORT3_VECTOR, keypad_detect) {
	P3IFG &= ~KEYPAD_PIN_MASK;			// Clear interrupt flags

	if (!(P3IES & KEYPAD_PIN_MASK))		// Check if this was a release
		if ((P3IN & KEYPAD_PIN_MASK) != KEYPAD_PIN_MASK)
			return;						// Don't "release" until all keys are up

	P3IE &= ~KEYPAD_PIN_MASK;			// Disable keypad interrupts for processing
	P3IES ^= KEYPAD_PIN_MASK;			// Switch edges (waiting for press/release)

	TA2CCTL0 = CCIE;					// Enable debounce timer interrupt
	TA2CTL = TIMER_CFG_UP;				// Start debounce timer
}

__isr(TIMER2_A0_VECTOR, keypad_debounce) {
	TA2CCTL0 = 0;								// Disable debounce timer interrupt
	TA2CTL = TIMER_CFG_STOP;					// Stop debounce timer

	// Since P3IES was switched in keypad_detect() this test must be inverted
	if (!(P3IES & KEYPAD_PIN_MASK)) {			// Only decode if key was pressed
		const uint8_t row = find_row[P3IN & KEYPAD_PIN_MASK];
		scan_cols();	// TODO Add debouncing into second timer?
		__delay_cycles(200);
		const uint8_t col = find_col[P9IN & KEYPAD_PIN_MASK];
		scan_rows();							// Back to rows for next interrupt
		const KeypadKeycode decoded_key = decode_table[col | row];
		if (decoded_key != KEYPAD_KEY_NONE)
			if (handler(decoded_key))
				__low_power_mode_off_on_exit();	// Wake-up for processing
	}

	P3IE |= KEYPAD_PIN_MASK;					// Re-enable keypad interrupts
}
