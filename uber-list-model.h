/* uber-list-model.h
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

#ifndef UBER_LIST_MODEL_H
#define UBER_LIST_MODEL_H

#include "uber-model.h"

G_BEGIN_DECLS

#define UBER_TYPE_LIST_MODEL            (uber_list_model_get_type())
#define UBER_LIST_MODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_LIST_MODEL, UberListModel))
#define UBER_LIST_MODEL_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_LIST_MODEL, UberListModel const))
#define UBER_LIST_MODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UBER_TYPE_LIST_MODEL, UberListModelClass))
#define UBER_IS_LIST_MODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBER_TYPE_LIST_MODEL))
#define UBER_IS_LIST_MODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UBER_TYPE_LIST_MODEL))
#define UBER_LIST_MODEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UBER_TYPE_LIST_MODEL, UberListModelClass))

typedef struct _UberListModel        UberListModel;
typedef struct _UberListModelClass   UberListModelClass;
typedef struct _UberListModelPrivate UberListModelPrivate;

struct _UberListModel
{
   GObject parent;

   /*< private >*/
   UberListModelPrivate *priv;
};

struct _UberListModelClass
{
   GObjectClass parent_class;
};

GType      uber_list_model_get_type (void) G_GNUC_CONST;
UberModel *uber_list_model_new      (guint n_columns,
                                     GType first_column,
                                     ...);

G_END_DECLS

#endif /* UBER_LIST_MODEL_H */
