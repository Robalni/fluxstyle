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

#ifndef THEME_FILE_H
#define THEME_FILE_H

#include "theme.h"

#include <stdbool.h>

void destroy_theme(Theme *theme);

bool load_theme(struct theme *theme, const char *fname);

void write_theme_file(struct theme *theme, const char *fname);

#endif  /* THEME_FILE_H */
