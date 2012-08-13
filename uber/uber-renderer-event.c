/* uber-renderer-event.c
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

#include <string.h>

#include "uber-debug.h"
#include "uber-renderer-event.h"
#include "uber-task-render.h"


typedef struct
{
	UberRendererEvent *renderer;
	gint              id;
	GQuark            key;
	PkModel          *model;
	gdouble           width;
	GdkColor          color;
	guint             handler;
	gdouble           end_time;
} Event;


struct _UberRendererEventPrivate
{
	GPtrArray     *events;
	GtkAdjustment *range;
	gdouble        end_time;
};


static void uber_renderer_init (UberRendererIface *iface);


G_DEFINE_TYPE_EXTENDED(UberRendererEvent,
                       uber_renderer_event,
                       G_TYPE_INITIALLY_UNOWNED,
                       0,
                       G_IMPLEMENT_INTERFACE(UBER_TYPE_RENDERER,
                                             uber_renderer_init))


static inline gdouble
get_x_for_time (gdouble ratio,
                gdouble x,
                gdouble begin_time,
                gdouble time_)
{
	return x + (ratio * (time_ - begin_time));
}


static inline gdouble
get_y_for_range (gdouble ratio,
                 gdouble y,
                 gdouble begin_value,
                 gdouble value)
{
	return y - (ratio * (value - begin_value));
}


static void
uber_renderer_event_render (UberRendererEvent *event,
                           UberTask         *task)
{
	UberRendererEventPrivate *priv;
	cairo_surface_t *surface;
	PkModelIter iter;
	cairo_t *cr;
	gdouble aggregate_time;
	gdouble begin_time;
	gdouble end_time;
	gdouble height;
	gdouble iter_x;
	gdouble width;
	gdouble x;
	gdouble x_ratio;
	gdouble y;
	Event *item;
	gint i;

	ENTRY;

	g_return_if_fail(UBER_IS_RENDERER_EVENT(event));
	g_return_if_fail(UBER_IS_TASK(task));

	priv = event->priv;

	g_object_get(task,
	             "begin-time", &begin_time,
	             "end-time", &end_time,
	             "height", &height,
	             "surface", &surface,
	             "x", &x,
	             "y", &y,
	             "width", &width,
	             NULL);

	if (begin_time == end_time) {
		EXIT;
	}

	g_assert(surface);
	g_assert(width > 0.0);
	g_assert(height > 0.0);

	x_ratio = width / (end_time - begin_time);

	/*
	 * To reduce the number of data points we look at, we will ask the
	 * data model to aggregate values to a given range of time. We only
	 * need a max of one data point per pixel.
	 */
	aggregate_time = (end_time - begin_time) / width;

	cr = cairo_create(surface);

	for (i = 0; i < priv->events->len; i++) {
		item = g_ptr_array_index(priv->events, i);

		cairo_set_line_width(cr, item->width);
		gdk_cairo_set_source_color(cr, &item->color);

		if (pk_model_get_iter_for_range(item->model, &iter,
		                                begin_time, end_time,
		                                aggregate_time)) {
			do {
				iter_x = get_x_for_time(x_ratio, x, begin_time, iter.time);
				cairo_move_to(cr, iter_x, y);
				cairo_line_to(cr, iter_x, y + height);
			} while (pk_model_iter_next(item->model, &iter));
			cairo_stroke(cr);
		}
	}

	cairo_destroy(cr);

	EXIT;
}


static UberTask*
uber_renderer_event_draw (UberRenderer     *renderer,
                         cairo_surface_t *surface,
                         gdouble          begin_time,
                         gdouble          end_time,
                         gdouble          x,
                         gdouble          y,
                         gdouble          width,
                         gdouble          height)
{
	UberTask *task;

	ENTRY;

	g_return_val_if_fail(UBER_IS_RENDERER_EVENT(renderer), NULL);

	task = g_object_new(UBER_TYPE_TASK_RENDER,
	                    "begin-time", begin_time,
	                    "end-time", end_time,
	                    "height", height,
	                    "surface", surface,
	                    "width", width,
	                    "x", x,
	                    "y", y,
	                    NULL);
	g_signal_connect_object(task, "render",
	                        G_CALLBACK(uber_renderer_event_render),
	                        renderer,
	                        G_CONNECT_SWAPPED);
	RETURN(task);
}


static GtkAdjustment*
uber_renderer_event_get_adjustment (UberRenderer *renderer)
{
	UberRendererEvent *event = (UberRendererEvent *)renderer;
	g_return_val_if_fail(UBER_IS_RENDERER_EVENT(event), NULL);
	return event->priv->range;
}


static void
uber_renderer_event_notify_end_time (PkModel    *model,
                                    GParamSpec *pspec,
                                    Event       *item)
{
	gdouble last_end_time;

	g_return_if_fail(PK_IS_MODEL(model));
	g_return_if_fail(item != NULL);
	g_return_if_fail(UBER_IS_RENDERER_EVENT(item->renderer));

	last_end_time = item->end_time;
	item->end_time = pk_model_get_end_time(model);
	uber_renderer_emit_invalidate(UBER_RENDERER(item->renderer),
	                             last_end_time, item->end_time);
}

