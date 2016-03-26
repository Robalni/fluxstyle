#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>

struct {
  float hue;
  float sat;
} values;

struct surface {
  cairo_surface_t *surf;
  cairo_t *cr;
  GtkWidget *image;
};

cairo_surface_t *make_surface();

void update_color(struct surface *surf);

void hue_changed(GtkAdjustment *adjustment, gpointer data);

void sat_changed(GtkAdjustment *adjustment, gpointer data);

void apply_theme(GtkButton *button, gpointer data);

void hsl_to_hex(char *hex, float hue, float sat, float light);

void
hsl_to_rgb(float *r, float *g, float *b, float hue, float sat, float light);

float hue_to_rgb(float x, float y, float hue);

void write_theme_file(const char *fname);

void restart_fluxbox();

bool file_contains(const char *fname, const unsigned char *str);

int main(int argc, char *argv[])
{
  GtkWidget *window;
  gtk_init(&argc, &argv);

  char *theme_file;
  if (argc == 2) {
    theme_file = argv[1];
  } else {
    theme_file = "-";
  }

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_window_set_title(GTK_WINDOW(window), "Make Fluxbox theme");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 20);

  struct surface surf;
  surf.surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 30, 30);
  surf.cr = cairo_create(surf.surf);
  surf.image = gtk_image_new_from_surface(surf.surf);

  GtkAdjustment *hue_adjustment = gtk_adjustment_new(0, 0, 360, 1, 0, 0);
  g_signal_connect(hue_adjustment, "value-changed", G_CALLBACK(hue_changed),
                   &surf);
  GtkWidget *hue_label = gtk_label_new("Hue");
  GtkWidget *hue_scale = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,
                                       hue_adjustment);
  gtk_scale_add_mark(GTK_SCALE(hue_scale), 60, GTK_POS_TOP, NULL);
  gtk_scale_add_mark(GTK_SCALE(hue_scale), 120, GTK_POS_TOP, NULL);
  gtk_scale_add_mark(GTK_SCALE(hue_scale), 180, GTK_POS_TOP, NULL);
  gtk_scale_add_mark(GTK_SCALE(hue_scale), 240, GTK_POS_TOP, NULL);
  gtk_scale_add_mark(GTK_SCALE(hue_scale), 300, GTK_POS_TOP, NULL);

  GtkAdjustment *sat_adjustment = gtk_adjustment_new(0, 0, 100, 1, 0, 0);
  g_signal_connect(sat_adjustment, "value-changed", G_CALLBACK(sat_changed),
                   &surf);
  GtkWidget *sat_label = gtk_label_new("Saturation");
  GtkWidget *sat_scale = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,
                                       sat_adjustment);
  gtk_widget_set_hexpand(sat_scale, true);

  GtkWidget *apply_btn = gtk_button_new_with_label("Apply");
  g_signal_connect(apply_btn, "clicked", G_CALLBACK(apply_theme), theme_file);

  GtkWidget *cancel_btn = gtk_button_new_with_label("Cancel");
  g_signal_connect(cancel_btn, "clicked", G_CALLBACK(gtk_main_quit), NULL);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(grid, 4);
  gtk_grid_set_column_spacing(grid, 4);
  gtk_grid_attach(GTK_GRID(grid), surf.image, 0, 0, 2, 1);
  gtk_grid_attach(GTK_GRID(grid), hue_label, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), hue_scale, 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), sat_label, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), sat_scale, 1, 2, 1, 1);

  GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  gtk_box_pack_end(GTK_BOX(button_box), apply_btn, false, false, 0);
  gtk_box_pack_end(GTK_BOX(button_box), cancel_btn, false, false, 0);
  gtk_grid_attach(GTK_GRID(grid), button_box, 0, 3, 2, 1);

  gtk_container_add(GTK_CONTAINER(window), grid);
  gtk_container_set_border_width(GTK_CONTAINER(window), 8);

  gtk_widget_show_all(window);
  update_color(&surf);
  gtk_main();

  return 0;
}

void update_color(struct surface *surf)
{
  float r, g, b;
  hsl_to_rgb(&r, &g, &b, values.hue, values.sat, 20);
  cairo_rectangle(surf->cr, 1, 1, 28, 28);
  cairo_set_source_rgb(surf->cr, r, g, b);
  cairo_fill(surf->cr);
  gtk_widget_queue_draw(surf->image);
}

