/*
This file is part of urlg.
urlg is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version. urlg is
distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with urlg. If not, see
<https://www.gnu.org/licenses/>.
*/
#ifndef ENTITY_H_
#define ENTITY_H_

#include <stdbool.h>

#include "common.h"
#include "mapgen.h"

struct entity {
    struct point p;
    char        *disp_ch;
    bool         solid;
    bool         visible;
};

bool entity_set_position(struct entity *e, struct point p, struct map *map);

#endif // ENTITY_H_
