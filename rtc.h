/*
 * Klok, a 7-segment Clock
 *
 * Copyright (c) Ralf Ramsauer <ralf@ramses-pyramidenbau.de>, 2017
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#ifndef RTC_H
#define RTC_H

#include <stdint.h>

#define MO 1
#define DI 2
#define MI 3
#define DO 4
#define FR 5
#define SA 6
#define SO 7

struct time {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
};

struct date {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t week_day;
};

int rtc_init(void);

int rtc_set_time(const struct time *time);
int rtc_get_time(struct time *time);

int rtc_set_date(const struct date *date);
int rtc_get_date(struct date *date);

#endif
