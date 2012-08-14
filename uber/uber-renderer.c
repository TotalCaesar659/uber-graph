/* uber-renderer.c
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

#include "uber-renderer.h"

enum
{
	INVALIDATE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

/**
 * uber_renderer_draw:
 * @renderer: (in): A #UberRenderer.
 * @surface: (in): A #cairo_surface_t to draw to.
 * @begin_time: (in): The begin of the time range to render.
 * @end_time: (in): The end of the time range to render.
 * @x: (in): The X coordinate.
 * @y: (in): The Y coordinate.
 * @width: (in): The width of the rendering.
 * @height: (in): The height of the rendering.
 *
 * Generates an asynchronous task to complete a draw request on the given
 * surface.
 *
 * Returns: (transfer full): An #UberTask.
 */
UberTask *
uber_renderer_draw (UberRenderer     *renderer,
                    cairo_surface_t *surface,
                    gdouble          begin_time,
                    gdouble          end_time,
                    gdouble          x,
                    gdouble          y,
                    gdouble          width,
                    gdouble          height)
{
	g_return_val_if_fail(UBER_IS_RENDERER(renderer), NULL);
	g_return_val_if_fail(surface, NULL);
	return UBER_RENDERER_GET_INTERFACE(renderer)->
      draw(renderer, surface, begin_time, end_time, x, y, width, height);
}


void
uber_renderer_emit_invalidate (UberRenderer *renderer,
                               gdouble      begin_time,
                               gdouble      end_time)
{
	g_signal_emit(renderer, signals[INVALIDATE], 0, begin_time, end_time);
}

/**
 * uber_renderer_get_adjustment:
 * @renderer: (in): A #UberRenderer.
 *
 * Gets an adjustment representing the range of the renderer.
 *
 * Returns: (transfer none): A #GtkAdjustment.
 */
GtkAdjustment *
uber_renderer_get_adjustment (UberRenderer *renderer)
{
	g_return_val_if_fail(UBER_IS_RENDERER(renderer), NULL);
	return UBER_RENDERER_GET_INTERFACE(renderer)->get_adjustment(renderer);
}


static void
uber_renderer_base_init (UberRendererIface *iface)
{
	static gsize initialized = FALSE;

	if (g_once_init_enter(&initialized)) {
		signals[INVALIDATE] = g_signal_new("invalidate",
		                                   UBER_TYPE_RENDERER,
		                                   G_SIGNAL_RUN_FIRST,
		                                   0,
		                                   NULL,
		                                   NULL,
                                         g_cclosure_marshal_generic,
		                                   G_TYPE_NONE,
		                                   2,
		                                   G_TYPE_DOUBLE,
		                                   G_TYPE_DOUBLE);
		g_once_init_leave(&initialized, TRUE);
	}

}


/**
 * uber_renderer_get_type:
 *
 * Retrieves the #GType for #UberRenderer.
 *
 * Returns: A #GType for #UberRenderer.
 * Side effects: Registers #GType on first call.
 */
GType
uber_renderer_get_type (void)
{
	static GType type_id = 0;

	if (g_once_init_enter((gsize *)&type_id)) {
		GType _type_id;
		const GTypeInfo g_type_info = {
			sizeof(UberRendererIface),
			(GBaseInitFunc)uber_renderer_base_init,
			NULL, /* base_finalize */
			NULL, /* class_init */
			NULL, /* class_finalize */
			NULL, /* class_data */
			0,    /* instance_size */
			0,    /* n_preallocs */
			NULL, /* instance_init */
			NULL  /* value_vtable */
		};

		_type_id = g_type_register_static(G_TYPE_INTERFACE,
		                                  "UberRenderer",
		                                  &g_type_info,
		                                  0);
		g_type_interface_add_prerequisite(_type_id, G_TYPE_INITIALLY_UNOWNED);
		g_once_init_leave((gsize *)&type_id, _type_id);
	}
	return type_id;
}
