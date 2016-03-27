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
#include "theme.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

static int scale(int n, float s);

static int skip_chars(FILE *f, const char *chars);

static int read_until(FILE *f, char **str, const char *chars);

void destroy_theme(Theme *theme)
{
  free(theme->file_name);
}

bool load_theme(Theme *theme, const char *fname)
{
  memset(theme, 0, sizeof (Theme));
  theme->file_name = malloc(strlen(fname) + 1);
  strcpy(theme->file_name, fname);
  theme->size = 9;

  FILE *f;
  if (fname[0] == '-' && fname[1] == 0) {
    f = fdopen(0, "r");
    if (f == NULL) {
      fputs("Could not read theme file!\n", stderr);
      perror("stdin");
      return false;
    }
  } else {
    f = fopen(fname, "r");
    if (f == NULL) {
      fputs("Could not read theme file!\n", stderr);
      perror(fname);
      return false;
    }
  }

  int ch;
  bool is_comment;
  char *key;
  char *value;
  do {
    skip_chars(f, " \t");
    is_comment = (fgetc(f) == '#');
    skip_chars(f, "# \t");
    read_until(f, &key, ": \t");
    skip_chars(f, ": \t");
    read_until(f, &value, "\r\n");
    skip_chars(f, "\r\n");

    if (key && value) {
      if (strcmp(key, "hue") == 0) {
        theme->hue = atof(value);
      } else if (strcmp(key, "saturation") == 0) {
        theme->saturation = atof(value);
      } else if (strcmp(key, "size") == 0) {
        theme->size = atoi(value);
      }
    }

    free(key);
    free(value);
  } while (is_comment);

  if (!(fname[0] == '-' && fname[1] == 0)) {
    fclose(f);
  }

  return true;
}

void write_theme_file(Theme *theme, const char *fname)
{
  FILE *f;
  if (fname[0] == '-' && fname[1] == 0) {
    f = fdopen(1, "w");
    if (f == NULL) {
      fputs("Could not write theme file!\n", stderr);
      perror("stdout");
      return;
    }
  } else {
    f = fopen(fname, "w");
    if (f == NULL) {
      fputs("Could not write theme file!\n", stderr);
      perror(fname);
      return;
    }
  }
  char color[] = "#000000";
  int font_size = theme->size;
  int borderw = scale(font_size, 0.15);
  int height = scale(font_size, 2.5);

  fprintf(f, "# Generated by fluxstyle\n");
  fprintf(f, "# hue: %.1f\n", theme->hue);
  fprintf(f, "# saturation: %.1f\n", theme->saturation);
  fprintf(f, "# size: %d\n", theme->size);

  fprintf(f, "window.*.focus: flat\n");
  fprintf(f, "window.*.unfocus: flat\n");
  fprintf(f, "window.justify: center\n");
  fprintf(f, "background: flat\n");
  fprintf(f, "menu.title: flat\n");
  fprintf(f, "menu.title.justify: center\n");
  fprintf(f, "menu.bullet: triangle\n");
  fprintf(f, "menu.bullet.position: right\n");
  fprintf(f, "window.borderWidth: %d\n", borderw);
  fprintf(f, "menu.borderWidth: %d\n", borderw);
  fprintf(f, "toolbar.borderWidth: %d\n", borderw);
  fprintf(f, "*.font: sans-%d\n", font_size);
  fprintf(f, "toolbar.height: %d\n", height);
  fprintf(f, "window.title.height: %d\n", height);
  fprintf(f, "menu.titleHeight: %d\n", height);
  fprintf(f, "menu.itemHeight: %d\n", height);
  fprintf(f, "window.handleWidth: %d\n", scale(height, 0.2));
  fprintf(f, "window.bevelWidth: %d\n", scale(height, 0.2));
  hsl_to_hex(color, theme->hue, theme->saturation, 10);
  fprintf(f, "background.color: %s\n", color);
  hsl_to_hex(color, theme->hue, 0, 100);
  fprintf(f, "menu.frame.color: %s\n", color);
  fprintf(f, "menu.title.textColor: %s\n", color);
  fprintf(f, "toolbar.iconbar.focused.textColor: %s\n", color);
  hsl_to_hex(color, theme->hue, 0, 70);
  fprintf(f, "window.label.unfocus.textColor: %s\n", color);
  fprintf(f, "window.button.unfocus.picColor: %s\n", color);
  fprintf(f, "toolbar.iconbar.unfocused.textColor: %s\n", color);
  hsl_to_hex(color, theme->hue, 0, 10);
  fprintf(f, "menu.title.color: %s\n", color);
  fprintf(f, "window.*.unfocus.color: %s\n", color);
  fprintf(f, "toolbar.*.color: %s\n", color);
  hsl_to_hex(color, theme->hue, theme->saturation, 20);
  fprintf(f, "window.*.focus.color: %s\n", color);
  fprintf(f, "window.button.pressed.color: %s\n", color);
  fprintf(f, "toolbar.iconbar.focused.color: %s\n", color);
  hsl_to_hex(color, theme->hue, theme->saturation, 5);
  fprintf(f, "background.color: %s\n", color);
  hsl_to_hex(color, theme->hue, theme->saturation, 0);
  fprintf(f, "window.borderColor: %s\n", color);
  fprintf(f, "menu.borderColor: %s\n", color);
  fprintf(f, "toolbar.borderColor: %s\n", color);
  fprintf(f, "menu.hilite.textColor: %s\n", color);
  fprintf(f, "menu.frame.textColor: %s\n", color);
  hsl_to_hex(color, theme->hue, theme->saturation, 70);
  fprintf(f, "menu.hilite.color: %s\n", color);

  if (!(fname[0] == '-' && fname[1] == 0)) {
    fclose(f);
  }
}

/********************/
/* Static functions */

static int scale(int n, float s)
{
  return (int)((float)n * s);
}

static int skip_chars(FILE *f, const char *chars)
{
  int skipped_chars = 0;
  bool char_found;
  int ch;
  int i;
  do {
    i = 0;
    char_found = false;
    ch = fgetc(f);
    while (chars[i] != 0) {
      if (ch == chars[i]) {
        char_found = true;
        skipped_chars++;
        break;
      }
      i++;
    }
  } while (char_found);
  ungetc(ch, f);
  return skipped_chars;
}

static int read_until(FILE *f, char **str, const char *chars)
{
  int read_chars = 0;
  int ch;
  int i;
  *str = NULL;
  while (true) {
    i = 0;
    ch = fgetc(f);
    if (read_chars % 16 == 0) {
      *str = realloc(*str, read_chars + 17);  /* One more for '\0' */
    }
    while (chars[i] != 0) {
      if (ch == chars[i] || ch == EOF) {
        ungetc(ch, f);
        (*str)[read_chars] = '\0';
        return read_chars;
      }
      i++;
    }
    (*str)[read_chars] = ch;
    read_chars++;
  }
  return read_chars;
}
