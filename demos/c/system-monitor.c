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

#include <sys/sysinfo.h>
#include <stdlib.h>

#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include "uber.h"
#include "blktrace.h"
#include "sysmon.h"

static gboolean     want_blktrace    = FALSE;
static const gchar *default_colors[] = { "#73d216",
                                         "#f57900",
                                         "#3465a4",
                                         "#ef2929",
                                         "#75507b",
                                         "#ce5c00",
                                         "#c17d11",
                                         "#ce5c00",
                                         NULL };

static void G_GNUC_NORETURN
sample_thread (gpointer data)
{
	while (TRUE) {
		g_usleep(G_USEC_PER_SEC);
		smon_next_cpu_info();
		smon_next_cpu_freq_info();
		smon_next_net_info();
		if (want_blktrace) {
			uber_blktrace_next();
		}
	}
}

static gboolean
dummy_scatter_func (UberScatter  *scatter,   /* IN */
                    GArray      **array,     /* OUT */
                    gpointer      user_data) /* IN */
{
	gdouble val;
	gint i;

	*array = g_array_new(FALSE, FALSE, sizeof(gdouble));
	for (i = 0; i < 4; i++) {
		val = g_random_double_range(0., 100.);
		g_array_append_val(*array, val);
	}
	return TRUE;
}

