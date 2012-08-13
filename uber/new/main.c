#include "uber-model-memory.h"

gint
main (gint argc,
      gchar *argv[])
{
   UberModelMemory *memory;
   UberModelIter iter;
   GValue value = { 0 };
   gdouble d;
   guint i;

   g_type_init();

   memory = UBER_MODEL_MEMORY(uber_model_memory_new(2, G_TYPE_INT, G_TYPE_DOUBLE));
   for (i = 0; i < 120; i++) {
      uber_model_memory_append(memory, &iter);

      g_value_init(&value, G_TYPE_INT);
      g_value_set_int(&value, i);
      uber_model_memory_set_value(memory, &iter, 0, &value);
      g_value_unset(&value);

      g_value_init(&value, G_TYPE_DOUBLE);
      g_value_set_double(&value, i * 100.0);
      uber_model_memory_set_value(memory, &iter, 1, &value);
      g_value_unset(&value);
   }

   if (uber_model_get_iter_at_row(UBER_MODEL(memory), &iter, 0)) {
      do {
         uber_model_get_value(UBER_MODEL(memory), &iter, 0, &value);
         i = g_value_get_int(&value);
         g_value_unset(&value);

         uber_model_get_value(UBER_MODEL(memory), &iter, 1, &value);
         d = g_value_get_double(&value);
         g_value_unset(&value);

         g_print("%03d: %f\n", i, d);
      } while (uber_model_iter_next(UBER_MODEL(memory), &iter));
   }

   return 0;
}