gint
uber_renderer_event_append (UberRendererEvent *event,
                           PkModel          *model,
                           GQuark            key)
{
	static gint sequence = 0;
	UberRendererEventPrivate *priv;
	UberRenderer *renderer = (UberRenderer *)event;
	Event *new_event;

	g_return_val_if_fail(UBER_IS_RENDERER_EVENT(event), 0);
	g_return_val_if_fail(PK_IS_MODEL(model), 0);
	g_return_val_if_fail(key != 0, 0);

	priv = event->priv;

	new_event = g_new0(Event, 1);
	new_event->renderer = event;
	new_event->id = ++sequence;
	new_event->key = key;
	new_event->width = 1.0;
	new_event->model = g_object_ref(model);
	new_event->handler =
		g_signal_connect(model, "notify::end-time",
		                 G_CALLBACK(uber_renderer_event_notify_end_time),
		                 new_event);

	g_ptr_array_add(priv->events, new_event);
	uber_renderer_emit_invalidate(renderer, 0.0, 0.0);

	return new_event->id;
}

void
uber_renderer_event_remove (UberRendererEvent *event,
                           gint              identifier)
{
	UberRendererEventPrivate *priv;
	UberRenderer *renderer = (UberRenderer *)event;
	Event *item;
	gint i;

	g_return_if_fail(UBER_IS_RENDERER_EVENT(event));
	g_return_if_fail(UBER_IS_RENDERER(renderer));
	g_return_if_fail(identifier > 0);

	priv = event->priv;

	for (i = 0; i < priv->events->len; i++) {
		item = g_ptr_array_index(priv->events, i);
		if (item->id == identifier) {
			g_signal_handler_disconnect(item->model, item->handler);
			item->handler = 0;
			g_clear_object(&item->model);
			g_ptr_array_remove_index(priv->events, i);
			uber_renderer_emit_invalidate(renderer, 0.0, 0.0);
			break;
		}
	}
}


/**
 * uber_renderer_event_dispose:
 * @object: (in): A #GObject.
 *
 * Dispose callback for @object.  This method releases references held
 * by the #GObject instance.
 *
 * Returns: None.
 * Side effects: Plenty.
 */
static void
uber_renderer_event_dispose (GObject *object)
{
	UberRendererEventPrivate *priv = UBER_RENDERER_EVENT(object)->priv;
	UberRendererEvent *event = (UberRendererEvent *)object;

	ENTRY;

	while (priv->events->len) {
		uber_renderer_event_remove(event, 0);
	}

	G_OBJECT_CLASS(uber_renderer_event_parent_class)->dispose(object);

	EXIT;
}


/**
 * uber_renderer_event_finalize:
 * @object: (in): A #UberRendererEvent.
 *
 * Finalizer for a #UberRendererEvent instance.  Frees any resources held by
 * the instance.
 *
 * Returns: None.
 * Side effects: None.
 */
static void
uber_renderer_event_finalize (GObject *object)
{
	UberRendererEventPrivate *priv = UBER_RENDERER_EVENT(object)->priv;

	ENTRY;

	g_ptr_array_free(priv->events, TRUE);

	G_OBJECT_CLASS(uber_renderer_event_parent_class)->finalize(object);

	EXIT;
}


/**
 * uber_renderer_event_set_property:
 * @object: (in): A #GObject.
 * @prop_id: (in): The property identifier.
 * @value: (in): The given property.
 * @pspec: (in): A #ParamSpec.
 *
 * Set a given #GObject property.
 */
static void
uber_renderer_event_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
	switch (prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}


/**
 * uber_renderer_event_class_init:
 * @klass: (in): A #UberRendererEventClass.
 *
 * Initializes the #UberRendererEventClass and prepares the vtable.
 *
 * Returns: None.
 * Side effects: None.
 */
static void
uber_renderer_event_class_init (UberRendererEventClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS(klass);
	object_class->dispose = uber_renderer_event_dispose;
	object_class->finalize = uber_renderer_event_finalize;
	object_class->set_property = uber_renderer_event_set_property;
	g_type_class_add_private(object_class, sizeof(UberRendererEventPrivate));
}


/**
 * uber_renderer_event_init:
 * @event: (in): A #UberRendererEvent.
 *
 * Initializes the newly created #UberRendererEvent instance.
 *
 * Returns: None.
 * Side effects: None.
 */
static void
uber_renderer_event_init (UberRendererEvent *event)
{
	event->priv =
		G_TYPE_INSTANCE_GET_PRIVATE(event,
		                            UBER_TYPE_RENDERER_EVENT,
		                            UberRendererEventPrivate);
	event->priv->events = g_ptr_array_new();
	event->priv->range = g_object_new(GTK_TYPE_ADJUSTMENT,
	                                 "lower", 0.0,
	                                 "upper", 100.0,
	                                 NULL);
}


static void
uber_renderer_init (UberRendererIface *iface)
{
	iface->draw = uber_renderer_event_draw;
	iface->get_adjustment = uber_renderer_event_get_adjustment;
}
