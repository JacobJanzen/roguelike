#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>

#include "cavegen.h"

#define MAIN_PANEL_WIDTH         100
#define MAIN_PANEL_HEIGHT        41
#define INSTRUCTION_PANEL_WIDTH  32
#define INSTRUCTION_PANEL_HEIGHT 43
#define MESSAGE_PANEL_WIDTH      100
#define MESSAGE_PANEL_HEIGHT     3

#define MAX_ENTITIES 100

struct entity {
    char *name;
    int   xpos;
    int   ypos;
    char *disp_ch;
};

WINDOW *create_newwin(int height, int width, int starty, int startx)

{
    WINDOW *local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);

    return local_win;
}

void initialize(void)
{
    srand(time(NULL));
    setlocale(LC_ALL, ""); // allow extended ASCII

    initscr(); // initialize curses

    // exit on unsupported consoles
    if (LINES < INSTRUCTION_PANEL_HEIGHT ||
        COLS < MAIN_PANEL_WIDTH + INSTRUCTION_PANEL_WIDTH || !has_colors()) {
        endwin();
        fprintf(
            stderr,
            "a color terminal is required with at least %dx%d characters\n",
            INSTRUCTION_PANEL_WIDTH + MAIN_PANEL_WIDTH, INSTRUCTION_PANEL_HEIGHT
        );
        exit(1);
    }

    // configure curses if startup was successful
    raw();                // disable line buffering
    keypad(stdscr, TRUE); // enable reading function keys
    noecho();             // don't print input
    curs_set(0);          // disable the cursor
    start_color();        // enable colours

    // setup colours
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    wattron(stdscr, COLOR_PAIR(1));
    refresh();
}

void display_map(
    WINDOW *win, enum tile_type *map, struct entity *entities, int num_entities
)
{
    for (int i = 1; i < MAIN_PANEL_HEIGHT - 1; ++i) {
        for (int j = 1; j < MAIN_PANEL_WIDTH - 1; ++j) {
            int i1 = i - 1 + entities[0].ypos;
            int j1 = j - 1 + entities[0].xpos;
            if (i1 > HEIGHT || j1 > WIDTH || i1 < 0 || j1 < 0) {
                mvwaddch(win, i, j, ' ');
            } else if (map[i1 * WIDTH + j1] == GROUND) {
                mvwaddch(win, i, j, '.');
            } else if (map[i1 * WIDTH + j1] == UP_STAIR) {
                mvwaddch(win, i, j, '<');
            } else if (map[i1 * WIDTH + j1] == DOWN_STAIR) {
                mvwaddch(win, i, j, '>');
            } else if (i1 > 0 && map[(i1 - 1) * WIDTH + j1] != WALL) {
                mvwprintw(win, i, j, "█");
            } else if (i1 < HEIGHT - 1 && map[(i1 + 1) * WIDTH + j1] != WALL) {
                mvwprintw(win, i, j, "█");
            } else if (j1 > 0 && map[i1 * WIDTH + j1 - 1] != WALL) {
                mvwprintw(win, i, j, "█");
            } else if (j1 < WIDTH - 1 && map[i1 * WIDTH + j1 + 1] != WALL) {
                mvwprintw(win, i, j, "█");
            } else {
                mvwaddch(win, i, j, ' ');
            }
        }
    }

    for (int i = 1; i < num_entities; ++i) {
        mvwprintw(
            win, entities[i].ypos - entities[0].ypos + 1,
            entities[i].xpos - entities[0].xpos + 1, entities[i].disp_ch
        );
    }
    wrefresh(win);
}

void display_instructions(WINDOW *win)
{
    mvwprintw(win, 1, 2, "h - move left");
    mvwprintw(win, 2, 2, "j - move down");
    mvwprintw(win, 3, 2, "k - move up");
    mvwprintw(win, 4, 2, "l - move right");
    mvwprintw(win, 5, 2, "> - move down staircase");
    mvwprintw(win, 6, 2, "< - exit via staircase");
    wrefresh(win);
}

void display_message(WINDOW *win, char *msg)
{
    for (int i = 1; i < MESSAGE_PANEL_WIDTH - 1; ++i) {
        mvwaddch(win, 1, i, ' ');
    }

    mvwprintw(win, 1, 1, msg);
    wrefresh(win);
}

