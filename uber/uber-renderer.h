/* uber-renderer.h
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

#ifndef UBER_RENDERER_H
#define UBER_RENDERER_H

#include <gtk/gtk.h>

#include "uber-task.h"

G_BEGIN_DECLS

#define UBER_TYPE_RENDERER             (uber_renderer_get_type())
#define UBER_RENDERER(o)               (G_TYPE_CHECK_INSTANCE_CAST((o),    UBER_TYPE_RENDERER, UberRenderer))
#define UBER_IS_RENDERER(o)            (G_TYPE_CHECK_INSTANCE_TYPE((o),    UBER_TYPE_RENDERER))
#define UBER_RENDERER_GET_INTERFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE((o), UBER_TYPE_RENDERER, UberRendererIface))

typedef struct _UberRenderer      UberRenderer;
typedef struct _UberRendererIface UberRendererIface;

struct _UberRendererIface
{
	GTypeInterface parent;

	/* interface methods */
	UberTask*       (*draw)           (UberRenderer     *renderer,
	                                  cairo_surface_t *surface,
	                                  gdouble          begin_time,
	                                  gdouble          end_time,
	                                  gdouble          x,
	                                  gdouble          y,
	                                  gdouble          width,
	                                  gdouble          height);
	GtkAdjustment* (*get_adjustment) (UberRenderer     *renderer);
};

GType          uber_renderer_get_type        (void) G_GNUC_CONST;
UberTask*       uber_renderer_draw            (UberRenderer     *renderer,
                                             cairo_surface_t *surface,
                                             gdouble          begin_time,
                                             gdouble          end_time,
                                             gdouble          x,
                                             gdouble          y,
                                             gdouble          width,
                                             gdouble          height);
void           uber_renderer_emit_invalidate (UberRenderer     *renderer,
                                             gdouble          begin_time,
                                             gdouble          end_time);
GtkAdjustment* uber_renderer_get_adjustment  (UberRenderer     *renderer);

G_END_DECLS

#endif /* UBER_RENDERER_H */
