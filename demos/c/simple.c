#include <gtk/gtk.h>
#include <uber/uber.h>

gint
main (gint   argc,
      gchar *argv[])
{
   GtkWindow *window;
   GtkWidget *graph;
   UberRenderer *renderer;
   UberModel *model;

   gtk_init(&argc, &argv);

   model = uber_model_memory_new(1, G_TYPE_DOUBLE);
   renderer = g_object_new(UBER_TYPE_RENDERER_LINE, NULL);
   uber_renderer_line_append(UBER_RENDERER_LINE(renderer), model, 0);
   graph = g_object_new(UBER_TYPE_GRAPH,
                        "renderer", renderer,
                        "visible", TRUE,
                        NULL);
   window = g_object_new(GTK_TYPE_WINDOW,
                         "child", graph,
                         "title", "Random Graph",
                         NULL);
   gtk_window_set_default_size(window, 640, 320);
   g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
   gtk_window_present(window);
   gtk_main();
   return 0;
}
