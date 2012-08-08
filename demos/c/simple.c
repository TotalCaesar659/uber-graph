/* main.c
 *
 * Copyright (C) 2010 Christian Hergert <chris@dronelabs.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include "uber.h"

static const gchar *default_colors[] = { "#73d216",
                                         "#f57900",
                                         "#3465a4",
                                         "#ef2929",
                                         "#75507b",
                                         "#ce5c00",
                                         "#c17d11",
                                         "#ce5c00",
                                         NULL };

#define CPU_MAX (100.0)
#define NLINES  (4)

static gboolean
dummy_heatmap_func (UberHeatMap  *map,   /* IN */
                    GArray      **array,     /* OUT */
                    gpointer      user_data) /* IN */
{
    gfloat maxval = *(gfloat *)user_data;
	gdouble val;
	gint i;

	*array = g_array_new(FALSE, FALSE, sizeof(gdouble));
	for (i = 0; i < 4; i++) {
		val = g_random_double_range(0., maxval);
		g_array_append_val(*array, val);
	}
	return TRUE;
}

static gboolean
dummy_line_func (UberLineGraph *graph,
                 guint          line,
                 gpointer       user_data)
{
    gfloat maxval = *(gfloat *)user_data;
	return g_random_double_range(0., maxval);
}


gint
main (gint   argc,   /* IN */
      gchar *argv[]) /* IN */
{
	UberRange line_many_range = { 0., CPU_MAX, CPU_MAX};
	GtkWidget *window;
	GtkWidget *line_many;
	GtkWidget *line;
	GtkWidget *map;
	GtkWidget *label;
	GtkAccelGroup *ag;
	GdkColor color;
	gint i;
	gint mod;

    const gfloat line_maxval = CPU_MAX;
    const gfloat scatter_maxval = CPU_MAX;

	gtk_init(&argc, &argv);

	/*
	 * Create window and graphs.
	 */
	window = uber_window_new();
	line_many = uber_line_graph_new();
	line = uber_line_graph_new();
	map = uber_heat_map_new();

	/*
	 * Multi Line Graph.
	 */
	uber_line_graph_set_autoscale(UBER_LINE_GRAPH(line_many), FALSE);
	uber_graph_set_format(UBER_GRAPH(line_many), UBER_GRAPH_FORMAT_PERCENT);
	uber_line_graph_set_range(UBER_LINE_GRAPH(line_many), &line_many_range);
	uber_line_graph_set_data_func(UBER_LINE_GRAPH(line_many),
	                              (UberLineGraphFunc)dummy_line_func, (gpointer *)&line_maxval, NULL);
	for (i = 0; i < NLINES; i++) {
		mod = i % G_N_ELEMENTS(default_colors);
		gdk_color_parse(default_colors[mod], &color);
		label = uber_label_new();
		uber_label_set_color(UBER_LABEL(label), &color);
		uber_line_graph_add_line(UBER_LINE_GRAPH(line_many), &color,
		                         UBER_LABEL(label));
	}

	uber_line_graph_set_autoscale(UBER_LINE_GRAPH(line), TRUE);
	label = uber_label_new();
	uber_label_set_text(UBER_LABEL(label), "Random");
	gdk_color_parse("#729fcf", &color);
	uber_line_graph_add_line(UBER_LINE_GRAPH(line), &color, UBER_LABEL(label));
	uber_line_graph_set_data_func(UBER_LINE_GRAPH(line),
	                              (UberLineGraphFunc)dummy_line_func, (gpointer *)&line_maxval, NULL);


	uber_graph_set_show_ylines(UBER_GRAPH(map), FALSE);
	gdk_color_parse(default_colors[0], &color);
	uber_heat_map_set_fg_color(UBER_HEAT_MAP(map), &color);
	uber_heat_map_set_data_func(UBER_HEAT_MAP(map),
	                            (UberHeatMapFunc)dummy_heatmap_func, (gpointer *)&scatter_maxval, NULL);
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(map), "Heat Map");
	uber_graph_set_show_xlabels(UBER_GRAPH(map), FALSE);
	gtk_widget_show(map);

	/*
	 * Add graphs.
	 */
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(line_many), "Random 0-100");
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(line),  "Autoscale");
	/*
	 * Disable X tick labels by default (except last).
	 */
	uber_graph_set_show_xlabels(UBER_GRAPH(line_many), FALSE);
	uber_graph_set_show_xlabels(UBER_GRAPH(line), TRUE);
	/*
	 * Show widgets.
	 */
	gtk_widget_show(line);
	gtk_widget_show(line_many);
	gtk_widget_show(window);
	/*
	 * Show line labels by default.
	 */
	uber_window_show_labels(UBER_WINDOW(window), UBER_GRAPH(line_many));
	/*
	 * Setup accelerators.
	 */
	ag = gtk_accel_group_new();
	gtk_accel_group_connect(ag, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_MASK,
	                        g_cclosure_new(gtk_main_quit, NULL, NULL));
	gtk_window_add_accel_group(GTK_WINDOW(window), ag);
	/*
	 * Attach signals.
	 */
	g_signal_connect(window,
	                 "delete-event",
	                 G_CALLBACK(gtk_main_quit),
	                 NULL);

	gtk_main();

	return EXIT_SUCCESS;
}
