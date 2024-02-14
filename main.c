#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>

#include "cavegen.h"
#include "common.h"
#include "display.h"
#include "entity.h"
#include "ht.h"

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

bool game_update(
    display_t *disp, enum action action, ht_t *entities, struct map *map
)
{
    struct entity *player = ht_find(entities, "player");
    struct entity *camera = ht_find(entities, "camera");

    struct point newp     = player->p;
    struct point newp_cam = camera->p;
    switch (action) {
    case ACTION_EXIT : return true;
    case ACTION_UP :
        --newp.y;
        --newp_cam.y;
        display_message(disp, "moving up");
        break;
    case ACTION_DOWN :
        ++newp.y;
        ++newp_cam.y;
        display_message(disp, "moving down");
        break;
    case ACTION_LEFT :
        --newp.x;
        --newp_cam.x;
        display_message(disp, "moving left");
        break;
    case ACTION_RIGHT :
        ++newp.x;
        ++newp_cam.x;
        display_message(disp, "moving right");
        break;
    case ACTION_STAIR_DOWN :
        if (map->map[player->p.y * map->width + player->p.x] == DOWN_STAIR) {
            free(map->map);
            create_cave(map);

            newp       = map->entry_point;
            newp_cam.x = map->entry_point.x - MAIN_PANEL_WIDTH / 2 + 1;
            newp_cam.y = map->entry_point.y - MAIN_PANEL_HEIGHT / 2 + 1;
            display_message(disp, "moving down stairs");
        } else {
            display_message(disp, "no stairs to go down");
        }
        break;
    case ACTION_STAIR_UP :
        if (map->map[player->p.y * WIDTH + player->p.x] == UP_STAIR) {
            display_message(disp, "moving up stairs");
            return true;
        } else {
            display_message(disp, "no stairs to go up");
        }
        break;
    default : display_message(disp, "unrecognized command"); break;
    }

    if (entity_set_pos(player, newp, map))
        entity_set_pos(camera, newp_cam, map);

    return false;
}

int main(void)
{
    unsigned int seed = time(NULL);
    srand(seed);

    display_t *disp = display_init();

    // create the map
    struct map map;
    create_cave(&map);

    // create the entity map
    ht_t *entities = ht_create(1);

    // create the camera
    struct entity camera;
    camera.disp_ch = "";
    camera.solid   = false;
    camera.visible = false;
    ht_insert(entities, "camera", &camera);

    // create the player
    struct entity player;
    player.disp_ch = "@";
    player.solid   = true;
    player.visible = true;
    ht_insert(entities, "player", &player);

    // set starting point
    struct point cam_p = {
        .x = map.entry_point.x - MAIN_PANEL_WIDTH / 2 + 1,
        .y = map.entry_point.y - MAIN_PANEL_HEIGHT / 2 + 1,
    };
    entity_set_pos(&player, map.entry_point, &map);
    entity_set_pos(&camera, cam_p, &map);

    // start displaying things
    display_map(disp, &map, entities);
    display_instructions(disp);
    display_status(disp, &player);
    display_message(disp, "");

    int  ch;
    bool done = false;
    while (!done) {
        enum action action = display_process_input();
        done               = game_update(disp, action, entities, &map);
        display_map(disp, &map, entities);
    }

    free(map.map);
    ht_destroy(entities);

    endwin();

    return 0;
}
