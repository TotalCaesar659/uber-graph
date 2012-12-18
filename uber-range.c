/* uber-range.c
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

#include "uber-range.h"

struct _UberRange
{
   gdouble min;
   gdouble max;
   gdouble range;
};

UberRange *
uber_range_new (void)
{
   return g_slice_new0(UberRange);
}

UberRange *
uber_range_copy (const UberRange *range)
{
   UberRange *copy;

   g_return_val_if_fail(range, NULL);

   copy = g_slice_new0(UberRange);
   *copy = *range;
   return copy;
}

void
uber_range_free (UberRange *range)
{
   g_return_if_fail(range);
   g_slice_free(UberRange, range);
}

gdouble
uber_range_get_max (const UberRange *range)
{
   return range->max;
}

void
uber_range_set_max (UberRange *range,
                    gdouble    max)
{
   g_return_if_fail(range);
   range->max = max;
   range->range = range->max - range->min;
}

gdouble
uber_range_get_min (const UberRange *range)
{
   return range->min;
}

void
uber_range_set_min (UberRange *range,
                    gdouble    min)
{
   g_return_if_fail(range);
   range->min = min;
   range->range = range->max - range->min;
}

gdouble
uber_range_translate (const UberRange *range,
                      gdouble          value)
{
   g_return_val_if_fail(range, G_MINDOUBLE);

   if (G_LIKELY(range->range != 0.0)) {
      value = CLAMP(value, range->min, range->max);
      return (value - range->min) / range->range;
   }

   return G_MINDOUBLE;
}
