/* uber-model.c
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

#include <glib-object.h>
#include <gobject/gvaluecollector.h>

#include "uber-model.h"

guint
uber_model_get_n_columns (UberModel *model)
{
   return UBER_MODEL_GET_INTERFACE(model)->get_n_columns(model);
}

guint
uber_model_get_n_rows (UberModel *model)
{
   return UBER_MODEL_GET_INTERFACE(model)->get_n_rows(model);
}

GType
uber_model_get_column_type (UberModel *model,
                            guint      column)
{
   return UBER_MODEL_GET_INTERFACE(model)->get_column_type(model, column);
}

gdouble
uber_model_get_double (UberModel     *model,
                       UberModelIter *iter,
                       guint          column)
{
   GValue value = { 0 };
   GValue dvalue = { 0 };
   gdouble ret;

   g_return_val_if_fail(UBER_IS_MODEL(model), 0.0);
   g_return_val_if_fail(iter, 0.0);

   uber_model_get_value(model, iter, column, &value);
   if (G_VALUE_HOLDS(&value, G_TYPE_DOUBLE)) {
      ret = g_value_get_double(&value);
      g_value_unset(&value);
      return ret;
   }

   g_value_init(&dvalue, G_TYPE_DOUBLE);
   g_value_transform(&value, &dvalue);
   ret = g_value_get_double(&dvalue);
   g_value_unset(&value);
   g_value_unset(&dvalue);

   return ret;
}

void
uber_model_get_value (UberModel     *model,
                      UberModelIter *iter,
                      guint          column,
                      GValue        *value)
{
   return UBER_MODEL_GET_INTERFACE(model)->get_value(model, iter, column, value);
}

gboolean
uber_model_get_iter_at_row (UberModel     *model,
                            UberModelIter *iter,
                            guint          row)
{
   return UBER_MODEL_GET_INTERFACE(model)->get_iter_at_row(model, iter, row);
}

gboolean
uber_model_iter_next (UberModel     *model,
                      UberModelIter *iter)
{
   return UBER_MODEL_GET_INTERFACE(model)->iter_next(model, iter);
}

static void
uber_model_get_valist (UberModel     *model,
                       UberModelIter *iter,
                       gint           first_column,
                       va_list        args)
{
   GValue value = { 0 };
   gchar *errstr = NULL;
   gint column = first_column;

   g_assert(UBER_IS_MODEL(model));
   g_assert(iter);
   g_assert(first_column >= 0);

   do {
      uber_model_get_value(model, iter, column, &value);
      G_VALUE_LCOPY(&value, args, 0, &errstr);
      if (errstr) {
         g_warning("Failed to copy value: %s", errstr);
         g_free(errstr);
         return;
      }
      g_value_unset(&value);
   } while (-1 != (column = va_arg(args, gint)));
}

void
uber_model_get (UberModel     *model,
                UberModelIter *iter,
                gint           first_column,
                ...)
{
   va_list args;

   g_return_if_fail(UBER_IS_MODEL(model));
   g_return_if_fail(iter);
   g_return_if_fail(first_column >= 0);

   va_start(args, first_column);
   uber_model_get_valist(model, iter, first_column, args);
   va_end(args);
}

GType
uber_model_get_type (void)
{
   static gsize initialized;
   static GType type_id;
   const GTypeInfo type_info = {
      sizeof(UberModelIface),
      NULL, /* base_init */
      NULL, /* base_finalize */
      NULL, /* class_init */
      NULL, /* class_finalize */
      NULL, /* class_data */
      0,    /* instance_size */
      0,    /* n_preallocs */
      NULL, /* instance_init */
      NULL  /* value_vtable */
   };

   if (g_once_init_enter(&initialized)) {
      type_id = g_type_register_static(G_TYPE_INTERFACE,
                                       "UberModel",
                                       &type_info,
                                       0);
      g_type_interface_add_prerequisite(type_id, G_TYPE_OBJECT);
      g_once_init_leave(&initialized, TRUE);
   }

   return type_id;
}
