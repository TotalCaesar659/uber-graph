#include "uber-pixring.h"
#include "uber-range.h"
#include "uber-renderer.h"
#include "uber-renderer-circle.h"

static UberRenderer *gRenderer;
static UberPixring  *gPixring;
static gint          gCount;
static UberRange    *gXRange;
static UberRange    *gYRange;
static GtkWidget    *gRaw;
static GtkWidget    *gRinged;
static GdkRGBA       gColors[5];

static gboolean
draw_chunk (gpointer widget)
{
   static guint cycle;
   cairo_matrix_t scale;
   GdkRectangle area = { 0 };
   cairo_t *cr;

   area.width = 600;
   area.height = 200;

   cr = uber_pixring_push(gPixring, 5);
   {
      gdouble y = g_random_double_range(0, 200);
      gdouble x = gCount;
      g_object_set(gRenderer,
                   "color", &gColors[cycle % G_N_ELEMENTS(gColors)],
                   "x", x,
                   "y", y,
                   NULL);
      cycle++;
   }

   uber_range_set_min(gXRange, gCount - 120);
   uber_range_set_max(gXRange, gCount);
   cairo_matrix_init_scale(&scale, 600.0, 200.0);
   cairo_transform(cr, &scale);
   uber_renderer_draw(gRenderer, cr, &area, gXRange, gYRange);
   cairo_destroy(cr);

   gtk_widget_queue_draw(gRaw);
   gtk_widget_queue_draw(gRinged);

   gCount += 5;

   return TRUE;
}

static gboolean
draw_raw (GtkWidget *widget,
          cairo_t   *cr,
          gpointer   user_data)
{
   cairo_surface_t *surface;

   cairo_save(cr);
   surface = uber_pixring_get_surface(gPixring);
   cairo_set_source_surface(cr, surface, 0, 0);
   cairo_rectangle(cr, 0, 0, 600, 200);
   cairo_fill(cr);
   cairo_restore(cr);

   return TRUE;
}

static gboolean
draw_ringed (GtkWidget *widget,
             cairo_t   *cr,
             gpointer   user_data)
{
   uber_pixring_draw(gPixring, cr);
   return TRUE;
}

static void
on_realize (GtkWidget *window,
            gpointer   user_data)
{
   cairo_surface_t *surface;

   surface =
      gdk_window_create_similar_surface(
         gtk_widget_get_window(window),
         CAIRO_CONTENT_COLOR_ALPHA,
         600,
         200);
   gPixring = uber_pixring_new(surface, 600, 200);
   gRenderer = g_object_new(UBER_TYPE_RENDERER_CIRCLE,
                            "radius", 1.0,
                            NULL);
   g_timeout_add(16, draw_chunk, NULL);
   cairo_surface_destroy(surface);
}

gint
main (gint   argc,
      gchar *argv[])
{
   GtkWindow *window;
   GtkContainer *vbox;

   gtk_init(&argc, &argv);
   gRaw = g_object_new(GTK_TYPE_DRAWING_AREA,
                       "visible", TRUE,
                       NULL);
   g_signal_connect(gRaw, "draw", G_CALLBACK(draw_raw), NULL);
   gRinged = g_object_new(GTK_TYPE_DRAWING_AREA,
                          "visible", TRUE,
                          NULL);
   g_signal_connect(gRinged, "draw", G_CALLBACK(draw_ringed), NULL);
   vbox = g_object_new(GTK_TYPE_VBOX,
                       "spacing", 6,
                       "visible", TRUE,
                       NULL);
   gtk_container_add(vbox, gRaw);
   gtk_container_add(vbox, gRinged);
   window = g_object_new(GTK_TYPE_WINDOW,
                         "child", vbox,
                         "height-request", 406,
                         "resizable", FALSE,
                         "title", "Pixring Test",
                         "width-request", 600,
                         NULL);
   g_signal_connect(window, "realize", G_CALLBACK(on_realize), NULL);
   g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
   gXRange = uber_range_new();
   uber_range_set_min(gXRange, 0);
   uber_range_set_max(gXRange, 600);
   gYRange = uber_range_new();
   uber_range_set_min(gYRange, 0);
   uber_range_set_max(gYRange, 200);
   gdk_rgba_parse(&gColors[0], "#babdb6");
   gdk_rgba_parse(&gColors[1], "#3465a4");
   gdk_rgba_parse(&gColors[2], "#4e9a06");
   gdk_rgba_parse(&gColors[3], "#f57900");
   gdk_rgba_parse(&gColors[4], "#a40000");
   gtk_window_present(window);
   gtk_main();
   uber_range_free(gXRange);
   uber_range_free(gYRange);
   return 0;
}
