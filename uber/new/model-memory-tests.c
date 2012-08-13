#ifdef G_DISABLE_ASSERT
#undef G_DISABLE_ASSERT
#endif

#include "uber-model-memory.h"

static void
test1 (void)
{
   UberModelMemory *memory;
   UberModelIter iter;
   UberModel *model;
   gint c;
   gint i;
   gint64 i64;
   gdouble d;

   model = uber_model_memory_new(3, G_TYPE_INT, G_TYPE_INT64, G_TYPE_DOUBLE);
   g_assert(UBER_IS_MODEL(model));

   memory = UBER_MODEL_MEMORY(model);
   g_assert(UBER_IS_MODEL_MEMORY(memory));

   g_assert_cmpint(uber_model_get_n_columns(model), ==, 3);
   g_assert_cmpint(uber_model_get_column_type(model, 0), ==, G_TYPE_INT);
   g_assert_cmpint(uber_model_get_column_type(model, 1), ==, G_TYPE_INT64);
   g_assert_cmpint(uber_model_get_column_type(model, 2), ==, G_TYPE_DOUBLE);

   for (i = 0; i < 1000; i++) {
      uber_model_memory_append(memory, &iter);
      uber_model_memory_set(memory, &iter,
                            0, i,
                            1, (guint64)i,
                            2, (gdouble)i,
                            -1);
   }

   c = 999;

   if (uber_model_get_iter_at_row(model, &iter, 0)) {
      do {
         uber_model_get(model, &iter,
                        0, &i,
                        1, &i64,
                        2, &d,
                        -1);
         g_assert_cmpint((gint)i, ==, c);
         g_assert_cmpint((gint)i64, ==, c);
         g_assert_cmpint((gint)d, ==, c);
         c--;
      } while (uber_model_iter_next(model, &iter));
   }

   g_object_unref(model);
}

gint
main (gint   argc,
      gchar *argv[])
{
   g_type_init();
   g_test_init(&argc, &argv, NULL);
   g_test_add_func("/UberModelMemory/wrap_around", test1);
   return g_test_run();
}
