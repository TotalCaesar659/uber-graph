/* uber-task.h
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

#ifndef UBER_TASK_H
#define UBER_TASK_H

#include <glib-object.h>

G_BEGIN_DECLS

#define UBER_TYPE_TASK            (uber_task_get_type())
#define UBER_TYPE_TASK_STATE      (uber_task_state_get_type())
#define UBER_TASK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_TASK, UberTask))
#define UBER_TASK_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBER_TYPE_TASK, UberTask const))
#define UBER_TASK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UBER_TYPE_TASK, UberTaskClass))
#define UBER_IS_TASK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBER_TYPE_TASK))
#define UBER_IS_TASK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UBER_TYPE_TASK))
#define UBER_TASK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UBER_TYPE_TASK, UberTaskClass))
#define UBER_TASK_ERROR           (uber_task_error_quark())

typedef struct _UberTask        UberTask;
typedef struct _UberTaskClass   UberTaskClass;
typedef struct _UberTaskPrivate UberTaskPrivate;
typedef enum   _UberTaskState   UberTaskState;
typedef enum   _UberTaskError   UberTaskError;

enum _UberTaskError
{
   UBER_TASK_ERROR_CANCELLED,
};

enum _UberTaskState
{
   UBER_TASK_INITIAL = 0,
   UBER_TASK_RUNNING = 1 << 0,
   UBER_TASK_SUCCESS = 1 << 1,
   UBER_TASK_FAILED  = 1 << 2,
   UBER_TASK_FINISHED_MASK = UBER_TASK_SUCCESS | UBER_TASK_FAILED,
};

struct _UberTask
{
   GInitiallyUnowned parent;

   /*< private >*/
   UberTaskPrivate *priv;
};

struct _UberTaskClass
{
   GInitiallyUnownedClass parent_class;

   void (*run)      (UberTask *task);
   void (*schedule) (UberTask *task);
};

void   uber_task_cancel         (UberTask     *task);
GQuark uber_task_error_quark    (void) G_GNUC_CONST;
GType  uber_task_get_type       (void) G_GNUC_CONST;
void   uber_task_fail           (UberTask     *task,
                                 const GError *error);
void   uber_task_finish         (UberTask     *task);
void   uber_task_run            (UberTask     *task);
void   uber_task_schedule       (UberTask     *task);
GType  uber_task_state_get_type (void) G_GNUC_CONST;
void   uber_task_use_idle       (UberTask     *task,
                                 gboolean      use_idle);

G_END_DECLS

#endif /* UBER_TASK_H */