gint
main (gint   argc,   /* IN */
      gchar *argv[]) /* IN */
{
	gdouble dashes[] = { 1.0, 4.0 };
	UberRange cpu_range = { 0., 100., 100. };
	UberRange net_range = { 0., 512., 512. };
	UberRange ui_range = { 0., 10., 10. };
	GtkWidget *window;
	GtkWidget *cpu;
	GtkWidget *net;
	GtkWidget *line;
	GtkWidget *map;
	GtkWidget *scatter;
	GtkWidget *label;
	GtkAccelGroup *ag;
	GdkRGBA color;
	gint lineno;
	gint nprocs;
	gint i;
	gint mod;

	gtk_init(&argc, &argv);
	nprocs = get_nprocs();
	/*
	 * Check for blktrace hack.
	 */
	if (argc > 1 && (g_strcmp0(argv[1], "--i-can-haz-blktrace") == 0)) {
		want_blktrace = TRUE;
	}
	/*
	 * Warm up differential samplers.
	 */
	smon_next_cpu_info();
	smon_next_cpu_freq_info();
	if (want_blktrace) {
		uber_blktrace_init();
	}
	/*
	 * Install event hook to track how many X events we are doing.
	 */
	gdk_event_handler_set(smon_gdk_event_hook, NULL, NULL);
	/*
	 * Create window and graphs.
	 */
	window = uber_window_new();
	cpu = uber_line_graph_new();
	net = uber_line_graph_new();
	line = uber_line_graph_new();
	map = uber_heat_map_new();
	scatter = uber_scatter_new();
	/*
	 * Configure CPU graph.
	 */
	uber_line_graph_set_autoscale(UBER_LINE_GRAPH(cpu), FALSE);
	uber_graph_set_format(UBER_GRAPH(cpu), UBER_GRAPH_FORMAT_PERCENT);
	uber_line_graph_set_range(UBER_LINE_GRAPH(cpu), &cpu_range);
	uber_line_graph_set_data_func(UBER_LINE_GRAPH(cpu),
	                              smon_get_cpu_info, NULL, NULL);
	for (i = 0; i < nprocs; i++) {
		mod = i % G_N_ELEMENTS(default_colors);
		gdk_rgba_parse(&color, default_colors[mod]);
		label = uber_label_new();
		uber_label_set_color(UBER_LABEL(label), &color);
		uber_line_graph_add_line(UBER_LINE_GRAPH(cpu), &color,
		                         UBER_LABEL(label));
		cpu_info.labels[i] = label;
		/*
		 * XXX: Add the line regardless. Just dont populate it if we don't
		 *      have data.
		 */
		lineno = uber_line_graph_add_line(UBER_LINE_GRAPH(cpu), &color, NULL);
		if (smon_cpu_has_freq_scaling(i)) {
			uber_line_graph_set_line_dash(UBER_LINE_GRAPH(cpu), lineno,
			                              dashes, G_N_ELEMENTS(dashes), 0);
		}
	}
	/*
	 * Add lines for GDK/X events.
	 */
	uber_line_graph_set_range(UBER_LINE_GRAPH(line), &ui_range);
	label = uber_label_new();
	uber_label_set_text(UBER_LABEL(label), "GDK Events");
	gdk_rgba_parse(&color, "#729fcf");
	uber_line_graph_add_line(UBER_LINE_GRAPH(line), &color, UBER_LABEL(label));
	label = uber_label_new();
	uber_label_set_text(UBER_LABEL(label), "X Events");
	gdk_rgba_parse(&color, "#a40000");
	uber_line_graph_add_line(UBER_LINE_GRAPH(line), &color, UBER_LABEL(label));
	uber_line_graph_set_data_func(UBER_LINE_GRAPH(line),
	                              smon_get_xevent_info, NULL, NULL);
	/*
	 * Add lines for bytes in/out.
	 */
	uber_line_graph_set_range(UBER_LINE_GRAPH(net), &net_range);
	uber_line_graph_set_data_func(UBER_LINE_GRAPH(net),
	                              smon_get_net_info, NULL, NULL);
	uber_graph_set_format(UBER_GRAPH(net), UBER_GRAPH_FORMAT_DIRECT1024);
	label = uber_label_new();
	uber_label_set_text(UBER_LABEL(label), "Bytes In");
	gdk_rgba_parse(&color, "#a40000");
	uber_line_graph_add_line(UBER_LINE_GRAPH(net), &color, UBER_LABEL(label));
	label = uber_label_new();
	uber_label_set_text(UBER_LABEL(label), "Bytes Out");
	gdk_rgba_parse(&color, "#4e9a06");
	uber_line_graph_add_line(UBER_LINE_GRAPH(net), &color, UBER_LABEL(label));

	/*
	 * Configure heat map.
	 */
	uber_graph_set_show_ylines(UBER_GRAPH(map), FALSE);
	gdk_rgba_parse(&color, default_colors[0]);
	uber_heat_map_set_fg_color(UBER_HEAT_MAP(map), &color);
	uber_heat_map_set_data_func(UBER_HEAT_MAP(map),
	                            (UberHeatMapFunc)dummy_scatter_func, NULL, NULL);
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(map), "IO Latency");
	uber_graph_set_show_xlabels(UBER_GRAPH(map), FALSE);
	gtk_widget_show(map);

	/*
	 * Configure scatter.
	 */
	if (want_blktrace) {
		uber_graph_set_show_ylines(UBER_GRAPH(scatter), FALSE);
		gdk_rgba_parse(&color, default_colors[3]);
		uber_scatter_set_fg_color(UBER_SCATTER(scatter), &color);
		uber_scatter_set_data_func(UBER_SCATTER(scatter),
								   (UberScatterFunc)uber_blktrace_get, NULL, NULL);
		uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(scatter), "IOPS By Size");
		uber_graph_set_show_xlabels(UBER_GRAPH(scatter), TRUE);
		gtk_widget_show(scatter);
	}
	/*
	 * Add graphs.
	 */
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(cpu), "CPU");
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(net), "Network");
	uber_window_add_graph(UBER_WINDOW(window), UBER_GRAPH(line), "UI Events");
	/*
	 * Disable X tick labels by default (except last).
	 */
	uber_graph_set_show_xlabels(UBER_GRAPH(cpu), FALSE);
	uber_graph_set_show_xlabels(UBER_GRAPH(net), FALSE);
	uber_graph_set_show_xlabels(UBER_GRAPH(line), FALSE);
	/*
	 * Show widgets.
	 */
	gtk_widget_show(net);
	gtk_widget_show(line);
	gtk_widget_show(cpu);
	gtk_widget_show(window);
	/*
	 * Show cpu labels by default.
	 */
	uber_window_show_labels(UBER_WINDOW(window), UBER_GRAPH(cpu));
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
	/*
	 * Start sampling thread.
	 */
	g_thread_new("sample", (GThreadFunc)sample_thread, NULL);
	gtk_main();
	/*
	 * Cleanup after blktrace.
	 */
	if (want_blktrace) {
		uber_blktrace_shutdown();
	}
	return EXIT_SUCCESS;
}
