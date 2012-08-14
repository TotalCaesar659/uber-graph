/* uber-graph.c
 *
 * Copyright (C) 2010 Christian Hergert <chris@dronelabs.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include <time.h>

#include "uber-debug.h"
#include "uber-frame-source.h"
#include "uber-graph.h"
#include "uber-renderer.h"


#define FRAMES_PER_SECOND      30
#define LABEL_XPAD             3
#define LABEL_YPAD             3


G_DEFINE_TYPE(UberGraph, uber_graph, GTK_TYPE_DRAWING_AREA)


struct _UberGraphPrivate
{
   UberGraphFlags flags;           /* Various flags */
   cairo_surface_t *foreground;     /* forground surface for gdk window */
   cairo_surface_t *background;     /* background surface for gdk window */
   PangoFontDescription *font_desc; /* Font description for labels */
   UberRenderer *renderer;           /* Renderer used for async rendering */
   GdkRectangle content_area;       /* Visible data area of widget */
   GdkRectangle data_area;          /* Data and buffer area of foreground */
   guint frame_handler;             /* Frame renderering handler */
   guint invalidate_handler;        /* Renderer invalidate handler */
   guint adjustment_handler;        /* Renderer adjustment range handler */
   gdouble n_seconds;               /* Number of seconds to display */
   gdouble n_buffered;              /* Number of seconds to buffer */
   gdouble offset_time;             /* Ring buffer offset in seconds */
   gdouble begin_time;              /* Beginning time of stored range */
   gdouble end_time;                /* End time of stored range */
   gdouble lower_value;             /* Lower value visualized */
   gdouble upper_value;             /* Upper value visualized */
   guint32 frame_count;             /* Debugging frame counter */
   gint label_height;               /* cached sizing of label height */
   gint label_width;                /* cached sizing of label width */
   gint max_lines;                  /* Maximum number of lines to draw */
   gint min_lines;                  /* Minimum number of lines to draw */
};


enum
{
   PROP_0,
   PROP_MAX_LINES,
   PROP_MIN_LINES,
   PROP_RENDERER,
};


enum
{
   FORMAT_VALUE,
   SIGNAL_LAST
};


static void uber_graph_render_background (UberGraph *graph);
static void uber_graph_render_foreground (UberGraph *graph,
                                          gdouble     begin_time,
                                          gdouble     end_time);


static gboolean UBER_GRAPH_DEBUG   = FALSE;
static guint    signals[SIGNAL_LAST] = { 0 };


gdouble
uber_get_current_time (void)
{
	GTimeVal tv;
	g_get_current_time(&tv);
	return tv.tv_sec + (tv.tv_usec / (gdouble)G_USEC_PER_SEC);
}


static gboolean
uber_graph_frame_timeout (gpointer data)
{
   GtkWidget *widget = (GtkWidget *)data;
   g_return_val_if_fail(GTK_IS_WIDGET(widget), FALSE);
   gtk_widget_queue_draw(widget);
   return TRUE;
}


void
uber_graph_start (UberGraph *graph)
{
   UberGraphPrivate *priv;
   gdouble time_span;

   g_return_if_fail(UBER_IS_GRAPH(graph));
   g_return_if_fail(!graph->priv->frame_handler);

   priv = graph->priv;

   if (priv->frame_handler) {
      g_warning("Cannot call uber_graph_start() after it has already been "
                "started.");
      return;
   }

   /*
    * Register the frame handler.
    */
   priv->frame_handler = uber_frame_source_add(FRAMES_PER_SECOND,
                                              uber_graph_frame_timeout,
                                              graph);

   /*
    * Prepare the time spans and render the current data-set.
    */
   time_span = priv->n_seconds + priv->n_buffered;
   priv->begin_time = uber_get_current_time();
   priv->end_time = priv->begin_time + time_span;
   uber_graph_render_background(graph);
   uber_graph_render_foreground(graph, 0.0, 0.0);

   gtk_widget_queue_resize(GTK_WIDGET(graph));
}


