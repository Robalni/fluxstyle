#ifndef THEME_FILE_H
#define THEME_FILE_H

#include "theme.h"

#include <stdbool.h>

void destroy_theme(Theme *theme);

bool load_theme(struct theme *theme, const char *fname);

void write_theme_file(struct theme *theme, const char *fname);

#endif  /* THEME_FILE_H */
