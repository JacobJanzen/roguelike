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
#include <curses.h>
#include <getopt.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../config.h"
#include "common.h"
#include "display.h"
#include "entity.h"
#include "game.h"
#include "ht.h"
#include "mapgen.h"

struct cmd_option_results {
    FILE *save_file;
};

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

    if (entity_set_position(player, newp, map))
        entity_set_position(camera, newp_cam, map);

    return false;
}

void print_version(void)
{
    printf("%s\n", PACKAGE_STRING);
    printf("Copyright (C) 2024  Jacob Janzen\n");
    printf("This is free software; see the source for copying conditions.\n");
    printf(
        "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A\n"
    );
    printf("PARTICULAR PURPOSE.\n");
}

void print_help(void)
{
    printf("Usage: urlg [options]\n");
    printf("Options:\n");
    printf("  -h, --help                  Print this message and exit.\n");
    printf("  -l, --load <file>           File to load a savegame from.\n");
    printf(
        "  -v, --version               Print the version number of urlg and "
        "exit.\n"
    );
    printf("\n");
    printf("Report bugs to <%s>\n", PACKAGE_BUGREPORT);
}

struct cmd_option_results *process_cmd_options(int argc, char **argv)
{
    int   option_index = 0;
    int   ch;
    bool  version_flag   = false;
    bool  help_flag      = false;
    char *save_file_name = NULL;

    static struct option longopts[] = {
        {"help",    no_argument,       NULL, 'h'}, // default to help if unrecognized
        {"version", no_argument,       NULL, 'v'},
        {"load",    required_argument, NULL, 'l'},
    };
    while ((ch = getopt_long(argc, argv, "l:vh", longopts, &option_index)) != -1
    ) {
        switch (ch) {
        case 'v' : version_flag = true; break;
        case 'h' : help_flag = true; break;
        case 'l' : save_file_name = optarg; break;
        case 0 : break;
        default :
            if (strcmp("load", longopts[option_index].name) == 0) {
                save_file_name = optarg;
            } else if (strcmp("version", longopts[option_index].name) == 0) {
                version_flag = true;
            } else {
                help_flag = true;
            }
            break;
        }
    }

    // print version or help and exit if version/help flag is given
    if (version_flag) {
        print_version();
        exit(EXIT_SUCCESS);
    }
    if (help_flag) {
        print_help();
        exit(EXIT_SUCCESS);
    }

    struct cmd_option_results *res = malloc(sizeof(struct cmd_option_results));
    if (save_file_name) {
        res->save_file = fopen(save_file_name, "r+");
        if (!res->save_file) {
            perror("failed to open save file");
            exit(EXIT_FAILURE);
        }
    }

    return res;
}

int main(int argc, char **argv)
{
    process_cmd_options(argc, argv);

    unsigned int seed = time(NULL);
    srand(seed);

    display_t *disp = display_init();

    if (!disp) {
        return EXIT_FAILURE;
    }

    struct game *game = game_init();

    // start displaying things
    display_refresh(game->display, &game->map, game->entities);

    bool done = false;
    while (!done) {
        enum action action = display_process_input();
        done = game_update(disp, action, game->entities, &game->map);
        display_map(disp, &game->map, game->entities);
    }

    game_destroy(game);

    return 0;
}
