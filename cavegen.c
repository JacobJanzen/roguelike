#include "cavegen.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_STEPS   200
#define NUM_WALKERS 100

enum direction {
    UP,
    LEFT,
    RIGHT,
    DOWN,
    NUM_DIRS,
};

void create_cave(struct map *map)
{
    map->width  = WIDTH;
    map->height = HEIGHT;

    // create a map consisting entirely of walls
    map->map = malloc(sizeof(enum tile_type) * HEIGHT * WIDTH);
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            map->map[i * WIDTH + j] = WALL;
        }
    }

    // start in the middle of the screen
    int start_x = WIDTH / 2;
    int start_y = HEIGHT / 2;

    // make the starting point GROUND
    map->map[start_y * WIDTH + start_x] = GROUND;

    // setup the open tiles
    struct point *open_tiles = malloc(sizeof(struct point) * HEIGHT * WIDTH);

    int num_open_tiles = 1;
    open_tiles[0].x    = start_x;
    open_tiles[0].y    = start_y;

    for (int i = 0; i < NUM_WALKERS; ++i) {
        // get a random open point
        struct point curr_point = open_tiles[rand() % num_open_tiles];

        int x_pos = curr_point.x;
        int y_pos = curr_point.y;

        // iterate until the walk exits the array or MAX_STEPS is reached
        for (int j = 1; j < MAX_STEPS - 1 && x_pos < WIDTH - 1 && x_pos >= 1 &&
                        y_pos < HEIGHT - 1 && y_pos >= 1;
             ++j) {
            // add new open point if the current point is still a wall
            if (map->map[y_pos * WIDTH + x_pos] == WALL) {
                open_tiles[num_open_tiles].x = x_pos;
                open_tiles[num_open_tiles].y = y_pos;
                ++num_open_tiles;
            }

            map->map[y_pos * WIDTH + x_pos] = GROUND; // assign ground

            // move in a random direction
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

    // assign the up stair and remove from open tiles
    int in           = rand() % num_open_tiles;
    map->entry_point = open_tiles[in];
    open_tiles[in]   = open_tiles[num_open_tiles - 1];
    --num_open_tiles;
    map->map[map->entry_point.y * WIDTH + map->entry_point.x] = UP_STAIR;

    // assign the down stair and remove from open tiles
    in                = rand() % num_open_tiles;
    struct point down = open_tiles[in];
    open_tiles[in]    = open_tiles[num_open_tiles - 1];
    --num_open_tiles;
    map->map[down.y * WIDTH + down.x] = DOWN_STAIR;

    free(open_tiles);
}
