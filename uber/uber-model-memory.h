/* uber-model-memory.h
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

#ifndef UBER_MODEL_MEMORY_H
#define UBER_MODEL_MEMORY_H

#include "uber-model.h"

G_BEGIN_DECLS

#define UBER_TYPE_MODEL_MEMORY            (uber_model_memory_get_type())
#define UBER_MODEL_MEMORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_MODEL_MEMORY, UberModelMemory))
#define UBER_MODEL_MEMORY_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_MODEL_MEMORY, UberModelMemory const))
#define UBER_MODEL_MEMORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UBER_TYPE_MODEL_MEMORY, UberModelMemoryClass))
#define UBER_IS_MODEL_MEMORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBER_TYPE_MODEL_MEMORY))
#define UBER_IS_MODEL_MEMORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UBER_TYPE_MODEL_MEMORY))
#define UBER_MODEL_MEMORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UBER_TYPE_MODEL_MEMORY, UberModelMemoryClass))

typedef struct _UberModelMemory        UberModelMemory;
typedef struct _UberModelMemoryClass   UberModelMemoryClass;
typedef struct _UberModelMemoryPrivate UberModelMemoryPrivate;

struct _UberModelMemory
{
   GObject parent;

   /*< private >*/
   UberModelMemoryPrivate *priv;
};

struct _UberModelMemoryClass
{
   GObjectClass parent_class;
};

GType      uber_model_memory_get_type  (void) G_GNUC_CONST;
UberModel *uber_model_memory_new       (guint n_columns,
                                        GType first_type,
                                        ...);
void       uber_model_memory_append    (UberModelMemory *memory,
                                        UberModelIter   *iter);
void       uber_model_memory_set       (UberModelMemory *memory,
                                        UberModelIter   *iter,
                                        gint             first_column,
                                        ...);
void       uber_model_memory_set_value (UberModelMemory *memory,
                                        UberModelIter   *iter,
                                        guint            column,
                                        const GValue    *value);

G_END_DECLS

#endif /* UBER_MODEL_MEMORY_H */
