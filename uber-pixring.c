/* uber-pixring.c
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

#include "uber-pixring.h"

G_DEFINE_TYPE(UberPixring, uber_pixring, G_TYPE_OBJECT)

struct _UberPixringPrivate
{
   guint            height;
   gboolean         no_clear;
   guint            offset;
   cairo_surface_t *surface;
   guint            width;
};

enum
{
   PROP_0,
   PROP_HEIGHT,
   PROP_SURFACE,
   PROP_WIDTH,
   LAST_PROP
};

static GParamSpec *gParamSpecs[LAST_PROP];

/**
 * uber_pixring_new:
 * @surface: (in): A #cairo_surface_t.
 * @width: (in): The width of @surface.
 * @height: (in): The height of @surface.
 *
 * Creates a new #UberPixring using @surface for storing rendered
 * data. @width and @height should be the width and height of @surface,
 * respectively.
 *
 * Returns: (transfer full): A new #UberPixring.
 */
UberPixring *
uber_pixring_new (cairo_surface_t *surface,
                  guint            width,
                  guint            height)
{
   return g_object_new(UBER_TYPE_PIXRING,
                       "height", height,
                       "width", width,
                       "surface", surface,
                       NULL);
}

void
uber_pixring_draw (UberPixring *pixring,
                   cairo_t     *cr)
{
   UberPixringPrivate *priv;

   g_return_if_fail(UBER_IS_PIXRING(pixring));
   g_return_if_fail(cr);

   priv = pixring->priv;

   cairo_save(cr);
   cairo_set_source_surface(cr, priv->surface, priv->width - priv->offset, 0);
   cairo_rectangle(cr,
                   priv->width - priv->offset, 0,
                   priv->offset, priv->height);
   cairo_fill(cr);
   cairo_set_source_surface(cr, priv->surface, -(gint)priv->offset, 0);
   cairo_rectangle(cr, 0, 0, priv->width - priv->offset, priv->height);
   cairo_fill(cr);
   cairo_restore(cr);
}

/**
 * uber_pixring_push:
 * @pixring: (in): A #UberPixring.
 * @width: (in): The width to push.
 *
 * Pushes @width pixels onto the #UberPixring. This causes the oldest
 * pixels of @width width to be overwritten.
 *
 * Returns: (transfer full): A #cairo_t to draw to the new region. This should
 *    be freed with cairo_destroy() when you have finished drawing.
 */
cairo_t *
uber_pixring_push (UberPixring *pixring,
                   guint        width)
{
   UberPixringPrivate *priv;
   cairo_t *cr;
   gdouble trx;

   g_return_val_if_fail(UBER_IS_PIXRING(pixring), NULL);
   g_return_val_if_fail(width, NULL);
   g_return_val_if_fail(width < pixring->priv->width, NULL);

   priv = pixring->priv;

   if (!priv->surface) {
      g_warning("Cannot push to pixring, surface missing.");
      return NULL;
   }

   if ((priv->offset + width) > priv->width) {
      /*
       * TODO: What should we do here? We need the equivalent of
       *       a XCopyArea() that can write over itself so that
       *       we end up with the usable portions of the image
       *       pushed to the beginning with enough space for
       *       the new area.
       */
      g_warning("Width is too large for request, clipping.");
      width = priv->width - priv->offset;
   }

   cr = cairo_create(priv->surface);

   /*
    * Clear the newly reclaimed area of the ring buffer pixmap.
    */
   cairo_save(cr);
   cairo_rectangle(cr, priv->offset, 0, width, priv->height);
   cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
   cairo_fill(cr);
   cairo_restore(cr);

   /*
    * Clip the drawable region to the new area.
    */
   cairo_rectangle(cr, priv->offset, 0, width, priv->height);
   cairo_clip(cr);

   /*
    * Translate the coordinate system so that the renderer will end
    * up drawing to the proper location in our ring buffer.
    */
   trx = -(gint)priv->width + (gint)priv->offset + (gint)width;
   cairo_translate(cr, trx, 0.0);

   /*
    * Increment our offset to the new position.
    */
   priv->offset += width;
   if (priv->offset >= priv->width) {
      priv->offset = 0;
   }

   return cr;
}

/**
 * uber_pixring_get_surface:
 * @pixring: (in): A #UberPixring.
 *
 * Gets the "surface" property. The surface is the underlying pixmap
 * data for the #UberPixring.
 *
 * Returns: (transfer none): A #cairo_surface_t.
 */
cairo_surface_t *
uber_pixring_get_surface (UberPixring *pixring)
{
   g_return_val_if_fail(UBER_IS_PIXRING(pixring), NULL);
   return pixring->priv->surface;
}

