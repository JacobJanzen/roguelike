/*
This file is part of urlg.
urlg is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version. urlg is
distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with Foobar. If not, see
<https://www.gnu.org/licenses/>.
*/
#include "entity.h"

bool entity_set_position(struct entity *e, struct point p, struct map *map)
{
    if (e->solid) {
        if (p.y < 0 || p.x < 0 || p.y >= map->height || p.x >= map->width) {
            return false;
        }
        if (map->map[p.y * map->width + p.x] == WALL) {
            return false;
        }
    }

    e->p = p;

    return true;
}