void hue_changed(GtkAdjustment *adjustment, gpointer data)
{
  struct surface *surf = (struct surface*)data;
  values.hue = gtk_adjustment_get_value(adjustment);
  update_color(surf);
}

void sat_changed(GtkAdjustment *adjustment, gpointer data)
{
  struct surface *surf = (struct surface*)data;
  values.sat = gtk_adjustment_get_value(adjustment);
  update_color(surf);
}

void apply_theme(GtkButton *button, gpointer data)
{
  char *fname = (char*)data;
  write_theme_file(fname);
  restart_fluxbox();
}

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

static int scale(int n, float s)
{
  return (int)((float)n * s);
}

void write_theme_file(const char *fname)
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
  int font_size = 9;
  int borderw = scale(font_size, 0.15);
  int height = scale(font_size, 2.5);

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
  hsl_to_hex(color, values.hue, values.sat, 10);
  fprintf(f, "background.color: %s\n", color);
  hsl_to_hex(color, values.hue, 0, 100);
  fprintf(f, "menu.frame.color: %s\n", color);
  fprintf(f, "menu.title.textColor: %s\n", color);
  fprintf(f, "toolbar.iconbar.focused.textColor: %s\n", color);
  hsl_to_hex(color, values.hue, 0, 70);
  fprintf(f, "window.label.unfocus.textColor: %s\n", color);
  fprintf(f, "window.button.unfocus.picColor: %s\n", color);
  fprintf(f, "toolbar.iconbar.unfocused.textColor: %s\n", color);
  hsl_to_hex(color, values.hue, 0, 10);
  fprintf(f, "menu.title.color: %s\n", color);
  fprintf(f, "window.*.unfocus.color: %s\n", color);
  fprintf(f, "toolbar.*.color: %s\n", color);
  hsl_to_hex(color, values.hue, values.sat, 20);
  fprintf(f, "window.*.focus.color: %s\n", color);
  fprintf(f, "window.button.pressed.color: %s\n", color);
  fprintf(f, "toolbar.iconbar.focused.color: %s\n", color);
  hsl_to_hex(color, values.hue, values.sat, 5);
  fprintf(f, "background.color: %s\n", color);
  hsl_to_hex(color, values.hue, values.sat, 0);
  fprintf(f, "window.borderColor: %s\n", color);
  fprintf(f, "menu.borderColor: %s\n", color);
  fprintf(f, "toolbar.borderColor: %s\n", color);
  fprintf(f, "menu.hilite.textColor: %s\n", color);
  fprintf(f, "menu.frame.textColor: %s\n", color);
  hsl_to_hex(color, values.hue, values.sat, 70);
  fprintf(f, "menu.hilite.color: %s\n", color);

  if (!(fname[0] == '-' && fname[1] == 0)) {
    fclose(f);
  }
}

void restart_fluxbox()
{
  DIR *proc_dir = opendir("/proc");
  if (proc_dir == NULL) {
    fputs("Could not restart fluxbox!\n", stderr);
    perror("opendir");
    return;
  }
  struct dirent *proc_ent;
  const int full_path_length = 30;  /* 30 > (length of "/proc/32768/comm") */
  char full_path[full_path_length];
  pid_t fluxbox_pid;
  bool fluxbox_was_killed = false;
  while ((proc_ent = readdir(proc_dir)) != NULL) {
    if (proc_ent->d_name[0] >= '1' && proc_ent->d_name[0] <= '9') {
      snprintf(full_path, full_path_length, "/proc/%s/comm", proc_ent->d_name);
      if (file_contains(full_path, "fluxbox")) {
        fluxbox_pid = atoi(proc_ent->d_name);
        if (fluxbox_pid > 0) {
          if (kill(fluxbox_pid, SIGUSR1) == 0) {
            fluxbox_was_killed = true;
          }
          break;
        }
      }
    }
  }
  closedir(proc_dir);
  if (!fluxbox_was_killed) {
    fputs("Could not restart fluxbox!\n", stderr);
  }
}

bool file_contains(const char *fname, const unsigned char *str)
{
  int i;
  FILE *f = fopen(fname, "r");
  if (!f) {
    return false;
  }
  int ch;
  /* Compare every character in str with the file. */
  for (i = 0; str[i] != 0; i++) {
    ch = fgetc(f);
    if (ch != (const int)str[i]) {
      fclose(f);
      return false;
    }
  }
  ch = fgetc(f);
  fclose(f);
  if (ch >= 32) {  /* Are there more printable characters in the file? */
    return false;
  }
  return true;
}