void
uber_graph_stop (UberGraph *graph)
{
   UberGraphPrivate *priv;
   guint source;

   g_return_if_fail(UBER_IS_GRAPH(graph));

   priv = graph->priv;

   if ((source = priv->frame_handler)) {
      priv->frame_handler = 0;
      g_source_remove(source);
   }
}


static void
uber_graph_adjustment_changed (UberGraph    *graph,
                               GtkAdjustment *adjustment)
{
   UberGraphPrivate *priv;

   g_return_if_fail(UBER_IS_GRAPH(graph));
   g_return_if_fail(GTK_IS_ADJUSTMENT(adjustment));

   priv = graph->priv;

   g_object_get(adjustment,
                "lower", &priv->lower_value,
                "upper", &priv->upper_value,
                NULL);
}


static void
uber_graph_invalidate (UberGraph  *graph,
                       gdouble      begin_time,
                       gdouble      end_time,
                       UberRenderer *renderer)
{
   ENTRY;
   uber_graph_render_foreground(graph, begin_time, end_time);
   EXIT;
}


static void
uber_graph_set_renderer (UberGraph  *graph,
                         UberRenderer *renderer)
{
   UberGraphPrivate *priv;
   GtkAdjustment *adj;

   g_return_if_fail(UBER_IS_GRAPH(graph));
   g_return_if_fail(UBER_IS_RENDERER(renderer));
   g_return_if_fail(graph->priv->renderer == NULL);

   priv = graph->priv;

   priv->renderer = g_object_ref_sink(renderer);
   adj = uber_renderer_get_adjustment(renderer);
   g_object_get(adj,
                "lower", &priv->lower_value,
                "upper", &priv->upper_value,
                NULL);
   priv->adjustment_handler =
      g_signal_connect_swapped(adj, "changed",
                               G_CALLBACK(uber_graph_adjustment_changed),
                               graph);
   priv->invalidate_handler =
      g_signal_connect_swapped(priv->renderer, "invalidate",
                               G_CALLBACK(uber_graph_invalidate),
                               graph);
}


static gdouble
uber_graph_get_fg_x_for_time (UberGraph *graph,
                              gdouble     time_)
{
   UberGraphPrivate *priv;
   gdouble pixel_width;
   gdouble time_span;

   g_return_val_if_fail(UBER_IS_GRAPH(graph), 0.0);

   priv = graph->priv;

   time_span = priv->n_seconds + priv->n_buffered;
   pixel_width = priv->data_area.width;
   return (time_ - priv->begin_time) / time_span * pixel_width;
}


static gdouble
uber_graph_get_fg_time_for_x (UberGraph *graph,
                              gdouble     x)
{
   UberGraphPrivate *priv;
   gdouble pixel_width;
   gdouble time_span;

   g_return_val_if_fail(UBER_IS_GRAPH(graph), 0.0);

   priv = graph->priv;

   time_span = priv->n_seconds + priv->n_buffered;
   pixel_width = priv->data_area.width;
   return x / pixel_width * time_span + priv->begin_time;
}


static void
uber_graph_draw_surface_at_offset (cairo_t         *cr,
                                   cairo_surface_t *surface,
                                   gdouble          surface_x,
                                   gdouble          surface_y,
                                   gdouble          x,
                                   gdouble          y,
                                   gdouble          width,
                                   gdouble          height)
{
   /*
    * Only draw what we can do on integer aligned offsets. This is why
    * we render outside the required range a bit in the asynchronous
    * draw. This helps avoid a "pinstripe" effect on rendered output.
    */
   width -= ceil(x) - x;
   x = ceil(x);

   cairo_save(cr);
   cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
   cairo_set_source_surface(cr, surface, surface_x, surface_y);
   cairo_rectangle(cr, x, y, width, height);
   cairo_fill(cr);
   cairo_restore(cr);
}


static void
uber_graph_draw_surface (cairo_t *cr,
                         cairo_surface_t *surface,
                         gdouble x,
                         gdouble y,
                         gdouble width,
                         gdouble height)
{
   uber_graph_draw_surface_at_offset(cr, surface, x, y, x, y,
                                     width, height);
}


