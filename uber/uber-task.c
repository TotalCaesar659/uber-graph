/* uber-task.c
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

#include "uber-debug.h"
#include "uber-task.h"


G_DEFINE_ABSTRACT_TYPE(UberTask, uber_task, G_TYPE_INITIALLY_UNOWNED)


struct _UberTaskPrivate
{
   UberTaskState state;
   GError *error;
   GMutex *mutex;
   gboolean use_idle;
};


enum
{
   PROP_0,
   PROP_STATE,
   PROP_ERROR,
};


static GThreadPool *threadpool = NULL;


static gboolean
uber_task_emit_notify_state_idle (gpointer data)
{
   UberTask *task = (UberTask *)data;

   g_object_notify(G_OBJECT(task), "state");
   g_object_unref(task);
   return FALSE;
}


static void
uber_task_emit_notify_state (UberTask *task)
{
   if (task->priv->use_idle) {
      g_timeout_add(0, uber_task_emit_notify_state_idle,
                    g_object_ref(task));
   } else {
      g_object_notify(G_OBJECT(task), "state");
   }
}


/**
 * uber_task_cancel:
 * @task: (in): A #UberTask.
 *
 * Cancels a UberTask. If the task is not already running, it will not ever
 * be run.
 */
void
uber_task_cancel (UberTask *task)
{
   UberTaskPrivate *priv;
   gboolean emit = FALSE;

   g_return_if_fail(UBER_IS_TASK(task));

   priv = task->priv;

   g_mutex_lock(priv->mutex);

   switch (priv->state) {
   case UBER_TASK_INITIAL:
   case UBER_TASK_RUNNING:
      priv->state = UBER_TASK_FAILED;
      g_set_error(&priv->error, UBER_TASK_ERROR, UBER_TASK_ERROR_CANCELLED,
                  "The given task was cancelled.");
      emit = TRUE;
      break;
   case UBER_TASK_FAILED:
   case UBER_TASK_SUCCESS:
   case UBER_TASK_FINISHED_MASK:
   default:
      break;
   }

   g_mutex_unlock(priv->mutex);

   if (emit) {
      uber_task_emit_notify_state(task);
   }
}


/**
 * uber_task_fail:
 * @task: (in): A #UberTask.
 * @error: (in): A #GError or %NULL.
 *
 * Completes a task and sets it to failed with an error.
 */
void
uber_task_fail (UberTask      *task,
               const GError *error)
{
   UberTaskPrivate *priv;
   gdouble emit = FALSE;

   g_return_if_fail(UBER_IS_TASK(task));

   priv = task->priv;

   g_mutex_lock(priv->mutex);

   switch (priv->state) {
   case UBER_TASK_INITIAL:
   case UBER_TASK_RUNNING:
      priv->error = g_error_copy(error);
      priv->state = UBER_TASK_FAILED;
      emit = TRUE;
      break;
   case UBER_TASK_FAILED:
   case UBER_TASK_SUCCESS:
      break;
   case UBER_TASK_FINISHED_MASK:
   default:
      g_assert_not_reached();
      return;
   }

   g_mutex_unlock(priv->mutex);

   if (emit) {
      uber_task_emit_notify_state(task);
   }
}


/**
 * uber_task_finish:
 * @task: (in): A #UberTask.
 *
 * Completes the task successfully.
 */
void
uber_task_finish (UberTask *task)
{
   UberTaskPrivate *priv;
   gboolean emit = FALSE;

   g_return_if_fail(UBER_IS_TASK(task));

   priv = task->priv;

   g_mutex_lock(priv->mutex);

   switch (priv->state) {
   case UBER_TASK_INITIAL:
   case UBER_TASK_RUNNING:
      priv->state = UBER_TASK_SUCCESS;
      emit = TRUE;
      break;
   case UBER_TASK_FAILED:
   case UBER_TASK_SUCCESS:
      break;
   case UBER_TASK_FINISHED_MASK:
   default:
      g_assert_not_reached();
      return;
   }

   g_mutex_unlock(priv->mutex);

   if (emit) {
      uber_task_emit_notify_state(task);
   }
}


/**
 * uber_task_run:
 * @task: (in): A #UberTask.
 *
 * Runs the task (synchronously).
 */
void
uber_task_run (UberTask *task)
{
   UberTaskPrivate *priv;
   gboolean run = FALSE;

   task = g_object_ref(task);
   priv = task->priv;

   g_mutex_lock(priv->mutex);

   if (priv->state == UBER_TASK_INITIAL) {
      priv->state = UBER_TASK_RUNNING;
      run = TRUE;
   }

   g_mutex_unlock(priv->mutex);

   if (run) {
      if (UBER_TASK_GET_CLASS(task)->run) {
         UBER_TASK_GET_CLASS(task)->run(task);
      }
   }

   g_object_unref(task);
}


