/*
 * Klok, a 7-segment Clock
 *
 * Copyright (c) Ralf Ramsauer <ralf@ramses-pyramidenbau.de>, 2017
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "display.h"
#include "rtc.h"
#include "current.h"
#include "animation.h"

static void print_time(struct time *time)
{
	display_num(1, time->second / 10);
	display_num(0, time->second % 10);

	display_num(3, time->minute / 10);
	display_num(2, time->minute % 10);

	display_num(5, time->hour / 10);
	display_num(4, time->hour % 10);

	display_dot(4, true);
	display_dot(2, true);
}

static void print_date(struct date *date)
{
	display_num(1, (date->year / 10) % 10);
	display_num(0, date->year % 10);

	display_num(3, date->month / 10);
	display_num(2, date->month % 10);

	display_num(5, date->day / 10);
	display_num(4, date->day % 10);

	display_dot(4, true);
	display_dot(2, true);
}

static void panic(void)
{
	display_panic();
	for(;;)
		asm volatile ("nop");
}

int main(void){
	int ret;
	unsigned int ctr;
	struct time time;
	struct date date;
	bool shown = false;

	MCUCSR |= (1<<JTD);
	MCUCSR |= (1<<JTD);

	rtc_init();
	display_init();
	sei();

#ifdef FLASH_DATE
	ret = rtc_set_time(&current_time);
	if (ret)
		panic();
	ret = rtc_set_date(&current_date);
	if (ret)
		panic();
#else
	animation();

	for (;;) {
		for (ctr = 0; ctr < 50; ctr++) {
			ret = rtc_get_time(&time);
			if (ret)
				display_panic();
			print_time(&time);
			_delay_ms(100);
		}

		if (!rtc_get_time(&time)) {
			if (time.minute % 5 == 0) {
				if (!shown) {
					animation();
					shown = true;
				}
			} else {
				shown = false;
			}
		}

		ret = rtc_get_date(&date);
		if (ret)
			display_panic();
		print_date(&date);
		_delay_ms(5000);
	}
#endif
	for (;;);
}
