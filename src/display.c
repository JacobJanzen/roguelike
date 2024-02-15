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
#include "display.h"

#include <curses.h>
#include <locale.h>
#include <stdlib.h>

#include "entity.h"

struct windows {
    WINDOW *main;
    WINDOW *inst;
    WINDOW *msgs;
    WINDOW *stat;
};

static WINDOW *create_newwin(int height, int width, int y, int x)
{
    WINDOW *local_win = newwin(height, width, y, x);
    box(local_win, 0, 0);
    wrefresh(local_win);

    return local_win;
}

static display_t *create_windows(void)
{
    display_t *wins = malloc(sizeof(display_t));

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

    return wins;
}

display_t *display_init(void)
{
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
        return NULL;
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

    return create_windows();
}

void display_destroy(display_t *disp)
{
    delwin(disp->main);
    delwin(disp->inst);
    delwin(disp->msgs);
    delwin(disp->stat);

    free(disp);
}

void display_map(display_t *disp, struct map *map, ht_t *entities)
{
    // print map
    struct entity *camera = ht_find(entities, "camera");

    for (int i = 1; i < MAIN_PANEL_HEIGHT - 1; ++i) {
        for (int j = 1; j < MAIN_PANEL_WIDTH - 1; ++j) {
            int map_i = i - 1 + camera->p.y;
            int map_j = j - 1 + camera->p.x;

            if (map_i > map->height || map_j > map->width || map_i < 0 ||
                map_j < 0) {
                mvwaddch(disp->main, i, j, ' ');
            } else {
                switch (map->map[map_i * map->width + map_j]) {
                case GROUND : mvwaddch(disp->main, i, j, '.'); break;
                case UP_STAIR : mvwaddch(disp->main, i, j, '<'); break;
                case DOWN_STAIR :
                    wattron(disp->main, COLOR_PAIR(2));
                    mvwaddch(disp->main, i, j, '>');
                    wattroff(disp->main, COLOR_PAIR(2));
                    break;
                case WALL :
                    if (map_i > 0 &&
                        map->map[(map_i - 1) * map->width + map_j] != WALL) {
                        mvwprintw(disp->main, i, j, "█");
                    } else if (map_i < map->width - 1 && map->map[(map_i + 1) * map->width + map_j] != WALL) {
                        mvwprintw(disp->main, i, j, "█");
                    } else if (map_j > 0 && map->map[map_i * map->width + map_j - 1] != WALL) {
                        mvwprintw(disp->main, i, j, "█");
                    } else if (map_j < map->width - 1 && map->map[map_i * map->width + map_j + 1] != WALL) {
                        mvwprintw(disp->main, i, j, "█");
                    } else {
                        mvwaddch(disp->main, i, j, ' ');
                    }
                    break;
                default : mvwaddch(disp->main, i, j, ' ');
                }
            }
        }
    }

    // print entities
    ht_iter_init(entities);
    struct kvp kvp = ht_iter_next(entities);
    while (kvp.key) {
        struct entity *e = kvp.val;
        if (e->visible) {
            mvwprintw(
                disp->main, e->p.y - camera->p.y + 1, e->p.x - camera->p.x + 1,
                e->disp_ch
            );
        }

        kvp = ht_iter_next(entities);
    }

    wrefresh(disp->main);
}

void display_instructions(display_t *disp)
{
    mvwprintw(disp->inst, 1, 2, "h - move left");
    mvwprintw(disp->inst, 2, 2, "j - move down");
    mvwprintw(disp->inst, 3, 2, "k - move up");
    mvwprintw(disp->inst, 4, 2, "l - move right");
    mvwprintw(disp->inst, 5, 2, "> - move down staircase");
    mvwprintw(disp->inst, 6, 2, "< - exit via staircase");
    wrefresh(disp->inst);
}

void display_message(display_t *disp, char *msg)
{
    for (int i = 1; i < MESSAGE_PANEL_WIDTH - 1; ++i) {
        mvwaddch(disp->msgs, 1, i, ' ');
    }

    mvwprintw(disp->msgs, 1, 1, msg);
    wrefresh(disp->msgs);
}

void display_status(display_t *disp, struct entity *entity)
{
    for (int i = 1; i < STATUS_PANEL_HEIGHT - 1; ++i) {
        for (int j = 1; j < STATUS_PANEL_WIDTH - 1; ++j) {
            mvwaddch(disp->stat, i, j, ' ');
        }
    }

    mvwprintw(disp->stat, 1, 2, "HP:");
    mvwprintw(disp->stat, 2, 2, "STAMINA:");
    mvwprintw(disp->stat, 3, 2, "MANA:");

    wrefresh(disp->stat);
}

enum action display_process_input(void)
{
    int ch = getch();
    switch (ch) {
    case 'k' : return ACTION_UP;
    case 'j' : return ACTION_DOWN;
    case 'h' : return ACTION_LEFT;
    case 'l' : return ACTION_RIGHT;
    case '>' : return ACTION_STAIR_DOWN;
    case '<' : return ACTION_STAIR_UP;
    case KEY_F(1) : return ACTION_EXIT;
    default : return ACTION_NONE;
    }
}
