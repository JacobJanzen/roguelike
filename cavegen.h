#ifndef CAVEGEN_H_
#define CAVEGEN_H_

#include <stdbool.h>

#define HEIGHT 100
#define WIDTH  180

enum tile_type {
    WALL,
    GROUND,
    UP_STAIR,
    DOWN_STAIR,
};

struct point {
    int x;
    int y;
};

void create_cave(
    enum tile_type **map, struct point **open, int *open_tiles,
    struct point *up, struct point *down
);

#endif // CAVEGEN_H_
