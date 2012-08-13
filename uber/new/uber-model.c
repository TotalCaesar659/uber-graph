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
