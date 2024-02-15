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
#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdbool.h>

#include "cavegen.h"
#include "entity.h"
#include "ht.h"

#define MAIN_PANEL_WIDTH         100
#define MAIN_PANEL_HEIGHT        41
#define INSTRUCTION_PANEL_WIDTH  32
#define INSTRUCTION_PANEL_HEIGHT 39
#define MESSAGE_PANEL_WIDTH      100
#define MESSAGE_PANEL_HEIGHT     3
#define STATUS_PANEL_WIDTH       32
#define STATUS_PANEL_HEIGHT      5

typedef struct windows display_t;

enum action {
    ACTION_NONE,
    ACTION_EXIT,
    ACTION_DOWN,
    ACTION_UP,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_STAIR_DOWN,
    ACTION_STAIR_UP,
    NUM_ACTIONS,
};

display_t *display_init(void);
void       display_destroy(display_t *disp);

void display_map(display_t *disp, struct map *map, ht_t *entities);
void display_instructions(display_t *disp);
void display_message(display_t *disp, char *msg);
void display_status(display_t *disp, struct entity *entity);

enum action display_process_input(void);

#endif // DISPLAY_H_
