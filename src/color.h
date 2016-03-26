/*
  fluxstyle - Fluxbox theme creator

  Written in 2016 by Robert Alm Nilsson rorialni@gmail.com

  To the extent possible under law, the author(s) have dedicated all
  copyright and related and neighboring rights to this software to the
  public domain worldwide. This software is distributed without any
  warranty.

  You should have received a copy of the CC0 Public Domain Dedication
  along with this software. If not, see
  <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef COLOR_H
#define COLOR_H

void hsl_to_hex(char *hex, float hue, float sat, float light);

float hue_to_rgb(float x, float y, float hue);

void
hsl_to_rgb(float *r, float *g, float *b, float hue, float sat, float light);

#endif  /* COLOR_H */
