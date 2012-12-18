/* uber-renderer.c
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

#include "uber-renderer.h"

G_DEFINE_ABSTRACT_TYPE(UberRenderer, uber_renderer, G_TYPE_INITIALLY_UNOWNED)

void
uber_renderer_draw (UberRenderer    *renderer,
                    cairo_t         *cr,
                    GdkRectangle    *area,
                    const UberRange *x_range,
                    const UberRange *y_range)
{
   g_return_if_fail(UBER_IS_RENDERER(renderer));
   g_return_if_fail(cr);
   g_return_if_fail(area);
   g_return_if_fail(x_range);
   g_return_if_fail(y_range);

   UBER_RENDERER_GET_CLASS(renderer)->
      draw(renderer, cr, area, x_range, y_range);
}

static void
uber_renderer_class_init (UberRendererClass *klass)
{
}

static void
uber_renderer_init (UberRenderer *renderer)
{
}
