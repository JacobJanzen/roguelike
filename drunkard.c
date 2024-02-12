#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT      50
#define WIDTH       50
#define MAX_STEPS   50
#define NUM_WALKERS 100

enum direction {
    UP,
    LEFT,
    RIGHT,
    DOWN,
    NUM_DIRS,
};

struct point {
    int x;
    int y;
};

int main(void)
{
    bool map[HEIGHT][WIDTH];
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            map[i][j] = false;
        }
    }

    int start_x = WIDTH / 2;
    int start_y = HEIGHT / 2;

    int          open_tiles = 1;
    struct point open[HEIGHT * WIDTH];
    struct point p = {.x = start_x, .y = start_y};
    open[0]        = p;

    srand(time(NULL));

    for (int i = 0; i < NUM_WALKERS; ++i) {
        struct point curr_point = open[rand() % open_tiles];
        int          x_pos      = curr_point.x;
        int          y_pos      = curr_point.y;

        for (int j = 0; j < MAX_STEPS && x_pos < WIDTH && x_pos >= 0 &&
                        y_pos < HEIGHT && y_pos >= 0;
             ++j) {
            if (!map[y_pos][x_pos]) {
                map[y_pos][x_pos]  = true;
                struct point p     = {.x = x_pos, .y = y_pos};
                open[open_tiles++] = p;
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

    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (map[i][j]) {
                printf("#");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}
