#ifndef CAVEGEN_H_
#define CAVEGEN_H_

#include <stdbool.h>

#define HEIGHT      100
#define WIDTH       180
#define MAX_STEPS   200
#define NUM_WALKERS 100

struct point {
    int x;
    int y;
};

void create_cave(bool **map, struct point **open, int *open_tiles);

#endif // CAVEGEN_H_