static void
uber_graph_task_notify_state (UberGraph  *graph,
                              GParamSpec *pspec,
                              UberTask   *task)
{
   UberGraphPrivate *priv;
   cairo_surface_t *surface;
   UberTaskState state;
   cairo_t *cr = NULL;
   gdouble begin_time;
   gdouble change;
   gdouble end_time;
   gdouble height;
   gdouble offset_x;
   gdouble target_x;
   gdouble target_y;
   gdouble time_span;
   gdouble width;
   gdouble x;
   gdouble y;

   ENTRY;

   g_return_if_fail(UBER_IS_GRAPH(graph));

   priv = graph->priv;

   g_object_get(task,
                "state", &state,
                "surface", &surface,
                NULL);

   if (state == UBER_TASK_SUCCESS) {
      g_object_get(task,
                   "begin-time", &begin_time,
                   "end-time", &end_time,
                   "height", &height,
                   "width", &width,
                   "x", &x,
                   "y", &y,
                   NULL);

      /*
       * Make sure this render has the right sizing information.
       * A task could finish after a new size-allocate and is therefore
       * now bogus.
       */
      if (((gint)height) != priv->data_area.height) {
         GOTO(cleanup);
      }

      time_span = priv->n_seconds + priv->n_buffered;

      /*
       * If we received a time range outside of what our visible area is,
       * lets update the range our graph contains.
       */
      if (end_time > priv->end_time) {
         change = end_time - priv->end_time;
         priv->offset_time = fmod(priv->offset_time + change, time_span);
         priv->end_time = end_time;
         priv->begin_time = end_time - time_span;
      }

      /*
       * Determine the X offset for the particular time as if there is no
       * offset in the ring buffer.
       */
      target_x = uber_graph_get_fg_x_for_time(graph, begin_time);
      target_y = 0;

      /*
       * Create our cairo context to draw onto the Xlib surface.
       */
      cr = cairo_create(priv->foreground);

      /*
       * Translate the X position into the ring buffers offset.
       */
      if (priv->offset_time > 0.0) {
         offset_x = uber_graph_get_fg_x_for_time(
               graph,
               priv->begin_time + priv->offset_time);
         target_x = fmod(target_x + offset_x, priv->data_area.width);
         if ((target_x + width) > priv->data_area.width) {
            /*
             * Well shit, we have to split this up into two draws. One at
             * the end of the ring buffer, and one at the beginning.
             */
            uber_graph_draw_surface(cr, surface,
                                      target_x, target_y,
                                      priv->data_area.width - target_x,
                                      height);
            width -= priv->data_area.width - target_x;
            target_x = priv->data_area.width - target_x;
            uber_graph_draw_surface_at_offset(cr, surface,
                                                -target_x, target_y,
                                                0.0, target_y,
                                                width, height);
         } else {
            uber_graph_draw_surface(cr, surface,
                                      target_x, target_y, width, height);
         }
      } else {
         uber_graph_draw_surface(cr, surface,
                                   target_x, target_y, width, height);
      }

      cairo_destroy(cr);
      gtk_widget_queue_draw(GTK_WIDGET(graph));
   }

cleanup:
   /*
    * Cleanup our surface that was created for the task.
    */
   if ((state & UBER_TASK_FINISHED_MASK) != 0) {
      cairo_surface_destroy(surface);
   }

   EXIT;
}


static guint
uber_graph_get_n_y_lines (UberGraph *graph)
{
   UberGraphPrivate *priv;
   gdouble height;
   gint max_lines;

   g_return_val_if_fail(UBER_IS_GRAPH(graph), 0);

   priv = graph->priv;

   height = (priv->label_height + LABEL_YPAD * 2 + 5);
   if (!(max_lines = priv->max_lines)) {
      max_lines = priv->content_area.height / height;
   }
   return CLAMP(priv->content_area.height / height,
                priv->min_lines, max_lines);
}


static guint
uber_graph_get_n_x_lines (UberGraph *graph)
{
   /*
    * TODO: Determine N lines based on height/values.
    */
   return 5;
}


