#ifndef COLOR_H
#define COLOR_H

void hsl_to_hex(char *hex, float hue, float sat, float light);

float hue_to_rgb(float x, float y, float hue);

void
hsl_to_rgb(float *r, float *g, float *b, float hue, float sat, float light);

#endif  /* COLOR_H */
