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
#ifndef CAVEGEN_H_
#define CAVEGEN_H_

#include "common.h"

#define HEIGHT 100
#define WIDTH  180

enum tile_type {
    WALL,
    GROUND,
    UP_STAIR,
    DOWN_STAIR,
};

struct map {
    enum tile_type *map;
    struct point    entry_point;

    int width;
    int height;
};

void create_cave(struct map *map);

#endif // CAVEGEN_H_
