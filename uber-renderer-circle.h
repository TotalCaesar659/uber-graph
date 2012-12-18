/* uber-renderer-circle.h
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

#ifndef UBER_RENDERER_CIRCLE_H
#define UBER_RENDERER_CIRCLE_H

#include "uber-renderer.h"

G_BEGIN_DECLS

#define UBER_TYPE_RENDERER_CIRCLE            (uber_renderer_circle_get_type())
#define UBER_RENDERER_CIRCLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_RENDERER_CIRCLE, UberRendererCircle))
#define UBER_RENDERER_CIRCLE_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_RENDERER_CIRCLE, UberRendererCircle const))
#define UBER_RENDERER_CIRCLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UBER_TYPE_RENDERER_CIRCLE, UberRendererCircleClass))
#define UBER_IS_RENDERER_CIRCLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBER_TYPE_RENDERER_CIRCLE))
#define UBER_IS_RENDERER_CIRCLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UBER_TYPE_RENDERER_CIRCLE))
#define UBER_RENDERER_CIRCLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UBER_TYPE_RENDERER_CIRCLE, UberRendererCircleClass))

typedef struct _UberRendererCircle        UberRendererCircle;
typedef struct _UberRendererCircleClass   UberRendererCircleClass;
typedef struct _UberRendererCirclePrivate UberRendererCirclePrivate;

struct _UberRendererCircle
{
   UberRenderer parent;

   /*< private >*/
   UberRendererCirclePrivate *priv;
};

struct _UberRendererCircleClass
{
   UberRendererClass parent_class;
};

void          uber_renderer_circle_get_color  (UberRendererCircle *circle,
                                               GdkRGBA            *color);
gdouble       uber_renderer_circle_get_radius (UberRendererCircle *circle);
GType         uber_renderer_circle_get_type   (void) G_GNUC_CONST;
gdouble       uber_renderer_circle_get_x      (UberRendererCircle *circle);
gdouble       uber_renderer_circle_get_y      (UberRendererCircle *circle);
UberRenderer *uber_renderer_circle_new        (void);
void          uber_renderer_circle_set_color  (UberRendererCircle *circle,
                                               const GdkRGBA      *color);
void          uber_renderer_circle_set_radius (UberRendererCircle *circle,
                                               gdouble             radius);
void          uber_renderer_circle_set_x      (UberRendererCircle *circle,
                                               gdouble             x);
void          uber_renderer_circle_set_y      (UberRendererCircle *circle,
                                               gdouble             y);

G_END_DECLS

#endif /* UBER_RENDERER_CIRCLE_H */
