/* uber-model.c
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

#include "uber-model.h"

GType
uber_model_get_type (void)
{
   static gsize initialized;
   static GType type_id;

   if (g_once_init_enter(&initialized)) {
      const GTypeInfo g_type_info = {
         sizeof(UberModelIface),
         NULL, /* base_init */
         NULL, /* base_finalize */
         NULL, /* class_init */
         NULL, /* class_finalize */
         NULL, /* class_data */
         0,    /* instance_size */
         0,    /* n_preallocs */
         NULL, /* instance_init */
         NULL  /* value_vtable */
      };

      type_id = g_type_register_static(G_TYPE_INTERFACE,
                                       "UberModel",
                                       &g_type_info,
                                       0);
      g_type_interface_add_prerequisite(type_id, G_TYPE_OBJECT);
      g_once_init_leave(&initialized, TRUE);
   }

   return type_id;
}
