/* uber-model-memory.c
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
#include <gobject/gvaluecollector.h>

#include "gring.h"
#include "uber-model-memory.h"

struct _UberModelMemoryPrivate
{
   GArray *columns;
   guint   n_rows;
   guint   next_offset;
};

typedef struct
{
   GType  type;
   GRing *values;
} Column;

static void uber_model_init (UberModelIface *iface);

G_DEFINE_TYPE_EXTENDED(UberModelMemory,
                       uber_model_memory,
                       G_TYPE_OBJECT,
                       0,
                       G_IMPLEMENT_INTERFACE(UBER_TYPE_MODEL,
                                             uber_model_init))

/**
 * uber_model_memory_new:
 * @n_columns: (in): How many column types will follow.
 * @first_type: (in): The first column type.
 *
 * Creates a new instance of #UberModelMemory.
 *
 * Returns: (transfer full): An #UberModelMemory.
 */
UberModel *
uber_model_memory_new (guint n_columns,
                       GType first_type,
                       ...)
{
   UberModelMemoryPrivate *priv;
   UberModelMemory *memory;
   va_list args;
   Column column;
   gsize size;
   GType type_id;

   g_return_val_if_fail(n_columns > 0, NULL);
   g_return_val_if_fail(first_type, NULL);

   priv = (memory = g_object_new(UBER_TYPE_MODEL_MEMORY, NULL))->priv;

   type_id = first_type;

   va_start(args, first_type);

   do {
      switch (type_id) {
      case G_TYPE_INT:
      case G_TYPE_UINT:
      case G_TYPE_FLOAT:
         size = 4;
         break;
      case G_TYPE_INT64:
      case G_TYPE_UINT64:
      case G_TYPE_DOUBLE:
         size = 8;
         break;
      default:
         g_warning("GType %s not supported as column.",
                   g_type_name(type_id));
         g_object_unref(memory);
         return NULL;
      }
      memset(&column, 0, sizeof column);
      column.type = type_id;
      column.values = g_ring_sized_new(size, priv->n_rows, NULL);
      g_array_append_val(memory->priv->columns, column);
   } while ((--n_columns > 0) && (type_id = va_arg(args, GType)));

   va_end(args);

   return UBER_MODEL(memory);
}

void
uber_model_memory_append (UberModelMemory *memory,
                          UberModelIter   *iter)
{
   UberModelMemoryPrivate *priv;
   const guint8 data[sizeof(gdouble)] = { 0 };
   Column *c;
   guint i;

   g_return_if_fail(UBER_IS_MODEL_MEMORY(memory));
   g_return_if_fail(iter);

   priv = memory->priv;

   memset(iter, 0, sizeof *iter);
   iter->user_data = GINT_TO_POINTER(priv->next_offset);
   priv->next_offset++;

   for (i = 0; i < priv->columns->len; i++) {
      c = &g_array_index(priv->columns, Column, i);
      g_ring_append_val(c->values, data);
   }
}

static Column *
uber_model_memory_get_column (UberModelMemory *memory,
                              guint            column)
{
   if (column >= memory->priv->columns->len) {
      g_warning("No such column: %u", column);
      return NULL;
   }
   return &g_array_index(memory->priv->columns, Column, column);
}

