/* uber-renderer-circle.c
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

#include <glib/gi18n.h>
#include <math.h>

#include "uber-renderer-circle.h"

G_DEFINE_TYPE(UberRendererCircle, uber_renderer_circle, UBER_TYPE_RENDERER)

struct _UberRendererCirclePrivate
{
   GdkRGBA color;
   gdouble radius;
   gdouble x;
   gdouble y;
};

enum
{
   PROP_0,
   PROP_COLOR,
   PROP_RADIUS,
   PROP_X,
   PROP_Y,
   LAST_PROP
};

static GParamSpec *gParamSpecs[LAST_PROP];

UberRenderer *
uber_renderer_circle_new (void)
{
   return g_object_new(UBER_TYPE_RENDERER_CIRCLE, NULL);
}

gdouble
uber_renderer_circle_get_x (UberRendererCircle *circle)
{
   g_return_val_if_fail(UBER_IS_RENDERER_CIRCLE(circle), 0.0);
   return circle->priv->x;
}

void
uber_renderer_circle_set_x (UberRendererCircle *circle,
                            gdouble             x)
{
   g_return_if_fail(UBER_IS_RENDERER_CIRCLE(circle));
   circle->priv->x = x;
   g_object_notify_by_pspec(G_OBJECT(circle), gParamSpecs[PROP_X]);
}

gdouble
uber_renderer_circle_get_y (UberRendererCircle *circle)
{
   g_return_val_if_fail(UBER_IS_RENDERER_CIRCLE(circle), 0.0);
   return circle->priv->y;
}

void
uber_renderer_circle_set_y (UberRendererCircle *circle,
                            gdouble             y)
{
   g_return_if_fail(UBER_IS_RENDERER_CIRCLE(circle));
   circle->priv->y = y;
   g_object_notify_by_pspec(G_OBJECT(circle), gParamSpecs[PROP_Y]);
}

/**
 * uber_renderer_circle_get_color:
 * @circle: (in): A #UberRendererCircle.
 * @color: (out): A #GdkRGBA.
 *
 * Gets the "color" property and stores it in @color.
 */
void
uber_renderer_circle_get_color (UberRendererCircle *circle,
                                GdkRGBA            *color)
{
   g_return_if_fail(UBER_IS_RENDERER_CIRCLE(circle));
   g_return_if_fail(color);
   *color = circle->priv->color;
}

void
uber_renderer_circle_set_color (UberRendererCircle *circle,
                                const GdkRGBA      *color)
{
   UberRendererCirclePrivate *priv;

   g_return_if_fail(UBER_IS_RENDERER_CIRCLE(circle));

   priv = circle->priv;

   if (color) {
      priv->color = *color;
   } else {
      memset(&priv->color, 0, sizeof priv->color);
   }

   g_object_notify_by_pspec(G_OBJECT(circle), gParamSpecs[PROP_COLOR]);
}

gdouble
uber_renderer_circle_get_radius (UberRendererCircle *circle)
{
   g_return_val_if_fail(UBER_IS_RENDERER_CIRCLE(circle), 0.0);
   return circle->priv->radius;
}

void
uber_renderer_circle_set_radius (UberRendererCircle *circle,
                                 gdouble             radius)
{
   g_return_if_fail(UBER_IS_RENDERER_CIRCLE(circle));
   g_return_if_fail(radius > 0.0);

   if (radius > 0.0) {
      circle->priv->radius = radius;
      g_object_notify_by_pspec(G_OBJECT(circle), gParamSpecs[PROP_COLOR]);
   }
}

static void
uber_renderer_circle_draw (UberRenderer    *renderer,
                           cairo_t         *cr,
                           GdkRectangle    *area,
                           const UberRange *x_range,
                           const UberRange *y_range)
{
   UberRendererCirclePrivate *priv;
   UberRendererCircle *circle = (UberRendererCircle *)renderer;
   gdouble x;
   gdouble y;
   gdouble r;

   g_assert(UBER_IS_RENDERER_CIRCLE(circle));
   g_assert(cr);
   g_assert(area);
   g_assert(x_range);
   g_assert(y_range);

   priv = circle->priv;

   x = uber_range_translate(x_range, priv->x);
   y = uber_range_translate(y_range, priv->y);
   r = 1.0 / area->width * 3.0;

   //g_print ("{priv->x: %f, x: %f, priv->y: %f, y: %f}\n", priv->x, x, priv->y, y);

   cairo_save(cr);
   cairo_arc(cr, x, y, r, 0.0, 2.0 * M_PI);
   gdk_cairo_set_source_rgba(cr, &priv->color);
   cairo_fill(cr);
   cairo_restore(cr);
}

