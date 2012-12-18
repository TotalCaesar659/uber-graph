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

#include "uber-model.h"
#include "uber-renderer.h"

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

struct _UberGraph
{
   GtkDrawingArea parent;

   /*< private >*/
   UberGraphPrivate *priv;
};

struct _UberGraphClass
{
   GtkDrawingAreaClass parent_class;
};

void       uber_graph_add_renderer    (UberGraph    *graph,
                                       UberRenderer *renderer);
GType      uber_graph_get_type        (void) G_GNUC_CONST;
GtkWidget *uber_graph_new             (void);
void       uber_graph_remove_renderer (UberGraph    *graph,
                                       UberRenderer *renderer);
void       uber_graph_set_attribute   (UberGraph    *graph,
                                       UberRenderer *renderer,
                                       const gchar  *attribute,
                                       guint         column);
void       uber_graph_set_model       (UberGraph    *graph,
                                       UberModel    *model);

G_END_DECLS

#endif /* UBER_GRAPH_H */
