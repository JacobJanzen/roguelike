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
    struct cmd_option_results *cmd = process_cmd_options(argc, argv);

    unsigned int seed = time(NULL);
    srand(seed);

    struct game *game;
    if (!cmd->save_file)
        game = game_init();
    else
        game = game_load(cmd->save_file);

    while (!game->done) {
        game_step(game);
    }

    game_destroy(game);

    if (cmd->save_file) {
        fclose(cmd->save_file);
    }
    free(cmd);

    return 0;
}
