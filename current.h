/*
 * Klok, a 7-segment Clock
 *
 * Copyright (c) Ralf Ramsauer <ralf@ramses-pyramidenbau.de>, 2017
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

//#define FLASH_DATE

static const struct time current_time = {
	.hour = 19,
	.minute = 52,
	.second = 0,
};

static const struct date current_date = {
	.year = 2017,
	.month = 1,
	.day = 21,
	.week_day = SA,
};
