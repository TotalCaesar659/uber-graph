/* uber-model.h
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

#ifndef UBER_MODEL_H
#define UBER_MODEL_H

#include <glib-object.h>

G_BEGIN_DECLS

#define UBER_TYPE_MODEL             (uber_model_get_type())
#define UBER_MODEL(o)               (G_TYPE_CHECK_INSTANCE_CAST((o),    UBER_TYPE_MODEL, UberModel))
#define UBER_IS_MODEL(o)            (G_TYPE_CHECK_INSTANCE_TYPE((o),    UBER_TYPE_MODEL))
#define UBER_MODEL_GET_INTERFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE((o), UBER_TYPE_MODEL, UberModelIface))

typedef struct _UberModel      UberModel;
typedef struct _UberModelIface UberModelIface;
typedef struct _UberModelIter  UberModelIter;

struct _UberModelIter
{
   gpointer user_data;
   gpointer user_data2;
   gpointer user_data3;
   gpointer user_data4;
};

struct _UberModelIface
{
   GTypeInterface parent;

   /* interface methods */
   GType    (*get_column_type) (UberModel     *model,
                                guint          column);
   guint    (*get_n_columns)   (UberModel     *model);
   guint    (*get_n_rows)      (UberModel     *model);
   void     (*get_value)       (UberModel     *model,
                                UberModelIter *iter,
                                guint          column,
                                GValue        *value);
   gboolean (*get_iter_at_row) (UberModel     *model,
                                UberModelIter *iter,
                                guint          row);
   gboolean (*iter_next)       (UberModel     *model,
                                UberModelIter *iter);
};

GType    uber_model_get_column_type (UberModel     *model,
                                     guint          column);
guint    uber_model_get_n_columns   (UberModel     *model);
guint    uber_model_get_n_rows      (UberModel     *model);
GType    uber_model_get_type        (void) G_GNUC_CONST;
void     uber_model_get_value       (UberModel     *model,
                                     UberModelIter *iter,
                                     guint          column,
                                     GValue        *value);
gboolean uber_model_get_iter_at_row (UberModel     *model,
                                     UberModelIter *iter,
                                     guint          row);
gboolean uber_model_iter_next       (UberModel     *model,
                                     UberModelIter *iter);

G_END_DECLS

#endif /* UBER_MODEL_H */
