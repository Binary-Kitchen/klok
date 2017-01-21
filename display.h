/*
 * Klok, a 7-segment Clock
 *
 * Copyright (c) Ralf Ramsauer <ralf@ramses-pyramidenbau.de>, 2017
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <stdbool.h>

void display_init(void);
void display_scroll_left(unsigned char fill);
void display_num(unsigned char pos, unsigned char num);
void display_panic(void);
void display_dot(unsigned char pos, bool set);
void display_set_character(unsigned char pos, unsigned char mask);
