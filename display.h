/*
 * Klok, a 7-segment Clock
 *
 * Copyright (c) Ralf Ramsauer <ralf@ramses-pyramidenbau.de>, 2017
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <stdbool.h>

#define MID	     (1<<0)
#define TOP_LEFT     (1<<1)
#define TOP	     (1<<2)
#define TOP_RIGHT    (1<<3)
#define DOT	     (1<<4)
#define BOTTOM_RIGHT (1<<5)
#define BOTTOM	     (1<<6)
#define BOTTOM_LEFT  (1<<7)

void display_init(void);
void display_scroll_left(unsigned char fill);
void display_num(unsigned char pos, unsigned char num);
void display_panic(void);
void display_dot(unsigned char pos, bool set);
void display_set_character(unsigned char pos, unsigned char mask);
