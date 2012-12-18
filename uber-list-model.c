/* uber-list-model.c
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

#include "uber-list-model.h"
#include "uber-model.h"

struct _UberListModelPrivate
{
   guint  n_columns;
   GType *column_types;
};

enum
{
   PROP_0,
   LAST_PROP
};

static void uber_model_init (UberModelIface *iface);

G_DEFINE_TYPE_EXTENDED(UberListModel, uber_list_model, G_TYPE_OBJECT, 0,
                       G_IMPLEMENT_INTERFACE(UBER_TYPE_MODEL,
                                             uber_model_init))

//static GParamSpec *gParamSpecs[LAST_PROP];

static UberModel *
uber_list_model_newv (guint   n_columns,
                      GType   first_column,
                      va_list args)
{
   UberListModel *model;
   GType column = first_column;
   guint i = 0;

   g_return_val_if_fail(n_columns, NULL);
   g_return_val_if_fail(first_column, NULL);

   model = g_object_new(UBER_TYPE_LIST_MODEL, NULL);
   model->priv->n_columns = n_columns;
   model->priv->column_types = g_malloc0_n(n_columns, sizeof(GType));

   while (TRUE) {
      model->priv->column_types[i++] = column;
      if (i < n_columns) {
         column = va_arg(args, GType);
         continue;
      }
      break;
   }

   return UBER_MODEL(model);
}

UberModel *
uber_list_model_new (guint n_columns,
                     GType first_column,
                     ...)
{
   UberModel *model;
   va_list args;

   g_return_val_if_fail(n_columns, NULL);
   g_return_val_if_fail(first_column, NULL);

   va_start(args, first_column);
   model = uber_list_model_newv(n_columns, first_column, args);
   va_end(args);

   return model;
}

static void
uber_list_model_finalize (GObject *object)
{
   G_OBJECT_CLASS(uber_list_model_parent_class)->finalize(object);
}

static void
uber_list_model_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
   //UberListModel *model = UBER_LIST_MODEL(object);

   switch (prop_id) {
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}

static void
uber_list_model_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
   //UberListModel *model = UBER_LIST_MODEL(object);

   switch (prop_id) {
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}

static void
uber_list_model_class_init (UberListModelClass *klass)
{
   GObjectClass *object_class;

   object_class = G_OBJECT_CLASS(klass);
   object_class->finalize = uber_list_model_finalize;
   object_class->get_property = uber_list_model_get_property;
   object_class->set_property = uber_list_model_set_property;
   g_type_class_add_private(object_class, sizeof(UberListModelPrivate));
}

static void
uber_list_model_init (UberListModel *model)
{
   model->priv =
      G_TYPE_INSTANCE_GET_PRIVATE(model,
                                  UBER_TYPE_LIST_MODEL,
                                  UberListModelPrivate);
}

static void
uber_model_init (UberModelIface *iface)
{
}
