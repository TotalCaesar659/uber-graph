/* uber-model.h
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

#ifndef UBER_MODEL_H
#define UBER_MODEL_H

#include <glib-object.h>

G_BEGIN_DECLS

#define UBER_TYPE_MODEL             (uber_model_get_type())
#define UBER_MODEL(o)               (G_TYPE_CHECK_INSTANCE_CAST((o),    UBER_TYPE_MODEL, UberModel))
#define UBER_IS_MODEL(o)            (G_TYPE_CHECK_INSTANCE_TYPE((o),    UBER_TYPE_MODEL))
#define UBER_MODEL_GET_INTERFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE((o), UBER_TYPE_MODEL, UberModelIface))

typedef struct _UberModel      UberModel;
typedef struct _UberModelIface UberModelIface;

struct _UberModelIface
{
   GTypeInterface parent;

   /* interface methods */
};

GType uber_model_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* UBER_MODEL_H */
