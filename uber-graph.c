/* uber-graph.c
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

#include <glib/gi18n.h>

#include "uber-graph.h"

G_DEFINE_TYPE(UberGraph, uber_graph, GTK_TYPE_DRAWING_AREA)

struct _UberGraphPrivate
{
   GHashTable *attributes;
   UberModel  *model;
   GSList     *renderers;
};

typedef struct
{
   GParamSpec *pspec;
   guint       column;
} Attribute;

enum
{
   PROP_0,
   PROP_MODEL,
   LAST_PROP
};

static GParamSpec *gParamSpecs[LAST_PROP];

static void
attributes_free (gpointer data)
{
   GSList *iter;

   for (iter = data; iter; iter = iter->next) {
      g_param_spec_unref(((Attribute *)iter->data)->pspec);
      g_slice_free(Attribute, iter->data);
   }

   g_slist_free(data);
}

void
uber_graph_add_attribute (UberGraph    *graph,
                          UberRenderer *renderer,
                          const gchar  *property,
                          guint         column)
{
   UberGraphPrivate *priv;
   GObjectClass *klass;
   GParamSpec *pspec;
   Attribute *attr;
   GSList *attrs;

   g_return_if_fail(UBER_IS_GRAPH(graph));
   g_return_if_fail(UBER_IS_RENDERER(renderer));
   g_return_if_fail(property);

   priv = graph->priv;

   klass = G_OBJECT_GET_CLASS(renderer);
   if (!(pspec = g_object_class_find_property(klass, property))) {
      g_warning("No such property: %s", property);
      return;
   }

   attrs = g_hash_table_lookup(priv->attributes, renderer);
   attr = g_slice_new(Attribute);
   attr->pspec = g_param_spec_ref(pspec);
   attr->column = column;
   attrs = g_slist_prepend(attrs, attr);

   g_hash_table_steal(priv->attributes, renderer);
   g_hash_table_insert(priv->attributes, renderer, attrs);
}

void
uber_graph_add_renderer (UberGraph    *graph,
                         UberRenderer *renderer)
{
   g_return_if_fail(UBER_IS_GRAPH(graph));
   g_return_if_fail(UBER_IS_RENDERER(renderer));

   graph->priv->renderers = g_slist_append(graph->priv->renderers,
                                           g_object_ref_sink(renderer));
   /*
    * TODO: Invalidate the entire back buffer, for now just do this by
    *       causing a resize.
    */
   gtk_widget_queue_resize(GTK_WIDGET(graph));
}

void
uber_graph_remove_renderer (UberGraph    *graph,
                            UberRenderer *renderer)
{
   UberGraphPrivate *priv;
   GSList *iter;

   g_return_if_fail(UBER_IS_GRAPH(graph));
   g_return_if_fail(UBER_IS_RENDERER(renderer));

   priv = graph->priv;

   for (iter = priv->renderers; iter; iter = iter->next) {
      if (iter->data == renderer) {
         priv->renderers = g_slist_remove_link(priv->renderers, iter);
         /*
          * TODO: Like "add_renderer", invalidate widget.
          */
         gtk_widget_queue_resize(GTK_WIDGET(graph));
      }
   }
}

UberModel *
uber_graph_get_model (UberGraph *graph)
{
   g_return_val_if_fail(UBER_IS_GRAPH(graph), NULL);
   return graph->priv->model;
}

void
uber_graph_set_model (UberGraph *graph,
                      UberModel *model)
{
   UberGraphPrivate *priv;

   g_return_if_fail(UBER_IS_GRAPH(graph));
   g_return_if_fail(!model || UBER_IS_MODEL(model));

   priv = graph->priv;

   g_clear_object(&priv->model);

   if (model) {
      priv->model = g_object_ref(model);
   }

   gtk_widget_queue_resize(GTK_WIDGET(graph));
}

static void
uber_graph_dispose (GObject *object)
{
   UberGraphPrivate *priv = UBER_GRAPH(object)->priv;
   GSList *list;

   if (priv->attributes) {
      g_hash_table_destroy(priv->attributes);
      priv->attributes = NULL;
   }

   if ((list = priv->renderers)) {
      priv->renderers = NULL;
      g_slist_foreach(list, (GFunc)g_object_unref, NULL);
      g_slist_free(list);
   }

   uber_graph_set_model(UBER_GRAPH(object), NULL);

   G_OBJECT_CLASS(uber_graph_parent_class)->dispose(object);
}

static void
uber_graph_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
   UberGraph *graph = UBER_GRAPH(object);

   switch (prop_id) {
   case PROP_MODEL:
      g_value_set_object(value, uber_graph_get_model(graph));
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}

static void
uber_graph_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
   UberGraph *graph = UBER_GRAPH(object);

   switch (prop_id) {
   case PROP_MODEL:
      uber_graph_set_model(graph, g_value_get_object(value));
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}

static void
uber_graph_class_init (UberGraphClass *klass)
{
   GObjectClass *object_class;

   object_class = G_OBJECT_CLASS(klass);
   object_class->dispose = uber_graph_dispose;
   object_class->get_property = uber_graph_get_property;
   object_class->set_property = uber_graph_set_property;
   g_type_class_add_private(object_class, sizeof(UberGraphPrivate));

   gParamSpecs[PROP_MODEL] =
      g_param_spec_object("model",
                          _("Model"),
                          _("The model for the graph."),
                          UBER_TYPE_MODEL,
                          G_PARAM_READWRITE);
   g_object_class_install_property(object_class, PROP_MODEL,
                                   gParamSpecs[PROP_MODEL]);
}

static void
uber_graph_init (UberGraph *graph)
{
   graph->priv =
      G_TYPE_INSTANCE_GET_PRIVATE(graph,
                                  UBER_TYPE_GRAPH,
                                  UberGraphPrivate);
   graph->priv->attributes =
      g_hash_table_new_full(g_direct_hash, g_direct_equal,
                            NULL, attributes_free);
}