void
uber_model_memory_set_value (UberModelMemory *memory,
                             UberModelIter   *iter,
                             guint            column,
                             const GValue    *value)
{
   UberModelMemoryPrivate *priv;
   Column *c;
   guint offset;

   g_return_if_fail(UBER_IS_MODEL_MEMORY(memory));
   g_return_if_fail(iter);
   g_return_if_fail(value);

   priv = memory->priv;

   if (!(c = uber_model_memory_get_column(memory, column))) {
      g_warning("No such column: %u", column);
      return;
   }

   /*
    * Get the ring offset from iter.
    */
   offset = priv->next_offset - GPOINTER_TO_INT(iter->user_data) - 1;
   offset += GPOINTER_TO_INT(iter->user_data2);

   /*
    * Set the value in the array based on the column type.
    */
   switch (c->type) {
   case G_TYPE_DOUBLE:
      g_ring_index(c->values, gdouble, offset) = value->data[0].v_double;
      break;
   case G_TYPE_FLOAT:
      g_ring_index(c->values, gfloat, offset) = value->data[0].v_float;
      break;
   case G_TYPE_INT:
      g_ring_index(c->values, gint, offset) = value->data[0].v_int;
      break;
   case G_TYPE_INT64:
      g_ring_index(c->values, gint64, offset) = value->data[0].v_int64;
      break;
   case G_TYPE_UINT:
      g_ring_index(c->values, guint, offset) = value->data[0].v_uint;
      break;
   case G_TYPE_UINT64:
      g_ring_index(c->values, guint64, offset) = value->data[0].v_uint64;
      break;
   default:
      g_assert_not_reached();
   }
}

static void
uber_model_memory_set_valist (UberModelMemory *memory,
                              UberModelIter   *iter,
                              gint             first_column,
                              va_list          args)
{
   Column *c;
   GValue value = { 0 };
   gchar *errstr = NULL;
   gint column = first_column;

   g_assert(UBER_IS_MODEL_MEMORY(memory));
   g_assert(iter);
   g_assert(first_column >= 0);

   do {
      if (!(c = uber_model_memory_get_column(memory, column))) {
         g_warning("No such column: %d", column);
         return;
      }
      G_VALUE_COLLECT_INIT(&value, c->type, args, 0, &errstr);
      if (errstr) {
         g_warning("Failed to collect value: %s", errstr);
         g_free(errstr);
         return;
      }
      uber_model_memory_set_value(memory, iter, column, &value);
      g_value_unset(&value);
   } while (-1 != (column = va_arg(args, gint)));
}

void
uber_model_memory_set (UberModelMemory *memory,
                       UberModelIter   *iter,
                       gint             first_column,
                       ...)
{
   va_list args;

   g_return_if_fail(UBER_IS_MODEL_MEMORY(memory));
   g_return_if_fail(iter);
   g_return_if_fail(first_column >= 0);

   va_start(args, first_column);
   uber_model_memory_set_valist(memory, iter, first_column, args);
   va_end(args);
}

static GType
uber_model_memory_get_column_type (UberModel *model,
                                   guint      column)
{
   UberModelMemory *memory = (UberModelMemory *)model;
   Column *c;

   g_return_val_if_fail(UBER_IS_MODEL_MEMORY(memory), G_TYPE_INVALID);

   if ((c = uber_model_memory_get_column(memory, column))) {
      return c->type;
   }

   return G_TYPE_INVALID;
}

static guint
uber_model_memory_get_n_columns (UberModel *model)
{
   UberModelMemory *memory = (UberModelMemory *)model;
   g_return_val_if_fail(UBER_IS_MODEL_MEMORY(memory), 0);
   return memory->priv->columns->len;
}

static guint
uber_model_memory_get_n_rows (UberModel *model)
{
   UberModelMemory *memory = (UberModelMemory *)model;
   g_return_val_if_fail(UBER_IS_MODEL_MEMORY(memory), 0);
   return memory->priv->n_rows;
}

