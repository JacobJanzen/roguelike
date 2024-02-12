#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>

#include "cavegen.h"

#define MAIN_PANEL_WIDTH         100
#define MAIN_PANEL_HEIGHT        41
#define INSTRUCTION_PANEL_WIDTH  32
#define INSTRUCTION_PANEL_HEIGHT 39
#define MESSAGE_PANEL_WIDTH      100
#define MESSAGE_PANEL_HEIGHT     3
#define STATUS_PANEL_WIDTH       32
#define STATUS_PANEL_HEIGHT      5

#define MAX_ENTITIES 100

struct entity {
    char        *name;
    struct point p;
    char        *disp_ch;
    bool         solid;
    bool         visible;
};

struct windows {
    WINDOW *main;
    WINDOW *inst;
    WINDOW *msgs;
    WINDOW *stat;
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
    if (LINES < MAIN_PANEL_HEIGHT + MESSAGE_PANEL_HEIGHT ||
        COLS < MAIN_PANEL_WIDTH + INSTRUCTION_PANEL_WIDTH || !has_colors()) {
        endwin();
        fprintf(
            stderr,
            "a color terminal is required with at least %dx%d characters\n",
            INSTRUCTION_PANEL_WIDTH + MAIN_PANEL_WIDTH,
            MAIN_PANEL_HEIGHT + MESSAGE_PANEL_HEIGHT
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
    init_pair(2, COLOR_BLACK, COLOR_RED);
    wattron(stdscr, COLOR_PAIR(1));
    refresh();
}

void display_map(
    WINDOW *win, struct map *map, struct entity *entities, int num_entities
)
{
    // print map
    for (int i = 1; i < MAIN_PANEL_HEIGHT - 1; ++i) {
        for (int j = 1; j < MAIN_PANEL_WIDTH - 1; ++j) {
            int map_i = i - 1 + entities[0].p.y;
            int map_j = j - 1 + entities[0].p.x;

            if (map_i > map->height || map_j > map->width || map_i < 0 ||
                map_j < 0) {
                mvwaddch(win, i, j, ' ');
            } else {
                switch (map->map[map_i * map->width + map_j]) {
                case GROUND : mvwaddch(win, i, j, '.'); break;
                case UP_STAIR : mvwaddch(win, i, j, '<'); break;
                case DOWN_STAIR :
                    wattron(win, COLOR_PAIR(2));
                    mvwaddch(win, i, j, '>');
                    wattroff(win, COLOR_PAIR(2));
                    break;
                case WALL :
                    if (map_i > 0 &&
                        map->map[(map_i - 1) * map->width + map_j] != WALL) {
                        mvwprintw(win, i, j, "█");
                    } else if (map_i < map->width - 1 && map->map[(map_i + 1) * map->width + map_j] != WALL) {
                        mvwprintw(win, i, j, "█");
                    } else if (map_j > 0 && map->map[map_i * map->width + map_j - 1] != WALL) {
                        mvwprintw(win, i, j, "█");
                    } else if (map_j < map->width - 1 && map->map[map_i * map->width + map_j + 1] != WALL) {
                        mvwprintw(win, i, j, "█");
                    } else {
                        mvwaddch(win, i, j, ' ');
                    }
                    break;
                default : mvwaddch(win, i, j, ' ');
                }
            }
        }
    }

    // print entities
    for (int i = 1; i < num_entities; ++i) {
        if (entities[i].visible) {
            mvwprintw(
                win, entities[i].p.y - entities[0].p.y + 1,
                entities[i].p.x - entities[0].p.x + 1, entities[i].disp_ch
            );
        }
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

void display_status(WINDOW *win, struct entity *entity)
{
    for (int i = 1; i < STATUS_PANEL_HEIGHT - 1; ++i) {
        for (int j = 1; j < STATUS_PANEL_WIDTH - 1; ++j) {
            mvwaddch(win, i, j, ' ');
        }
    }

    mvwprintw(win, 1, 2, "HP:");
    mvwprintw(win, 2, 2, "STAMINA:");
    mvwprintw(win, 3, 2, "MANA:");

    wrefresh(win);
}

bool entity_set_pos(struct entity *e, struct point p, struct map *map)
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

void create_windows(struct windows *wins)
{
    wins->inst = create_newwin(
        INSTRUCTION_PANEL_HEIGHT, INSTRUCTION_PANEL_WIDTH,
        (LINES - INSTRUCTION_PANEL_HEIGHT - STATUS_PANEL_HEIGHT) / 2 + 1,
        (COLS - MAIN_PANEL_WIDTH - INSTRUCTION_PANEL_WIDTH) / 2 +
            MAIN_PANEL_WIDTH - 1
    );
    wins->msgs = create_newwin(
        MESSAGE_PANEL_HEIGHT, MESSAGE_PANEL_WIDTH,
        (LINES - MAIN_PANEL_HEIGHT - MESSAGE_PANEL_HEIGHT) / 2 +
            MAIN_PANEL_HEIGHT,
        (COLS - MESSAGE_PANEL_WIDTH - INSTRUCTION_PANEL_WIDTH) / 2
    );
    wins->stat = create_newwin(
        STATUS_PANEL_HEIGHT, STATUS_PANEL_WIDTH,
        (LINES - INSTRUCTION_PANEL_HEIGHT - STATUS_PANEL_HEIGHT) / 2 +
            INSTRUCTION_PANEL_HEIGHT,
        (COLS - MAIN_PANEL_WIDTH - INSTRUCTION_PANEL_WIDTH) / 2 +
            MAIN_PANEL_WIDTH - 1
    );
    wins->main = create_newwin(
        MAIN_PANEL_HEIGHT, MAIN_PANEL_WIDTH,
        (LINES - MAIN_PANEL_HEIGHT - MESSAGE_PANEL_HEIGHT) / 2 + 1,
        (COLS - MAIN_PANEL_WIDTH - INSTRUCTION_PANEL_WIDTH) / 2
    );
}

int main(void)
{
    initialize();

    // create windows
    struct windows windows;
    create_windows(&windows);

    // create the map
    struct map map;
    create_cave(&map);

    // create the camera and player at the up stairs
    struct entity *entities = malloc(sizeof(struct entity) * MAX_ENTITIES);
    entities[0].disp_ch     = "";
    entities[0].name        = "camera";
    entities[0].p.x         = map.entry_point.x - MAIN_PANEL_WIDTH / 2 + 1;
    entities[0].p.y         = map.entry_point.y - MAIN_PANEL_HEIGHT / 2 + 1;
    entities[0].solid       = false;
    entities[0].visible     = false;
    entities[1].disp_ch     = "@";
    entities[1].name        = "player";
    entities[1].p           = map.entry_point;
    entities[1].solid       = true;
    entities[1].visible     = true;
    int num_entities        = 2;

    // start displaying things
    display_map(windows.main, &map, entities, num_entities);
    display_instructions(windows.inst);
    display_status(windows.stat, &entities[1]);
    display_message(windows.msgs, "");

    int  ch;
    bool done = false;
    while (!done && (ch = getch()) != KEY_F(1)) {
        struct point newp     = entities[1].p;
        struct point newp_cam = entities[0].p;
        switch (ch) {
        case 'k' :
            --newp.y;
            --newp_cam.y;
            break;
        case 'j' :
            ++newp.y;
            ++newp_cam.y;
            break;
        case 'h' :
            --newp.x;
            --newp_cam.x;
            break;
        case 'l' :
            ++newp.x;
            ++newp_cam.x;
            break;
        case '>' :
            if (map.map[entities[1].p.y * map.width + entities[1].p.x] ==
                DOWN_STAIR) {
                free(map.map);
                create_cave(&map);

                newp       = map.entry_point;
                newp_cam.x = map.entry_point.x - MAIN_PANEL_WIDTH / 2 + 1;
                newp_cam.y = map.entry_point.y - MAIN_PANEL_HEIGHT / 2 + 1;

                display_message(windows.msgs, "Entered new level");
            }
            break;
        case '<' :
            if (map.map[entities[1].p.y * WIDTH + entities[1].p.x] ==
                UP_STAIR) {
                done = true;
            }
            break;
        }

        if (entity_set_pos(&entities[1], newp, &map))
            entity_set_pos(&entities[0], newp_cam, &map);

        display_map(windows.main, &map, entities, num_entities);
    }

    free(map.map);
    free(entities);

    endwin();

    return 0;
}
