#include <curses.h>
#include <locale.h>
#include <stdlib.h>

#include "cavegen.h"

#define MAIN_PANEL_WIDTH         100
#define MAIN_PANEL_HEIGHT        41
#define INSTRUCTION_PANEL_WIDTH  32
#define INSTRUCTION_PANEL_HEIGHT 43
#define MESSAGE_PANEL_WIDTH      100
#define MESSAGE_PANEL_HEIGHT     3

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);

    return local_win;
}

void initialize(void)
{
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

void display_map(WINDOW *win, bool *map, int cam_y, int cam_x)
{
    for (int i = 1; i < MAIN_PANEL_HEIGHT - 1; ++i) {
        for (int j = 1; j < MAIN_PANEL_WIDTH - 1; ++j) {
            int i1 = i - 1 + cam_y;
            int j1 = j - 1 + cam_x;
            if (i1 > HEIGHT || j1 > WIDTH || i1 < 0 || j1 < 0) {
                mvwaddch(win, i, j, ' ');
            } else if (map[i1 * WIDTH + j1]) {
                mvwaddch(win, i, j, '.');
            } else if (i1 > 0 && map[(i1 - 1) * WIDTH + j1]) {
                mvwprintw(win, i, j, "█");
            } else if (i1 < HEIGHT - 1 && map[(i1 + 1) * WIDTH + j1]) {
                mvwprintw(win, i, j, "█");
            } else if (j1 > 0 && map[i1 * WIDTH + j1 - 1]) {
                mvwprintw(win, i, j, "█");
            } else if (j1 < WIDTH - 1 && map[i1 * WIDTH + j1 + 1]) {
                mvwprintw(win, i, j, "█");
            } else {
                mvwaddch(win, i, j, ' ');
            }
        }
    }
    mvwaddch(win, MAIN_PANEL_HEIGHT / 2, MAIN_PANEL_WIDTH / 2, '@');
    wrefresh(win);
}

int main(void)
{
    int starty = 12;
    int startx = 40;

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

    mvwprintw(inst, 1, 2, "h - move left");
    mvwprintw(inst, 2, 2, "j - move down");
    mvwprintw(inst, 3, 2, "k - move up");
    mvwprintw(inst, 4, 2, "l - move right");
    wrefresh(inst);

    mvwprintw(msgs, 1, 1, "TODO: put messages here");
    wrefresh(msgs);

    wattron(main_win, COLOR_PAIR(1));
    wrefresh(main_win);

    bool         *map;
    struct point *open_tiles;
    int           num_open_tiles;
    create_cave(&map, &open_tiles, &num_open_tiles);

    display_map(main_win, map, starty, startx);

    int ch;
    while ((ch = getch()) != KEY_F(1)) {
        int xpos = startx + MAIN_PANEL_WIDTH / 2 - 1;
        int ypos = starty + MAIN_PANEL_HEIGHT / 2 - 1;
        switch (ch) {
        case 'k' :
            if (ypos > 0) {
                if (map[(ypos - 1) * WIDTH + xpos])
                    --starty;
            }
            break;
        case 'j' :
            if (ypos < HEIGHT - 1) {
                if (map[(ypos + 1) * WIDTH + xpos])
                    ++starty;
            }
            break;
        case 'h' :
            if (xpos > 0) {
                if (map[ypos * WIDTH + xpos - 1])
                    --startx;
            }
            break;
        case 'l' :
            if (xpos < WIDTH - 1) {
                if (map[ypos * WIDTH + xpos + 1])
                    ++startx;
            }
            break;
        }
        display_map(main_win, map, starty, startx);
    }

    endwin();

    return 0;
}