static gchar *
uber_graph_format_value (UberGraph *graph,
                         gdouble    value)
{
   UberGraphPrivate *priv;

   /*
    * TODO: This should be a callback in priv rather than branching like this.
    */

   g_return_val_if_fail(UBER_IS_GRAPH(graph), NULL);

   priv = graph->priv;

   if ((priv->flags & UBER_GRAPH_PERCENT)) {
      return g_strdup_printf("%d %%", (gint)value);
   }
   return g_strdup_printf("%0.0f", value);
}


static void
uber_graph_render_background (UberGraph *graph)
{
   static const gdouble dashes[] = { 1.0, 2.0 };
   UberGraphPrivate *priv;
   GtkStateType state;
   PangoLayout *layout;
   GtkWidget *widget = (GtkWidget *)graph;
   GtkStyle *style;
   cairo_t *cr;
   gdouble value;
   gdouble x;
   gdouble y;
   gchar *text;
   gchar label[32];
   guint n_lines;
   gint i;
   gint height;
   gint width;

   ENTRY;

   g_return_if_fail(UBER_IS_GRAPH(graph));

   priv = graph->priv;

   /*
    * Retrieve required resources.
    */
   style = gtk_widget_get_style(widget);
   state = gtk_widget_get_state(widget);
   cr = cairo_create(priv->background);

   /*
    * Set background color to the styles light color.
    */
   gdk_cairo_set_source_color(cr, &style->light[state]);
   cairo_rectangle(cr,
                   priv->content_area.x + 0.5,
                   priv->content_area.y + 0.5,
                   priv->content_area.width - 1.0,
                   priv->content_area.height - 1.0);
   cairo_fill(cr);

   /*
    * Stroke the outer line or the graph to the styles foreground
    * color.
    */
   gdk_cairo_set_source_color(cr, &style->fg[state]);
   cairo_set_line_width(cr, 1.0);
   cairo_set_dash(cr, dashes, G_N_ELEMENTS(dashes), 0);
   cairo_rectangle(cr,
                   priv->content_area.x + 0.5,
                   priv->content_area.y + 0.5,
                   priv->content_area.width - 1.0,
                   priv->content_area.height - 1.0);
   cairo_stroke(cr);

   layout = pango_cairo_create_layout(cr);
   pango_layout_set_font_description(layout, priv->font_desc);

   /*
    * Stroke the inner vertical grid lines of the graph to the styles
    * foreground color. Draw the label at the bottom.
    */
   n_lines = uber_graph_get_n_x_lines(graph);
   for (i = 0; i <= n_lines; i++) {
      x = priv->content_area.x +
          floor(priv->content_area.width / ((gdouble)n_lines + 1) * i) +
          0.5;
      y = priv->content_area.y + 1.5;

      /*
       * Don't draw the first line.
       */
      if (i != 0) {
         cairo_move_to(cr, x, y);
         cairo_line_to(cr, x, y + priv->content_area.height - 3.0);
         cairo_stroke(cr);
      }

      /*
       * Time labels for X axis.
       */
      value = floor(priv->n_seconds / (n_lines + 1.0) * (n_lines + 1 - i));
      g_snprintf(label, sizeof label, "%d", (gint)value);
      pango_layout_set_text(layout, label, -1);
      y = priv->content_area.y + priv->content_area.height + LABEL_YPAD;
      cairo_move_to(cr, x, y);
      pango_cairo_show_layout(cr, layout);
   }

   /*
    * Stroke the inner horizontal grid lines of the graph to the styles
    * foreground color.
    */
   n_lines = uber_graph_get_n_y_lines(graph);
   for (i = 0; i <= n_lines; i++) {
      x = priv->content_area.x + 1.5;
      y = priv->content_area.y +
          floor(priv->content_area.height / ((gdouble)n_lines + 1) * i) +
          0.5;

      /*
       * Don't draw the first line.
       */
      if (i != 0) {
         cairo_move_to(cr, x, y);
         cairo_line_to(cr, x + priv->content_area.width - 3.0, y);
         cairo_stroke(cr);
      }

      /*
       * Time labels for Y axis.
       */
      value = priv->upper_value -
              ((priv->upper_value - priv->lower_value) / (n_lines + 1.0) * i);
      g_signal_emit(graph, signals[FORMAT_VALUE], 0, value, &text);
      pango_layout_set_text(layout, text, -1);
      pango_layout_get_pixel_size(layout, &width, &height);
      x = priv->content_area.x - LABEL_XPAD - width;
      y = priv->content_area.y + (priv->content_area.height / (n_lines + 1.0) * i);
      cairo_move_to(cr, x, y);
      pango_cairo_show_layout(cr, layout);
      g_free(text);
   }

   /*
    * Cleanup resources.
    */
   g_object_unref(layout);
   cairo_destroy(cr);

   EXIT;
}