static void
uber_renderer_circle_finalize (GObject *object)
{
   G_OBJECT_CLASS(uber_renderer_circle_parent_class)->finalize(object);
}

static void
uber_renderer_circle_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
   UberRendererCircle *circle = UBER_RENDERER_CIRCLE(object);

   switch (prop_id) {
   case PROP_COLOR: {
      GdkRGBA color;
      uber_renderer_circle_get_color(circle, &color);
      g_value_set_boxed(value, &color);
      break;
   }
   case PROP_RADIUS:
      g_value_set_double(value, uber_renderer_circle_get_radius(circle));
      break;
   case PROP_X:
      g_value_set_double(value, uber_renderer_circle_get_x(circle));
      break;
   case PROP_Y:
      g_value_set_double(value, uber_renderer_circle_get_y(circle));
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}

static void
uber_renderer_circle_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
   UberRendererCircle *circle = UBER_RENDERER_CIRCLE(object);

   switch (prop_id) {
   case PROP_COLOR:
      uber_renderer_circle_set_color(circle, g_value_get_boxed(value));
      break;
   case PROP_RADIUS:
      uber_renderer_circle_set_radius(circle, g_value_get_double(value));
      break;
   case PROP_X:
      uber_renderer_circle_set_x(circle, g_value_get_double(value));
      break;
   case PROP_Y:
      uber_renderer_circle_set_y(circle, g_value_get_double(value));
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}

static void
uber_renderer_circle_class_init (UberRendererCircleClass *klass)
{
   GObjectClass *object_class;
   UberRendererClass *renderer_class;

   object_class = G_OBJECT_CLASS(klass);
   object_class->finalize = uber_renderer_circle_finalize;
   object_class->get_property = uber_renderer_circle_get_property;
   object_class->set_property = uber_renderer_circle_set_property;
   g_type_class_add_private(object_class, sizeof(UberRendererCirclePrivate));

   renderer_class = UBER_RENDERER_CLASS(klass);
   renderer_class->draw = uber_renderer_circle_draw;

   gParamSpecs[PROP_COLOR] =
      g_param_spec_boxed("color",
                         _("Color"),
                         _("The circles color."),
                         GDK_TYPE_RGBA,
                         G_PARAM_READWRITE);
   g_object_class_install_property(object_class, PROP_COLOR,
                                   gParamSpecs[PROP_COLOR]);

   gParamSpecs[PROP_RADIUS] =
      g_param_spec_double("radius",
                          _("Radius"),
                          _("The radius of the circle."),
                          1.0,
                          G_MAXDOUBLE,
                          3.0,
                          G_PARAM_READWRITE);
   g_object_class_install_property(object_class, PROP_RADIUS,
                                   gParamSpecs[PROP_RADIUS]);

   gParamSpecs[PROP_X] =
      g_param_spec_double("x",
                          _("X"),
                          _("The x coordinate."),
                          -G_MINDOUBLE,
                          G_MAXDOUBLE,
                          0.0,
                          G_PARAM_READWRITE);
   g_object_class_install_property(object_class, PROP_X,
                                   gParamSpecs[PROP_X]);

   gParamSpecs[PROP_Y] =
      g_param_spec_double("y",
                          _("Y"),
                          _("The y coordinate."),
                          -G_MINDOUBLE,
                          G_MAXDOUBLE,
                          0.0,
                          G_PARAM_READWRITE);
   g_object_class_install_property(object_class, PROP_Y,
                                   gParamSpecs[PROP_Y]);
}

static void
uber_renderer_circle_init (UberRendererCircle *circle)
{
   circle->priv =
      G_TYPE_INSTANCE_GET_PRIVATE(circle,
                                  UBER_TYPE_RENDERER_CIRCLE,
                                  UberRendererCirclePrivate);
   circle->priv->radius = 3.0;
   gdk_rgba_parse(&circle->priv->color, "#000000");
}
