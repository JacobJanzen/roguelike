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
