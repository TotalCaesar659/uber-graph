/* uber-renderer-line.h
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

#ifndef UBER_RENDERER_LINE_H
#define UBER_RENDERER_LINE_H

#include <perfkit/perfkit.h>

#include "uber-renderer.h"

G_BEGIN_DECLS

#define UBER_TYPE_RENDERER_LINE            (uber_renderer_line_get_type())
#define UBER_RENDERER_LINE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_RENDERER_LINE, UberRendererLine))
#define UBER_RENDERER_LINE_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_RENDERER_LINE, UberRendererLine const))
#define UBER_RENDERER_LINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UBER_TYPE_RENDERER_LINE, UberRendererLineClass))
#define UBER_IS_RENDERER_LINE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBER_TYPE_RENDERER_LINE))
#define UBER_IS_RENDERER_LINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UBER_TYPE_RENDERER_LINE))
#define UBER_RENDERER_LINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UBER_TYPE_RENDERER_LINE, UberRendererLineClass))

typedef struct _UberRendererLine        UberRendererLine;
typedef struct _UberRendererLineClass   UberRendererLineClass;
typedef struct _UberRendererLinePrivate UberRendererLinePrivate;

struct _UberRendererLine
{
	GInitiallyUnowned parent;

	/*< private >*/
	UberRendererLinePrivate *priv;
};

struct _UberRendererLineClass
{
	GInitiallyUnownedClass parent_class;
};

GType uber_renderer_line_get_type    (void) G_GNUC_CONST;
gint  uber_renderer_line_append      (UberRendererLine *line,
                                     PkModel         *model,
                                     GQuark           key);
void  uber_renderer_line_remove      (UberRendererLine *line,
                                     gint             identifier);
void  uber_renderer_line_set_styling (UberRendererLine *line,
                                     gint             identifier,
                                     const GdkColor  *color,
                                     gdouble          line_width,
                                     gdouble         *dashes,
                                     gint             n_dashes);

G_END_DECLS

#endif /* UBER_RENDERER_LINE_H */