/**
 * uber_task_schedule:
 * @task: (in): A #UberTask.
 *
 * Schedules a task to be run asynchronously. By default, the task is
 * scheduled onto the default threadpool for tasks. The size of the
 * threadpool matches the number of cpus.
 */
void
uber_task_schedule (UberTask *task)
{
   UBER_TASK_GET_CLASS(task)->schedule(task);
}


/**
 * uber_task_real_schedule:
 * @task: (in): A #UberTask.
 *
 * Schedules a task to run on the default threadpool.
 */
static void
uber_task_real_schedule (UberTask *task)
{
   g_thread_pool_push(threadpool, g_object_ref_sink(task), NULL);
}


/**
 * uber_task_use_idle:
 * @task: (in): A #UberTask.
 * @use_idle: (in): If state changes should propagate from main thread.
 *
 * Sets if the task should report state changes from the idle thread.
 */
void
uber_task_use_idle (UberTask  *task,
                   gboolean  use_idle)
{
   g_return_if_fail(UBER_IS_TASK(task));
   task->priv->use_idle = use_idle;
}


static void
uber_task_thread (gpointer data,
                  gpointer user_data)
{
   UberTask *task = (UberTask *)data;
   uber_task_run(task);
   g_object_unref(task);
}


static void
uber_task_finalize (GObject *object)
{
   UberTaskPrivate *priv = UBER_TASK(object)->priv;

   ENTRY;

   g_clear_error(&priv->error);
   g_mutex_free(priv->mutex);

   G_OBJECT_CLASS(uber_task_parent_class)->finalize(object);

   EXIT;
}


static void
uber_task_get_property (GObject    *object,
                       guint       prop_id,
                       GValue     *value,
                       GParamSpec *pspec)
{
   UberTask *task = UBER_TASK(object);

   switch (prop_id) {
   case PROP_ERROR:
      g_value_set_boxed(value, task->priv->error);
      break;
   case PROP_STATE:
      g_value_set_enum(value, task->priv->state);
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   }
}


static void
uber_task_class_init (UberTaskClass *klass)
{
   GObjectClass *object_class;

   object_class = G_OBJECT_CLASS(klass);
   object_class->finalize = uber_task_finalize;
   object_class->get_property = uber_task_get_property;
   g_type_class_add_private(object_class, sizeof(UberTaskPrivate));

   klass->schedule = uber_task_real_schedule;

   g_object_class_install_property(object_class,
                                   PROP_STATE,
                                   g_param_spec_enum("state",
                                                     "state",
                                                     "state",
                                                     UBER_TYPE_TASK_STATE,
                                                     UBER_TASK_INITIAL,
                                                     G_PARAM_READABLE));

   g_object_class_install_property(object_class,
                                   PROP_ERROR,
                                   g_param_spec_boxed("error",
                                                      "error",
                                                      "error",
                                                      G_TYPE_ERROR,
                                                      G_PARAM_READABLE));

   /*
    * Create threadpool for generic tasks. Sized the same as the number
    * of CPUs.
    */
   threadpool = g_thread_pool_new(uber_task_thread, NULL,
                                  uber_get_num_cpus(),
                                  FALSE, NULL);
   if (!threadpool) {
      g_error("Failed to create task threadpool");
      g_assert(FALSE);
   }
}


static void
uber_task_init (UberTask *task)
{
   task->priv = G_TYPE_INSTANCE_GET_PRIVATE(task, UBER_TYPE_TASK, UberTaskPrivate);
   task->priv->mutex = g_mutex_new();
}


GType
uber_task_state_get_type (void)
{
   static GType type_id = 0;
   static const GEnumValue values[] = {
      { UBER_TASK_INITIAL, "UBER_TASK_INITIAL", "INITIAL" },
      { UBER_TASK_RUNNING, "UBER_TASK_RUNNING", "RUNNING" },
      { UBER_TASK_SUCCESS, "UBER_TASK_SUCCESS", "SUCCESS" },
      { UBER_TASK_FAILED,  "UBER_TASK_FAILED",  "FAILED" },
      { 0 }
   };

   if (G_UNLIKELY(!type_id)) {
      type_id = g_enum_register_static("UberTaskState", values);
   }
   return type_id;
}


GQuark
uber_task_error_quark (void)
{
   return g_quark_from_static_string("uber-task-error-quark");
}
