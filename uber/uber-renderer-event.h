/* uber-renderer-event.h
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

#ifndef UBER_RENDERER_EVENT_H
#define UBER_RENDERER_EVENT_H

#include "uber-model.h"
#include "uber-renderer.h"

G_BEGIN_DECLS

#define UBER_TYPE_RENDERER_EVENT            (uber_renderer_event_get_type())
#define UBER_RENDERER_EVENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_RENDERER_EVENT, UberRendererEvent))
#define UBER_RENDERER_EVENT_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_RENDERER_EVENT, UberRendererEvent const))
#define UBER_RENDERER_EVENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UBER_TYPE_RENDERER_EVENT, UberRendererEventClass))
#define UBER_IS_RENDERER_EVENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBER_TYPE_RENDERER_EVENT))
#define UBER_IS_RENDERER_EVENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UBER_TYPE_RENDERER_EVENT))
#define UBER_RENDERER_EVENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UBER_TYPE_RENDERER_EVENT, UberRendererEventClass))

typedef struct _UberRendererEvent        UberRendererEvent;
typedef struct _UberRendererEventClass   UberRendererEventClass;
typedef struct _UberRendererEventPrivate UberRendererEventPrivate;

struct _UberRendererEvent
{
	GInitiallyUnowned parent;

	/*< private >*/
	UberRendererEventPrivate *priv;
};

struct _UberRendererEventClass
{
	GInitiallyUnownedClass parent_class;
};

GType uber_renderer_event_get_type (void) G_GNUC_CONST;
gint  uber_renderer_event_append   (UberRendererEvent *event,
                                    UberModel         *model,
                                    guint              column);
void  uber_renderer_event_remove   (UberRendererEvent *event,
                                    gint               identifier);

G_END_DECLS

#endif /* UBER_RENDERER_EVENT_H */
