/* uber-pixring.h
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

#ifndef UBER_PIXRING_H
#define UBER_PIXRING_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define UBER_TYPE_PIXRING            (uber_pixring_get_type())
#define UBER_PIXRING(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_PIXRING, UberPixring))
#define UBER_PIXRING_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_PIXRING, UberPixring const))
#define UBER_PIXRING_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UBER_TYPE_PIXRING, UberPixringClass))
#define UBER_IS_PIXRING(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBER_TYPE_PIXRING))
#define UBER_IS_PIXRING_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UBER_TYPE_PIXRING))
#define UBER_PIXRING_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UBER_TYPE_PIXRING, UberPixringClass))

typedef struct _UberPixring        UberPixring;
typedef struct _UberPixringClass   UberPixringClass;
typedef struct _UberPixringPrivate UberPixringPrivate;

struct _UberPixring
{
   GObject parent;

   /*< private >*/
   UberPixringPrivate *priv;
};

struct _UberPixringClass
{
   GObjectClass parent_class;
};

void             uber_pixring_draw        (UberPixring     *pixring,
                                           cairo_t         *cr);
guint            uber_pixring_get_height  (UberPixring     *pixring);
cairo_surface_t *uber_pixring_get_surface (UberPixring     *pixring);
GType            uber_pixring_get_type    (void) G_GNUC_CONST;
guint            uber_pixring_get_width   (UberPixring     *pixring);
UberPixring     *uber_pixring_new         (cairo_surface_t *surface,
                                           guint            width,
                                           guint            height);
cairo_t         *uber_pixring_push        (UberPixring     *pixring,
                                           guint            width);

G_END_DECLS

#endif /* UBER_PIXRING_H */