static void
uber_model_memory_get_value (UberModel     *model,
                             UberModelIter *iter,
                             guint          column,
                             GValue        *value)
{
   UberModelMemoryPrivate *priv;
   UberModelMemory *memory = (UberModelMemory *)model;
   Column *c;
   guint offset;

   g_return_if_fail(UBER_IS_MODEL_MEMORY(memory));
   g_return_if_fail(iter);
   g_return_if_fail(value);

   priv = memory->priv;

   /*
    * Get the column requested.
    */
   if (!(c = uber_model_memory_get_column(memory, column))) {
      g_warning("No such column: %u", column);
      return;
   }

   /*
    * Get the desired offset from the iter.
    */
   offset = priv->next_offset - GPOINTER_TO_INT(iter->user_data) - 1;
   offset += GPOINTER_TO_INT(iter->user_data2);

   /*
    * Initialize the GType to contain our value.
    */
   g_value_init(value, c->type);

   /*
    * Copy the value from the array to the output value.
    */
   switch (c->type) {
   case G_TYPE_DOUBLE:
      value->data[0].v_double = g_ring_index(c->values, gdouble, offset);
      break;
   case G_TYPE_FLOAT:
      value->data[0].v_float = g_ring_index(c->values, gfloat, offset);
      break;
   case G_TYPE_INT:
      value->data[0].v_int = g_ring_index(c->values, gint, offset);
      break;
   case G_TYPE_INT64:
      value->data[0].v_int64 = g_ring_index(c->values, gint64, offset);
      break;
   case G_TYPE_UINT:
      value->data[0].v_uint = g_ring_index(c->values, guint, offset);
      break;
   case G_TYPE_UINT64:
      value->data[0].v_uint64 = g_ring_index(c->values, guint64, offset);
      break;
   default:
      g_assert_not_reached();
   }
}

static gboolean
uber_model_memory_get_iter_at_row (UberModel     *model,
                                   UberModelIter *iter,
                                   guint          row)
{
   UberModelMemoryPrivate *priv;
   UberModelMemory *memory = (UberModelMemory *)model;

   g_return_val_if_fail(UBER_IS_MODEL_MEMORY(memory), FALSE);
   g_return_val_if_fail(iter, FALSE);

   priv = memory->priv;

   memset(iter, 0, sizeof *iter);
   iter->user_data = GINT_TO_POINTER(priv->next_offset) - 1;
   iter->user_data2 = GINT_TO_POINTER(row);
   return GPOINTER_TO_INT(iter->user_data2) < priv->n_rows;
}

static gboolean
uber_model_memory_iter_next (UberModel     *model,
                             UberModelIter *iter)
{
   UberModelMemoryPrivate *priv;
   UberModelMemory *memory = (UberModelMemory *)model;

   g_return_val_if_fail(UBER_IS_MODEL_MEMORY(memory), FALSE);
   g_return_val_if_fail(iter, FALSE);

   priv = memory->priv;
   iter->user_data2 = GINT_TO_POINTER(GPOINTER_TO_INT(iter->user_data2) + 1);
   return GPOINTER_TO_INT(iter->user_data2) < priv->n_rows;
}

static void
uber_model_memory_finalize (GObject *object)
{
   /*
    * TODO: Free resources.
    */
   G_OBJECT_CLASS(uber_model_memory_parent_class)->finalize(object);
}

static void
uber_model_memory_class_init (UberModelMemoryClass *klass)
{
   GObjectClass *object_class;

   object_class = G_OBJECT_CLASS(klass);
   object_class->finalize = uber_model_memory_finalize;
   g_type_class_add_private(object_class, sizeof(UberModelMemoryPrivate));
}

static void
uber_model_memory_init (UberModelMemory *memory)
{
   memory->priv =
      G_TYPE_INSTANCE_GET_PRIVATE(memory,
                                  UBER_TYPE_MODEL_MEMORY,
                                  UberModelMemoryPrivate);
   memory->priv->n_rows = 60;
   memory->priv->columns = g_array_new(FALSE, TRUE, sizeof(Column));
   memory->priv->next_offset = 0;
}

static void
uber_model_init (UberModelIface *iface)
{
   iface->get_column_type = uber_model_memory_get_column_type;
   iface->get_n_columns = uber_model_memory_get_n_columns;
   iface->get_n_rows = uber_model_memory_get_n_rows;
   iface->get_value = uber_model_memory_get_value;
   iface->get_iter_at_row = uber_model_memory_get_iter_at_row;
   iface->iter_next = uber_model_memory_iter_next;
}