static void
uber_graph_render_foreground (UberGraph *graph,
                              gdouble     begin_time,
                              gdouble     end_time)
{
   UberGraphPrivate *priv;
   cairo_surface_t *surface;
   UberTask *task;
   gdouble height;
   gdouble width;
   gdouble x;

   g_return_if_fail(UBER_IS_GRAPH(graph));

   priv = graph->priv;

   /*
    * Make sure we have a surface and area for rendering.
    */
   if (!priv->foreground) {
      return;
   }

   /*
    * Fill in current values if requested.
    */
   if (begin_time == 0.0) {
      begin_time = priv->begin_time;
   }
   if (end_time == 0.0) {
      end_time = priv->end_time;
   }

   /*
    * Try to get a bit overlapping of a time so that we don't get a
    * "pinstripe" effect in the output. It is also important to line
    * up on an integer boundry so clearing the region on the destination
    * doesn't have to antialias to neighboring pixels.
    */
   height = priv->data_area.height;
   x = uber_graph_get_fg_x_for_time(graph, begin_time);
   x = floor(x) - 1.0;
   begin_time = uber_graph_get_fg_time_for_x(graph, x);
   width = uber_graph_get_fg_x_for_time(graph, end_time) - x;
   x = 0.0;

   g_assert(height > 0);
   g_assert(width > 0);
   g_assert(x >= 0);

   /*
    * Create a new image surface to render upon within a thread.
    * Since this is threaded, we cannot use an Xlib surface as that
    * would cause corruption to the X thread.
    */
   surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                        ceil(width), ceil(height));

   /*
    * Create asynchronous render task to draw required contents to
    * the surface. When the task has completed, we will copy the image
    * contents to the surface we use to render.
    */
   task = uber_renderer_draw(priv->renderer,
                            cairo_surface_reference(surface),
                            begin_time,
                            end_time,
                            x, 0, width, height);
   g_signal_connect_swapped(task, "notify::state",
                            G_CALLBACK(uber_graph_task_notify_state),
                            graph);
   uber_task_schedule(task);

   /*
    * Cleanup after allocations.
    */
   cairo_surface_destroy(surface);
}


/**
 * uber_graph_set_flags:
 * @graph: (in): A #UberGraph.
 * @flags: (in): flags for the graph.
 *
 * Sets the flags for the graph.
 */
void
uber_graph_set_flags (UberGraph      *graph,
                      UberGraphFlags  flags)
{
   g_return_if_fail(UBER_IS_GRAPH(graph));

   graph->priv->flags = flags;
   uber_graph_render_background(graph);
   uber_graph_render_foreground(graph, 0.0, 0.0);
   gtk_widget_queue_draw(GTK_WIDGET(graph));
}


