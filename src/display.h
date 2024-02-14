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
