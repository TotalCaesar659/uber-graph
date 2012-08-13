/* uber-graph.h
 *
 * Copyright (C) 2012 Christian Hergert <chris@dronelabs.com>
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UBER_GRAPH_H
#define UBER_GRAPH_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define UBER_TYPE_GRAPH            (uber_graph_get_type())
#define UBER_GRAPH(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_GRAPH, UberGraph))
#define UBER_GRAPH_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_GRAPH, UberGraph const))
#define UBER_GRAPH_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UBER_TYPE_GRAPH, UberGraphClass))
#define UBER_IS_GRAPH(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBER_TYPE_GRAPH))
#define UBER_IS_GRAPH_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UBER_TYPE_GRAPH))
#define UBER_GRAPH_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UBER_TYPE_GRAPH, UberGraphClass))

typedef struct _UberGraph        UberGraph;
typedef struct _UberGraphClass   UberGraphClass;
typedef struct _UberGraphPrivate UberGraphPrivate;
typedef enum   _UberGraphFlags   UberGraphFlags;

enum _UberGraphFlags
{
	UBER_GRAPH_NONE    = 0,
	UBER_GRAPH_PERCENT = 1 << 0,
};

struct _UberGraph
{
	GtkDrawingArea parent;

	/*< private >*/
	UberGraphPrivate *priv;
};

struct _UberGraphClass
{
	GtkDrawingAreaClass parent_class;

	gchar* (*format_value) (UberGraph *graph,
	                        gdouble     value);
};

GType uber_graph_get_type  (void) G_GNUC_CONST;
void  uber_graph_start     (UberGraph      *graph);
void  uber_graph_stop      (UberGraph      *graph);
void  uber_graph_set_flags (UberGraph      *graph,
                            UberGraphFlags  flags);

G_END_DECLS

#endif /* UBER_GRAPH_H */