static void
uber_graph_size_allocate (GtkWidget     *widget,
                            GtkAllocation *alloc)
{
   UberGraphPrivate *priv;
   PangoLayout *layout;
   UberGraph *graph = (UberGraph *)widget;
   GdkWindow *window;
   cairo_t *cr;
   gdouble each;

   g_return_if_fail(UBER_IS_GRAPH(graph));

   priv = graph->priv;

   /*
    * Chain up to allow Gtk to perform the resize.
    */
   GTK_WIDGET_CLASS(uber_graph_parent_class)->size_allocate(widget, alloc);

   /*
    * Get our window to create our suraces and such.
    */
   window = gtk_widget_get_window(widget);
   if (!window) {
      return;
   }

   /*
    * Cleanup after previous cairo surfaces.
    */
   if (priv->foreground) {
      cairo_surface_destroy(priv->foreground);
      priv->foreground = NULL;
   }
   if (priv->background) {
      cairo_surface_destroy(priv->background);
      priv->background = NULL;
   }

   /*
    * Determine the font height so we can draw our labels.
    */
   cr = gdk_cairo_create(window);
   layout = pango_cairo_create_layout(cr);
   pango_layout_set_font_description(layout, priv->font_desc);
   pango_layout_set_text(layout, "XXXXXXXXX", -1);
   pango_layout_get_pixel_size(layout, &priv->label_width,
                               &priv->label_height);
   g_object_unref(layout);
   cairo_destroy(cr);

   /*
    * Setup ring buffer defaults.
    */
   priv->offset_time = 0.0;

   /*
    * Determine the visible data area.
    */
   priv->content_area.x = 1 + priv->label_width;
   priv->content_area.y = 1;
   priv->content_area.width = alloc->width - priv->label_width - 2;
   priv->content_area.height = alloc->height
                             - (LABEL_YPAD * 2)
                             - priv->label_height
                             - 2;

   /*
    * Determine the data area including buffer area.
    */
   each = priv->content_area.width / priv->n_seconds;
   priv->data_area.x = 0;
   priv->data_area.y = 0;
   priv->data_area.width = priv->content_area.width +
                           ceil(priv->n_buffered * each);
   priv->data_area.height = priv->content_area.height;

   /*
    * Create new cairo surface for drawing the background.
    */
   priv->background =
      gdk_window_create_similar_surface(window,
                                        CAIRO_CONTENT_COLOR_ALPHA,
                                        alloc->width, alloc->height);

   /*
    * Create new cairo surface for drawing the foreground. This matches the
    * size of the content area plus enough space for the buffered region.
    */
   priv->foreground =
      gdk_window_create_similar_surface(window,
                                        CAIRO_CONTENT_COLOR_ALPHA,
                                        priv->data_area.width,
                                        priv->data_area.height);

   /*
    * Render the entire graph immediately.
    */
   uber_graph_render_background(graph);
   uber_graph_render_foreground(graph, 0.0, 0.0);
}


static gboolean
uber_graph_draw (GtkWidget *widget,
                   cairo_t   *cr)
{
   UberGraph *graph = (UberGraph *)widget;
   UberGraphPrivate *priv;
   GtkAllocation a;
   gdouble height;
   gdouble offset_x;
   gdouble scroll_x;
   gdouble time_span;
   gdouble width;
   gdouble x;
   gdouble y;

   g_return_val_if_fail(UBER_IS_GRAPH(graph), FALSE);

   priv = graph->priv;
   priv->frame_count++;

   if (gtk_widget_is_drawable(widget)) {
      /*
       * Ensure everything is order.
       */
      g_assert(priv->background);
      g_assert(priv->foreground);
      gtk_widget_get_allocation(widget, &a);

      /*
       * Blit the background to the context.
       */
      cairo_set_source_surface(cr, priv->background, 0, 0);
      cairo_rectangle(cr, 0, 0, a.width, a.height);
      cairo_fill(cr);

      /*
       * Clip future operations to the content area.
       */
      cairo_rectangle(cr,
                      priv->content_area.x + 1.5,
                      priv->content_area.y + 1.5,
                      priv->content_area.width - 3.0,
                      priv->content_area.height - 3.0);
      cairo_clip(cr);

      /*
       * At this point we are ready to blit our foreground (which is treated
       * as a pixmap ring buffer) to the widgets surface. This is done in
       * two steps. First, we draw the freshest part of the surface (always
       * the left hand side of the source) to the right hand side of the
       * destination. Then we draw the less-fresh data (always the right
       * hand side of the source) to the right hand side of the destination.
       */
      offset_x = uber_graph_get_fg_x_for_time(graph,
                                                priv->begin_time +
                                                priv->offset_time);
      time_span = priv->n_seconds + priv->n_buffered;
      scroll_x = priv->data_area.width / time_span *
                 (uber_get_current_time() - priv->end_time);

      /*
       * Draw the left side of the ring buffer.
       */
      if (offset_x > 0.0) {
         x = priv->content_area.x + priv->data_area.width - offset_x;
         y = priv->content_area.y;
         width = offset_x;
         height = priv->content_area.height;
         x -= scroll_x;
         cairo_set_source_surface(cr, priv->foreground, x, y);
         cairo_rectangle(cr, x, y, width, height);
         cairo_fill(cr);
      }

      /*
       * Draw the right side of ring buffer.
       */
      x = priv->content_area.x - offset_x;
      y = priv->content_area.y;
      width = priv->data_area.width;
      height = priv->content_area.height;
      x -= scroll_x;
      x += 1.0; /* Avoid 1 pixel gap */
      cairo_set_source_surface(cr, priv->foreground, x, y);
      cairo_rectangle(cr, x, y, width, height);
      cairo_fill(cr);

      /*
       * Draw frame and clocking information on top of the graph if
       * debugging is enabled.
       */
      if (G_UNLIKELY(UBER_GRAPH_DEBUG)) {
         PangoLayout *layout;
         struct timespec ts;
         char text[32];

         clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
         g_snprintf(text, sizeof text, "%ld.%09ld\n%d",
                    ts.tv_sec, ts.tv_nsec, priv->frame_count);
         layout = pango_cairo_create_layout(cr);
         cairo_set_source_rgb(cr, 1, 0, 0);
         pango_layout_set_text(layout, text, -1);
         cairo_move_to(cr, priv->content_area.width / 2,
                       priv->content_area.height / 2);
         pango_cairo_show_layout(cr, layout);
         g_object_unref(layout);
      }
   }

   return FALSE;
}


