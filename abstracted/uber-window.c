/* uber-window.c
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

#include "uber-window.h"

/**
 * SECTION:uber-window.h
 * @title: UberWindow
 * @short_description: 
 *
 * Section overview.
 */

G_DEFINE_TYPE(UberWindow, uber_window, GTK_TYPE_WINDOW)

struct _UberWindowPrivate
{
	gint graph_count;

	GtkWidget *notebook;
	GtkWidget *table;
};

/**
 * uber_window_new:
 *
 * Creates a new instance of #UberWindow.
 *
 * Returns: the newly created instance of #UberWindow.
 * Side effects: None.
 */
GtkWidget*
uber_window_new (void)
{
	UberWindow *window;

	window = g_object_new(UBER_TYPE_WINDOW, NULL);
	return GTK_WIDGET(window);
}

/**
 * uber_window_add_graph:
 * @window: A #UberWindow.
 *
 * XXX
 *
 * Returns: None.
 * Side effects: None.
 */
void
uber_window_add_graph (UberWindow  *window, /* IN */
                       UberGraph   *graph,  /* IN */
                       const gchar *title)  /* IN */
{
	UberWindowPrivate *priv;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *labels;
	gchar *formatted;
	gint left_attach;
	gint top_attach;

	g_return_if_fail(UBER_IS_WINDOW(window));

	priv = window->priv;
	/*
	 * Format title string.
	 */
	formatted = g_markup_printf_escaped("<b>%s</b>", title);
	/*
	 * Create container for graph.
	 */
	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 3);
	label = gtk_label_new(NULL);
	labels = uber_graph_get_labels(graph);
	gtk_label_set_markup(GTK_LABEL(label), formatted);
	gtk_label_set_angle(GTK_LABEL(label), -270.);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(graph), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);
	if (labels) {
		gtk_box_pack_start(GTK_BOX(vbox), labels, FALSE, TRUE, 0);
	}
	gtk_widget_show(label);
	gtk_widget_show(hbox);
	gtk_widget_show(vbox);
	/*
	 * Append graph to table.
	 */
	left_attach = 0;
	top_attach = priv->graph_count; // % 4;
	gtk_table_attach(GTK_TABLE(priv->table), hbox,
	                 left_attach,
	                 left_attach + 1,
	                 top_attach,
	                 top_attach + 1,
	                 GTK_FILL | GTK_EXPAND,
	                 GTK_FILL | GTK_EXPAND,
	                 0, 0);
	/*
	 * Cleanup.
	 */
	g_free(formatted);
	priv->graph_count++;
}

/**
 * uber_window_finalize:
 * @object: A #UberWindow.
 *
 * Finalizer for a #UberWindow instance.  Frees any resources held by
 * the instance.
 *
 * Returns: None.
 * Side effects: None.
 */
static void
uber_window_finalize (GObject *object) /* IN */
{
	G_OBJECT_CLASS(uber_window_parent_class)->finalize(object);
}

/**
 * uber_window_class_init:
 * @klass: A #UberWindowClass.
 *
 * Initializes the #UberWindowClass and prepares the vtable.
 *
 * Returns: None.
 * Side effects: None.
 */
static void
uber_window_class_init (UberWindowClass *klass) /* IN */
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = uber_window_finalize;
	g_type_class_add_private(object_class, sizeof(UberWindowPrivate));
}

/**
 * uber_window_init:
 * @window: A #UberWindow.
 *
 * Initializes the newly created #UberWindow instance.
 *
 * Returns: None.
 * Side effects: None.
 */
static void
uber_window_init (UberWindow *window) /* IN */
{
	UberWindowPrivate *priv;

	window->priv = G_TYPE_INSTANCE_GET_PRIVATE(window,
	                                           UBER_TYPE_WINDOW,
	                                           UberWindowPrivate);

	/*
	 * Initialize defaults.
	 */
	priv = window->priv;
	gtk_window_set_title(GTK_WINDOW(window), "Uber Graph");
	gtk_window_set_default_size(GTK_WINDOW(window), 750, 550);
	gtk_container_set_border_width(GTK_CONTAINER(window), 12);
	/*
	 * Create notebook container for pages.
	 */
	priv->notebook = gtk_notebook_new();
	gtk_notebook_set_show_border(GTK_NOTEBOOK(priv->notebook), FALSE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(priv->notebook), FALSE);
	gtk_container_add(GTK_CONTAINER(window), priv->notebook);
	gtk_widget_show(priv->notebook);
	/*
	 * Create table for graphs.
	 */
	priv->table = gtk_table_new(1, 1, TRUE);
	gtk_notebook_append_page(GTK_NOTEBOOK(priv->notebook), priv->table, NULL);
	gtk_widget_show(priv->table);
}
