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

#include "color.h"

#include <stdio.h>

void hsl_to_hex(char *hex, float hue, float sat, float light)
{
  int r, g, b;
  float x, y;
  hue /= 360.0;
  sat /= 100.0;
  light /= 100.0;
  if (light < 0.5) {
    y = light * (1 + sat);
  } else {
    y = (light + sat) - (light * sat);
  }
  x = 2 * light - y;
  r = 255 * hue_to_rgb(x, y, hue + 1.0/3.0);
  g = 255 * hue_to_rgb(x, y, hue);
  b = 255 * hue_to_rgb(x, y, hue - 1.0/3.0);
  snprintf(hex, 8, "#%02X%02X%02X", r, g, b);
}

void
hsl_to_rgb(float *r, float *g, float *b, float hue, float sat, float light)
{
  float x, y;
  hue /= 360.0;
  sat /= 100.0;
  light /= 100.0;
  if (light < 0.5) {
    y = light * (1 + sat);
  } else {
    y = (light + sat) - (light * sat);
  }
  x = 2 * light - y;
  *r = hue_to_rgb(x, y, hue + 1.0/3.0);
  *g = hue_to_rgb(x, y, hue);
  *b = hue_to_rgb(x, y, hue - 1.0/3.0);
}

float hue_to_rgb(float x, float y, float hue)
{
  if (hue < 0) {
    hue += 1;
  } else if (hue > 1) {
    hue -= 1;
  }
  if (hue * 6 < 1) {
    return x + (y-x) * 6 * hue;
  } else if (2 * hue < 1) {
    return y;
  } else if (3 * hue < 2) {
    return x + (y-x) * (2.0/3.0 - hue) * 6;
  } else {
    return x;
  }
}