static void
uber_graph_set_max_lines (UberGraph *graph,
                          gint        max_lines)
{
   g_return_if_fail(UBER_IS_GRAPH(graph));

   graph->priv->max_lines = max_lines;
   uber_graph_render_background(graph);
   gtk_widget_queue_draw(GTK_WIDGET(graph));
}


static void
uber_graph_set_min_lines (UberGraph *graph,
                            gint        min_lines)
{
   g_return_if_fail(UBER_IS_GRAPH(graph));

   graph->priv->min_lines = min_lines;
   uber_graph_render_background(graph);
   gtk_widget_queue_draw(GTK_WIDGET(graph));
}


static void
uber_graph_get_preferred_width (GtkWidget *widget,
                                  gint      *min_width,
                                  gint      *natural_width)
{
   /*
    * TODO: Calculate based on font sizing and such.
    */
   *min_width = *natural_width = 150;
}


static void
uber_graph_get_preferred_height (GtkWidget *widget,
                                 gint      *min_height,
                                 gint      *natural_height)
{
   /*
    * TODO: Calculate based on font sizing and such.
    */
   *min_height = *natural_height = 50;
}


static gboolean
uber_graph_string_accumulator (GSignalInvocationHint *ihint,
                                 GValue                *return_accu,
                                 const GValue          *handler_return,
                                 gpointer               data)
{
   if (g_value_get_string(handler_return)) {
      g_value_copy(handler_return, return_accu);
      return FALSE;
   }
   return TRUE;
}


static void
uber_graph_realize (GtkWidget *widget)
{
   ENTRY;
   GTK_WIDGET_CLASS(uber_graph_parent_class)->realize(widget);
   gtk_widget_queue_resize(widget);
   EXIT;
}


static void
uber_graph_dispose (GObject *object)
{
   UberGraphPrivate *priv = UBER_GRAPH(object)->priv;

   ENTRY;

   uber_graph_stop(UBER_GRAPH(object));

   if (priv->invalidate_handler) {
      g_source_remove(priv->invalidate_handler);
   }

   if (priv->adjustment_handler) {
      g_source_remove(priv->adjustment_handler);
   }

   g_clear_object(&priv->renderer);

   G_OBJECT_CLASS(uber_graph_parent_class)->dispose(object);

   EXIT;
}


