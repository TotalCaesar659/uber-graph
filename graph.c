#include <glib/gi18n.h>

#include "uber-graph.h"
#include "uber-list-model.h"

gint
main (gint   argc,
      gchar *argv[])
{
   UberModel *model;
   GtkWidget *graph;
   GtkWindow *window;

   gtk_init(&argc, &argv);
   window = g_object_new(GTK_TYPE_WINDOW,
                         "border-width", 12,
                         "height-request", 200,
                         "resizable", FALSE,
                         "title", _("Uber Graph"),
                         "width-request", 600,
                         NULL);
   model = uber_list_model_new(1, G_TYPE_DOUBLE);
   graph = g_object_new(UBER_TYPE_GRAPH,
                        "model", model,
                        "visible", TRUE,
                        NULL);
   gtk_container_add(GTK_CONTAINER(window), graph);
   g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
   gtk_window_present(window);
   gtk_main();
   return 0;
}
