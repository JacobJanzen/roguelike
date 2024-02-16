/*
This file is part of urlg.
urlg is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version. urlg is
distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with urlg. If not, see
<https://www.gnu.org/licenses/>.
*/
#include "game.h"

#include <stdlib.h>

#include "entity.h"

struct game *game_init(void)
{
    struct game *g = malloc(sizeof(struct game));

    create_cave(&g->map);
    g->entities = ht_create(64);
    g->display  = display_init();
    if (!g->entities || !g->display) {
        if (g->entities)
            free(g->entities);
        if (g->display)
            display_destroy(g->display);
        free(g);
        return NULL;
    }

    struct entity *camera = malloc(sizeof(struct entity));
    camera->disp_ch       = "";
    camera->solid         = false;
    camera->visible       = false;
    ht_insert(g->entities, "camera", camera);

    struct entity *player = malloc(sizeof(struct entity));
    player->disp_ch       = "@";
    player->solid         = true;
    player->visible       = true;
    ht_insert(g->entities, "player", player);

    struct point cam_p = {
        .x = g->map.entry_point.x - MAIN_PANEL_WIDTH / 2 + 1,
        .y = g->map.entry_point.y - MAIN_PANEL_HEIGHT / 2 + 1,
    };
    entity_set_position(player, g->map.entry_point, &g->map);
    entity_set_position(camera, cam_p, &g->map);

    return g;
}

void game_destroy(struct game *game)
{
    free(game->map.map);

    ht_iter_init(game->entities);
    struct kvp kvp = ht_iter_next(game->entities);
    while (kvp.key) {
        if (kvp.val)
            free(kvp.val);
        kvp = ht_iter_next(game->entities);
    }
    ht_destroy(game->entities);
    display_destroy(game->display);
}
