#include <curses.h>
#include <stdlib.h>

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

void destroy_win(WINDOW *local_win)
{
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(local_win);
    delwin(local_win);
}

int main(void)
{
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

    int starty = 12;
    int startx = 40;
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    wattron(stdscr, COLOR_PAIR(1));
    refresh();

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
    mvwaddch(main_win, starty, startx, '@');
    wrefresh(main_win);

    int ch;
    while ((ch = getch()) != KEY_F(1)) {
        switch (ch) {
        case 'k' :
            if (starty > 1) {
                mvwaddch(main_win, starty, startx, ' ');
                mvwaddch(main_win, --starty, startx, '@');
            }
            break;
        case 'j' :
            if (starty < MAIN_PANEL_HEIGHT - 2) {
                mvwaddch(main_win, starty, startx, ' ');
                mvwaddch(main_win, ++starty, startx, '@');
            }
            break;
        case 'h' :
            if (startx > 1) {
                mvwaddch(main_win, starty, startx, ' ');
                mvwaddch(main_win, starty, --startx, '@');
            }
            break;
        case 'l' :
            if (startx < MAIN_PANEL_WIDTH - 2) {
                mvwaddch(main_win, starty, startx, ' ');
                mvwaddch(main_win, starty, ++startx, '@');
            }
            break;
        }
        wrefresh(main_win);
    }

    endwin();

    return 0;
}