static void
uber_graph_finalize (GObject *object)
{
   UberGraphPrivate *priv = UBER_GRAPH(object)->priv;

   ENTRY;

   if (priv->foreground) {
      cairo_surface_destroy(priv->foreground);
      priv->foreground = NULL;
   }

   if (priv->background) {
      cairo_surface_destroy(priv->background);
      priv->background = NULL;
   }

   if (priv->font_desc) {
      pango_font_description_free(priv->font_desc);
      priv->font_desc = NULL;
   }

   G_OBJECT_CLASS(uber_graph_parent_class)->finalize(object);

   EXIT;
}


static void
uber_graph_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
   UberGraph *graph = UBER_GRAPH(object);

   switch (prop_id) {
   case PROP_MAX_LINES:
      uber_graph_set_max_lines(graph, g_value_get_int(value));
      break;
   case PROP_MIN_LINES:
      uber_graph_set_min_lines(graph, g_value_get_int(value));
      break;
   case PROP_RENDERER:
      uber_graph_set_renderer(graph, g_value_get_object(value));
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}


static void
uber_graph_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
   UberGraph *graph = UBER_GRAPH(object);

   switch (prop_id) {
   case PROP_MAX_LINES:
      g_value_set_int(value, graph->priv->max_lines);
      break;
   case PROP_MIN_LINES:
      g_value_set_int(value, graph->priv->min_lines);
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}


static void
uber_graph_class_init (UberGraphClass *klass)
{
   GObjectClass *object_class;
   GtkWidgetClass *widget_class;

   object_class = G_OBJECT_CLASS(klass);
   object_class->dispose = uber_graph_dispose;
   object_class->finalize = uber_graph_finalize;
   object_class->set_property = uber_graph_set_property;
   object_class->get_property = uber_graph_get_property;
   g_type_class_add_private(object_class, sizeof(UberGraphPrivate));

   widget_class = GTK_WIDGET_CLASS(klass);
   widget_class->size_allocate = uber_graph_size_allocate;
   widget_class->draw = uber_graph_draw;
   widget_class->get_preferred_height = uber_graph_get_preferred_height;
   widget_class->get_preferred_width = uber_graph_get_preferred_width;
   widget_class->realize = uber_graph_realize;

   klass->format_value = uber_graph_format_value;

   g_object_class_install_property(object_class,
                                   PROP_RENDERER,
                                   g_param_spec_object("renderer",
                                                       "renderer",
                                                       "renderer",
                                                       UBER_TYPE_RENDERER,
                                                       G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

   g_object_class_install_property(object_class,
                                   PROP_MAX_LINES,
                                   g_param_spec_int("max-lines",
                                                    "max-lines",
                                                    "max-lines",
                                                    0,
                                                    G_MAXINT,
                                                    0,
                                                    G_PARAM_READWRITE));

   g_object_class_install_property(object_class,
                                   PROP_MIN_LINES,
                                   g_param_spec_int("min-lines",
                                                    "min-lines",
                                                    "min-lines",
                                                    0,
                                                    G_MAXINT,
                                                    0,
                                                    G_PARAM_READWRITE));

   signals[FORMAT_VALUE] = g_signal_new("format-value",
                                        UBER_TYPE_GRAPH,
                                        G_SIGNAL_RUN_LAST,
                                        G_STRUCT_OFFSET(UberGraphClass, format_value),
                                        uber_graph_string_accumulator,
                                        NULL,
                                        g_cclosure_marshal_generic,
                                        G_TYPE_STRING,
                                        1,
                                        G_TYPE_DOUBLE);

   if (g_getenv("UBER_GRAPH_DEBUG")) {
      UBER_GRAPH_DEBUG = 1;
   }
}


static void
uber_graph_init (UberGraph *graph)
{
   UberGraphPrivate *priv;

   priv = G_TYPE_INSTANCE_GET_PRIVATE(graph, UBER_TYPE_GRAPH,
                                      UberGraphPrivate);
   graph->priv = priv;

   priv->n_seconds = 60.0;
   priv->n_buffered = 1.0;

   priv->font_desc = pango_font_description_new();
   pango_font_description_set_family_static(priv->font_desc, "Monospace");
   pango_font_description_set_size(priv->font_desc, PANGO_SCALE * 8);
}
