/* uber-range.h
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

#ifndef UBER_RANGE_H
#define UBER_RANGE_H

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _UberRange UberRange;

UberRange *uber_range_copy      (const UberRange *range);
void       uber_range_free      (UberRange       *range);
gdouble    uber_range_get_max   (const UberRange *range);
gdouble    uber_range_get_min   (const UberRange *range);
GType      uber_range_get_type  (void) G_GNUC_CONST;
UberRange *uber_range_new       (void);
void       uber_range_set_max   (UberRange       *range,
                                 gdouble          max);
void       uber_range_set_min   (UberRange       *range,
                                 gdouble          min);
gdouble    uber_range_translate (const UberRange *range,
                                 gdouble          value);

G_END_DECLS

#endif /* UBER_RANGE_H */
