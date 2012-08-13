/* uber-task-render.h
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

#ifndef UBER_TASK_RENDER_H
#define UBER_TASK_RENDER_H

#include "uber-task.h"

G_BEGIN_DECLS

#define UBER_TYPE_TASK_RENDER            (uber_task_render_get_type())
#define UBER_TASK_RENDER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_TASK_RENDER, UberTaskRender))
#define UBER_TASK_RENDER_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_TASK_RENDER, UberTaskRender const))
#define UBER_TASK_RENDER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UBER_TYPE_TASK_RENDER, UberTaskRenderClass))
#define UBER_IS_TASK_RENDER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBER_TYPE_TASK_RENDER))
#define UBER_IS_TASK_RENDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UBER_TYPE_TASK_RENDER))
#define UBER_TASK_RENDER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UBER_TYPE_TASK_RENDER, UberTaskRenderClass))

typedef struct _UberTaskRender        UberTaskRender;
typedef struct _UberTaskRenderClass   UberTaskRenderClass;
typedef struct _UberTaskRenderPrivate UberTaskRenderPrivate;

struct _UberTaskRender
{
	UberTask parent;

	/*< private >*/
	UberTaskRenderPrivate *priv;
};

struct _UberTaskRenderClass
{
	UberTaskClass parent_class;

	void (*render) (UberTaskRender *render);
};

GType uber_task_render_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* UBER_TASK_RENDER_H */