static void
uber_pixring_set_surface (UberPixring     *pixring,
                          cairo_surface_t *surface)
{
   UberPixringPrivate *priv;

   g_return_if_fail(UBER_IS_PIXRING(pixring));

   priv = pixring->priv;

   if (priv->surface) {
      cairo_surface_destroy(priv->surface);
      priv->surface = NULL;
   }

   if (surface) {
      priv->surface = cairo_surface_reference(surface);
   }

   g_object_notify_by_pspec(G_OBJECT(pixring), gParamSpecs[PROP_SURFACE]);
}

guint
uber_pixring_get_height (UberPixring *pixring)
{
   g_return_val_if_fail(UBER_IS_PIXRING(pixring), 0);
   return pixring->priv->height;
}

static void
uber_pixring_set_height (UberPixring *pixring,
                         guint        height)
{
   g_return_if_fail(UBER_IS_PIXRING(pixring));
   g_return_if_fail(height > 0);

   pixring->priv->height = height;
   g_object_notify_by_pspec(G_OBJECT(pixring), gParamSpecs[PROP_HEIGHT]);
}

guint
uber_pixring_get_width (UberPixring *pixring)
{
   g_return_val_if_fail(UBER_IS_PIXRING(pixring), 0);
   return pixring->priv->width;
}

static void
uber_pixring_set_width (UberPixring *pixring,
                        guint        width)
{
   g_return_if_fail(UBER_IS_PIXRING(pixring));
   g_return_if_fail(width > 0);

   pixring->priv->width = width;
   g_object_notify_by_pspec(G_OBJECT(pixring), gParamSpecs[PROP_WIDTH]);
}

static void
uber_pixring_finalize (GObject *object)
{
   UberPixringPrivate *priv;

   priv = UBER_PIXRING(object)->priv;

   if (priv->surface) {
      cairo_surface_destroy(priv->surface);
      priv->surface = NULL;
   }

   G_OBJECT_CLASS(uber_pixring_parent_class)->finalize(object);
}

static void
uber_pixring_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
   UberPixring *pixring = UBER_PIXRING(object);

   switch (prop_id) {
   case PROP_HEIGHT:
      g_value_set_uint(value, uber_pixring_get_height(pixring));
      break;
   case PROP_SURFACE:
      g_value_set_pointer(value, uber_pixring_get_surface(pixring));
      break;
   case PROP_WIDTH:
      g_value_set_uint(value, uber_pixring_get_width(pixring));
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}

static void
uber_pixring_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
   UberPixring *pixring = UBER_PIXRING(object);

   switch (prop_id) {
   case PROP_HEIGHT:
      uber_pixring_set_height(pixring, g_value_get_uint(value));
      break;
   case PROP_SURFACE:
      uber_pixring_set_surface(pixring, g_value_get_pointer(value));
      break;
   case PROP_WIDTH:
      uber_pixring_set_width(pixring, g_value_get_uint(value));
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}

static void
uber_pixring_class_init (UberPixringClass *klass)
{
   GObjectClass *object_class;

   object_class = G_OBJECT_CLASS(klass);
   object_class->finalize = uber_pixring_finalize;
   object_class->get_property = uber_pixring_get_property;
   object_class->set_property = uber_pixring_set_property;
   g_type_class_add_private(object_class, sizeof(UberPixringPrivate));

   gParamSpecs[PROP_SURFACE] =
      g_param_spec_pointer("surface",
                           _("Surface"),
                           _("The cairo surface."),
                           G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
   g_object_class_install_property(object_class, PROP_SURFACE,
                                   gParamSpecs[PROP_SURFACE]);

   gParamSpecs[PROP_HEIGHT] =
      g_param_spec_uint("height",
                          _("Height"),
                          _("The height of the pixring."),
                          1,
                          G_MAXUINT,
                          1,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
   g_object_class_install_property(object_class, PROP_HEIGHT,
                                   gParamSpecs[PROP_HEIGHT]);

   gParamSpecs[PROP_WIDTH] =
      g_param_spec_uint("width",
                        _("Width"),
                        _("The width of the pixring."),
                        1,
                        G_MAXUINT,
                        1,
                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
   g_object_class_install_property(object_class, PROP_WIDTH,
                                   gParamSpecs[PROP_WIDTH]);
}

static void
uber_pixring_init (UberPixring *pixring)
{
   pixring->priv =
      G_TYPE_INSTANCE_GET_PRIVATE(pixring,
                                  UBER_TYPE_PIXRING,
                                  UberPixringPrivate);
   pixring->priv->no_clear = FALSE;
}
