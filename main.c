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

static int print_time(void)
{
	int ret;
	struct time time;

	ret = rtc_get_time(&time);
	if (ret)
		return ret;

	display_num(1, time.second / 10);
	display_num(0, time.second % 10);

	display_num(3, time.minute / 10);
	display_num(2, time.minute % 10);

	display_num(5, time.hour / 10);
	display_num(4, time.hour % 10);

	display_dot(4, true);
	display_dot(2, true);

	return 0;
}

static int print_date(void)
{
	int ret;
	struct date date;

	ret = rtc_get_date(&date);
	if (ret)
		return ret;

	display_num(1, (date.year / 10) % 10);
	display_num(0, date.year % 10);

	display_num(3, date.month / 10);
	display_num(2, date.month % 10);

	display_num(5, date.day / 10);
	display_num(4, date.day % 10);

	display_dot(4, true);
	display_dot(2, true);

	return 0;
}

static void panic(void)
{
	display_panic();
	for(;;)
		asm volatile ("nop");
}

static void circle(unsigned char pos, unsigned char width, unsigned int delay)
{
	unsigned int i;

	display_clear();
	for (i = pos; i <= pos + width; i++) {
		display_set_character(i, BOTTOM);
		_delay_ms(delay);
	}

	display_or_character(pos + width, BOTTOM_LEFT);
	_delay_ms(delay);

	display_or_character(pos + width, TOP_LEFT);
	_delay_ms(delay);

	for (i = pos ; i <= pos + width; i++) {
		display_or_character(i, TOP);
		_delay_ms(delay);
	}

	display_or_character(pos, TOP_RIGHT);
	_delay_ms(delay);

	display_or_character(pos, BOTTOM_RIGHT);
	_delay_ms(delay);
}

static void animation(void)
{
	unsigned int i;
	unsigned int delay;

	delay = 25;
	circle(0, 5, delay);
	_delay_ms(250);
	circle(1, 3, delay);
	_delay_ms(250);
	circle(2, 1, delay);
	_delay_ms(250);
	for (i = 0; i < SEGMENTS; i++)
		circle(i, 0, delay);
	for (i = 0; i < SEGMENTS; i++)
		circle(SEGMENTS - 1 - i, 0, delay);
}

int main(void){
	int ret;
	unsigned int ctr;
	struct time time;
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
			ret = print_time();
			if (ret)
				display_panic();
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

		ret = print_date();
		if (ret)
			display_panic();
		_delay_ms(5000);
	}
#endif
	for (;;);
}
