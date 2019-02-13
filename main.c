#include "config.h"
#include "devices/keypad.h"
#include "devices/break_beam.h"
#include "devices/max31856.h"
#include "devices/outputs.h"
#include "devices/rtc.h"
#include "lcd/msp.h"
#include "lcd/lcd.h"
#include "ui/ui.h"

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	rtc_init();
	keypad_init();
	init_bb();
	initMSP430();
	tmp_init();
//	pwm_out_init();

	PM5CTL0 &= ~LOCKLPM5;

	delay(1);
	initLCD();
	tmp_conf();
	ui_init();

	__enable_interrupt();
	
	for (;;) {
		if(ui_mode != RECIPIE_ENTRY)
			sample_routine();
		ui_update();
		__low_power_mode_0();
	}

	return 0;
}
