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

#include "uber-range.h"

G_BEGIN_DECLS

#define UBER_TYPE_RENDERER            (uber_renderer_get_type())
#define UBER_RENDERER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_RENDERER, UberRenderer))
#define UBER_RENDERER_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_RENDERER, UberRenderer const))
#define UBER_RENDERER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UBER_TYPE_RENDERER, UberRendererClass))
#define UBER_IS_RENDERER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBER_TYPE_RENDERER))
#define UBER_IS_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UBER_TYPE_RENDERER))
#define UBER_RENDERER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UBER_TYPE_RENDERER, UberRendererClass))

typedef struct _UberRenderer        UberRenderer;
typedef struct _UberRendererClass   UberRendererClass;
typedef struct _UberRendererPrivate UberRendererPrivate;

struct _UberRenderer
{
   GInitiallyUnowned parent;

   /*< private >*/
   UberRendererPrivate *priv;
};

struct _UberRendererClass
{
   GInitiallyUnownedClass parent_class;

   void (*draw) (UberRenderer    *renderer,
                 cairo_t         *cr,
                 GdkRectangle    *area,
                 const UberRange *x_range,
                 const UberRange *y_range);
};

GType uber_renderer_get_type (void) G_GNUC_CONST;
void  uber_renderer_draw     (UberRenderer    *renderer,
                              cairo_t         *cr,
                              GdkRectangle    *area,
                              const UberRange *x_range,
                              const UberRange *y_range);

G_END_DECLS

#endif /* UBER_RENDERER_H */
