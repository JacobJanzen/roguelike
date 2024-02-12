#include "cavegen.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT      100
#define WIDTH       180
#define MAX_STEPS   200
#define NUM_WALKERS 100

enum direction {
    UP,
    LEFT,
    RIGHT,
    DOWN,
    NUM_DIRS,
};

void create_cave(bool **map, struct point **open, int *open_tiles)
{
    srand(time(NULL));
    *map = malloc(sizeof(bool) * HEIGHT * WIDTH);
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            (*map)[i * WIDTH + j] = false;
        }
    }

    int start_x    = WIDTH / 2;
    int start_y    = HEIGHT / 2;
    *open_tiles    = 1;
    *open          = malloc(sizeof(struct point) * HEIGHT * WIDTH);
    struct point p = {.x = start_x, .y = start_y};
    (*open)[0]     = p;

    for (int i = 0; i < NUM_WALKERS; ++i) {
        struct point curr_point = (*open)[rand() % *open_tiles];
        int          x_pos      = curr_point.x;
        int          y_pos      = curr_point.y;

        for (int j = 1; j < MAX_STEPS - 1 && x_pos < WIDTH - 1 && x_pos >= 1 &&
                        y_pos < HEIGHT - 1 && y_pos >= 1;
             ++j) {
            if (!((*map)[y_pos * WIDTH + x_pos])) {
                (*map)[y_pos * WIDTH + x_pos] = true;
                struct point p                = {.x = x_pos, .y = y_pos};
                (*open)[(*open_tiles)++]      = p;
            }
            enum direction dir = rand() % NUM_DIRS;
            switch (dir) {
            case UP : --y_pos; break;
            case LEFT : --x_pos; break;
            case RIGHT : ++x_pos; break;
            case DOWN : ++y_pos; break;
            default : exit(EXIT_FAILURE); // should not occur
            }
        }
    }

    for (int i = 0; i < NUM_WALKERS; ++i) {
        struct point curr_point = (*open)[rand() % *open_tiles];
        int          x_pos      = curr_point.x;
        int          y_pos      = curr_point.y;

        for (int j = 1; j < MAX_STEPS - 1 && x_pos < WIDTH - 1 && x_pos >= 1 &&
                        y_pos < HEIGHT - 1 && y_pos >= 1;
             ++j) {
            if (!((*map)[y_pos * WIDTH + x_pos])) {
                (*map)[y_pos * WIDTH + x_pos] = true;
                struct point p                = {.x = x_pos, .y = y_pos};
                (*open)[(*open_tiles)++]      = p;
            }
            enum direction dir = rand() % NUM_DIRS;
            switch (dir) {
            case UP : --y_pos; break;
            case LEFT : --x_pos; break;
            case RIGHT : ++x_pos; break;
            case DOWN : ++y_pos; break;
            default : exit(EXIT_FAILURE); // should not occur
            }
        }
    }
}