int main(void)
{
    initialize();

    // create the windows
    WINDOW *inst = create_newwin(
        INSTRUCTION_PANEL_HEIGHT, INSTRUCTION_PANEL_WIDTH,
        (LINES - INSTRUCTION_PANEL_HEIGHT) / 2,
        (COLS - MAIN_PANEL_WIDTH - INSTRUCTION_PANEL_WIDTH) / 2 +
            MAIN_PANEL_WIDTH - 1
    );
    WINDOW *msgs = create_newwin(
        MESSAGE_PANEL_HEIGHT, MESSAGE_PANEL_WIDTH,
        (LINES - MAIN_PANEL_HEIGHT - MESSAGE_PANEL_HEIGHT) / 2 +
            MAIN_PANEL_HEIGHT,
        (COLS - MESSAGE_PANEL_WIDTH - INSTRUCTION_PANEL_WIDTH) / 2
    );
    WINDOW *main_win = create_newwin(
        MAIN_PANEL_HEIGHT, MAIN_PANEL_WIDTH,
        (LINES - MAIN_PANEL_HEIGHT - MESSAGE_PANEL_HEIGHT) / 2 + 1,
        (COLS - MAIN_PANEL_WIDTH - INSTRUCTION_PANEL_WIDTH) / 2
    );

    // create the map
    enum tile_type *map;
    struct point   *open_tiles;
    int             num_open_tiles;
    struct point    up;
    struct point    down;
    create_cave(&map, &open_tiles, &num_open_tiles, &up, &down);

    // create the camera and player at the up stairs
    struct entity *entities = malloc(sizeof(struct entity) * MAX_ENTITIES);
    entities[0].disp_ch     = "";
    entities[0].name        = "camera";
    entities[0].xpos        = up.x - MAIN_PANEL_WIDTH / 2 + 1;
    entities[0].ypos        = up.y - MAIN_PANEL_HEIGHT / 2 + 1;
    entities[1].disp_ch     = "@";
    entities[1].name        = "player";
    entities[1].xpos        = up.x;
    entities[1].ypos        = up.y;
    int num_entities        = 2;

    // start displaying things
    display_map(main_win, map, entities, num_entities);
    display_instructions(inst);
    display_message(msgs, "TODO: put a message here");

    int  ch;
    bool done = false;
    while (!done && (ch = getch()) != KEY_F(1)) {
        switch (ch) {
        case 'k' :
            if (entities[1].ypos > 0) {
                if (map[(entities[1].ypos - 1) * WIDTH + entities[1].xpos]) {
                    --entities[0].ypos;
                    --entities[1].ypos;
                }
            }
            break;
        case 'j' :
            if (entities[1].ypos < HEIGHT - 1) {
                if (map[(entities[1].ypos + 1) * WIDTH + entities[1].xpos]) {
                    ++entities[0].ypos;
                    ++entities[1].ypos;
                }
            }
            break;
        case 'h' :
            if (entities[1].xpos > 0) {
                if (map[entities[1].ypos * WIDTH + entities[1].xpos - 1]) {
                    --entities[0].xpos;
                    --entities[1].xpos;
                }
            }
            break;
        case 'l' :
            if (entities[1].xpos < WIDTH - 1) {
                if (map[entities[1].ypos * WIDTH + entities[1].xpos + 1]) {
                    ++entities[0].xpos;
                    ++entities[1].xpos;
                }
            }
            break;
        case '>' :
            if (map[entities[1].ypos * WIDTH + entities[1].xpos] ==
                DOWN_STAIR) {
                free(map);
                free(open_tiles);
                create_cave(&map, &open_tiles, &num_open_tiles, &up, &down);
                entities[0].xpos = up.x - MAIN_PANEL_WIDTH / 2 + 1;
                entities[0].ypos = up.y - MAIN_PANEL_HEIGHT / 2 + 1;
                entities[1].xpos = up.x;
                entities[1].ypos = up.y;
            }
            break;
        case '<' :
            if (map[entities[1].ypos * WIDTH + entities[1].xpos] == UP_STAIR) {
                done = true;
            }
            break;
        }
        display_map(main_win, map, entities, num_entities);
    }

    free(map);
    free(open_tiles);
    free(entities);

    endwin();

    return 0;
}
