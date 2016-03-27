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
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>

struct surface {
  cairo_surface_t *surf;
  cairo_t *cr;
  GtkWidget *image;
  Theme *theme;
};

void update_color(struct surface *surf);

void hue_changed(GtkAdjustment *adjustment, gpointer data);

void sat_changed(GtkAdjustment *adjustment, gpointer data);

void size_changed(GtkAdjustment *adjustment, gpointer data);

void apply_theme(GtkButton *button, gpointer data);

void restart_fluxbox();

bool file_contains(const char *fname, const char *str);

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

  Theme theme;
  load_theme(&theme, theme_file);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_window_set_title(GTK_WINDOW(window), "Make Fluxbox theme");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 20);

  struct surface surf;
  surf.theme = &theme;
  surf.surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 30, 30);
  surf.cr = cairo_create(surf.surf);
  surf.image = gtk_image_new_from_surface(surf.surf);

  GtkAdjustment *hue_adjustment = gtk_adjustment_new(theme.hue, 0, 360, 1,
                                                     0, 0);
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

  GtkAdjustment *sat_adjustment = gtk_adjustment_new(theme.saturation, 0, 100,
                                                     1, 0, 0);
  g_signal_connect(sat_adjustment, "value-changed", G_CALLBACK(sat_changed),
                   &surf);
  GtkWidget *sat_label = gtk_label_new("Saturation");
  GtkWidget *sat_scale = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,
                                       sat_adjustment);
  gtk_widget_set_hexpand(sat_scale, true);

  GtkAdjustment *spin_adjustment = gtk_adjustment_new(theme.size, 1, 100, 1,
                                                      0, 0);
  g_signal_connect(spin_adjustment, "value-changed", G_CALLBACK(size_changed),
                   &surf);
  GtkWidget *spin_label = gtk_label_new("Size");
  GtkWidget *spin_button = gtk_spin_button_new(spin_adjustment, 1, 0);

  GtkWidget *apply_btn = gtk_button_new_with_label("Apply");
  g_signal_connect(apply_btn, "clicked", G_CALLBACK(apply_theme), &theme);

  GtkWidget *cancel_btn = gtk_button_new_with_label("Cancel");
  g_signal_connect(cancel_btn, "clicked", G_CALLBACK(gtk_main_quit), NULL);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 4);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 4);
  gtk_grid_attach(GTK_GRID(grid), surf.image, 0, 0, 2, 1);
  gtk_grid_attach(GTK_GRID(grid), hue_label, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), hue_scale, 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), sat_label, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), sat_scale, 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), spin_label, 0, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), spin_button, 1, 3, 1, 1);

  GtkWidget *text_view = gtk_text_view_new();
  GtkTextBuffer *text_buffer
    = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
  if (theme.extra_lines) {
    gtk_text_buffer_set_text(text_buffer, theme.extra_lines, -1);
  }
  theme.gtk_buffer = text_buffer;
  GtkWidget *text_label
    = gtk_label_new("You can add your own style rules here.");
  gtk_widget_set_halign(text_label, GTK_ALIGN_START);
  GtkWidget *adv_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_box_pack_start(GTK_BOX(adv_box), text_label, false, false, 0);
  gtk_box_pack_start(GTK_BOX(adv_box), text_view, true, true, 0);

  GtkWidget *stack = gtk_stack_new();
  gtk_stack_add_titled(GTK_STACK(stack), grid, "basic", "Basic");
  gtk_stack_add_titled(GTK_STACK(stack), adv_box, "adv", "Advanced");
  GtkWidget *stack_switcher = gtk_stack_switcher_new();
  gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(stack_switcher),
                               GTK_STACK(stack));
  gtk_widget_set_halign(stack_switcher, GTK_ALIGN_CENTER);

  GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  gtk_box_pack_end(GTK_BOX(button_box), apply_btn, false, false, 0);
  gtk_box_pack_end(GTK_BOX(button_box), cancel_btn, false, false, 0);

  GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_box_pack_start(GTK_BOX(main_box), stack_switcher, false, false, 0);
  gtk_box_pack_start(GTK_BOX(main_box), stack, true, true, 0);
  gtk_box_pack_end(GTK_BOX(main_box), button_box, false, false, 0);

  gtk_container_add(GTK_CONTAINER(window), main_box);
  gtk_container_set_border_width(GTK_CONTAINER(window), 8);

  gtk_widget_show_all(window);
  update_color(&surf);
  gtk_main();

  return 0;
}

void update_color(struct surface *surf)
{
  float r, g, b;
  hsl_to_rgb(&r, &g, &b, surf->theme->hue, surf->theme->saturation, 20);
  cairo_rectangle(surf->cr, 1, 1, 28, 28);
  cairo_set_source_rgb(surf->cr, r, g, b);
  cairo_fill(surf->cr);
  gtk_widget_queue_draw(surf->image);
}

void hue_changed(GtkAdjustment *adjustment, gpointer data)
{
  struct surface *surf = (struct surface*)data;
  surf->theme->hue = gtk_adjustment_get_value(adjustment);
  update_color(surf);
}

void sat_changed(GtkAdjustment *adjustment, gpointer data)
{
  struct surface *surf = (struct surface*)data;
  surf->theme->saturation = gtk_adjustment_get_value(adjustment);
  update_color(surf);
}

void size_changed(GtkAdjustment *adjustment, gpointer data)
{
  struct surface *surf = (struct surface*)data;
  surf->theme->size = gtk_adjustment_get_value(adjustment);
}

void apply_theme(GtkButton *button, gpointer data)
{
  Theme *theme = (Theme*)data;

  GtkTextIter start_iter;
  GtkTextIter end_iter;
  gtk_text_buffer_get_start_iter(theme->gtk_buffer, &start_iter);
  gtk_text_buffer_get_end_iter(theme->gtk_buffer, &end_iter);
  theme->extra_lines = gtk_text_buffer_get_text(theme->gtk_buffer,
                                                &start_iter, &end_iter, false);

  write_theme_file(theme, theme->file_name);
  restart_fluxbox();
  if (theme->file_name[0] == '-' && theme->file_name[1] == '\0') {
    gtk_main_quit();
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

bool file_contains(const char *fname, const char *str)
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
    if (ch != (const int)(const unsigned char)str[i]) {
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
