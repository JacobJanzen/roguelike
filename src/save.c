#include "save.h"

#include <stdio.h>
#include <string.h>

#include "game.h"

#define NUMBER_STRLEN 32

struct game *game_load(char *savegame)
{
    FILE *fp = fopen(savegame, "r");
    if (!fp)
        return game_init();

    // TODO implement this
    fclose(fp);
    return game_init();
}

static void save_bool(bool val, FILE *fp)
{
    if (val)
        fwrite("true", 4, 1, fp);
    else
        fwrite("false", 5, 1, fp);
}

static void save_int(int val, FILE *fp)
{
    char val_str[NUMBER_STRLEN];
    snprintf(val_str, NUMBER_STRLEN, "%d", val);
    fwrite(val_str, strlen(val_str), 1, fp);
}

static void save_str(char *val, FILE *fp)
{
    fwrite("\"", 1, 1, fp);
    fwrite(val, strlen(val), 1, fp);
    fwrite("\"", 1, 1, fp);
}

static void save_point(struct point *point, FILE *fp)
{
    fwrite("{", 1, 1, fp);

    save_str("x", fp);
    fwrite(":", 1, 1, fp);
    save_int(point->x, fp);

    fwrite(",", 1, 1, fp);

    save_str("y", fp);
    fwrite(":", 1, 1, fp);
    save_int(point->y, fp);

    fwrite("}", 1, 1, fp);
}

static void save_map(struct map *map, FILE *fp)
{
    fwrite("{", 1, 1, fp);

    save_str("height", fp);
    fwrite(":", 1, 1, fp);
    save_int(map->height, fp);
    fwrite(",", 1, 1, fp);

    save_str("width", fp);
    fwrite(":", 1, 1, fp);
    save_int(map->width, fp);
    fwrite(",", 1, 1, fp);

    save_str("entry_point", fp);
    fwrite(":", 1, 1, fp);
    save_point(&map->entry_point, fp);
    fwrite(",", 1, 1, fp);

    save_str("map", fp);
    fwrite(":", 1, 1, fp);
    fwrite("[", 1, 1, fp);
    int top = map->width * map->height - 1;
    for (int i = 0; i <= top; ++i) {
        save_int(map->map[i], fp);
        if (i < top)
            fwrite(",", 1, 1, fp);
    }
    fwrite("]", 1, 1, fp);

    fwrite("}", 1, 1, fp);
}

static void save_entity(struct entity *entity, FILE *fp)
{
    fwrite("{", 1, 1, fp);

    save_str("p", fp);
    fwrite(":", 1, 1, fp);
    save_point(&entity->p, fp);
    fwrite(",", 1, 1, fp);

    save_str("disp_ch", fp);
    fwrite(":", 1, 1, fp);
    save_str(entity->disp_ch, fp);
    fwrite(",", 1, 1, fp);

    save_str("solid", fp);
    fwrite(":", 1, 1, fp);
    save_bool(entity->solid, fp);
    fwrite(",", 1, 1, fp);

    save_str("visible", fp);
    fwrite(":", 1, 1, fp);
    save_bool(entity->visible, fp);

    fwrite("}", 1, 1, fp);
}

void game_save(struct game *game, char *savegame)
{
    FILE *fp = fopen(savegame, "w");
    if (!fp) {
        perror("could not open file for saving");
    } else {
        fwrite("{", 1, 1, fp);

        save_str("map", fp);
        fwrite(":", 1, 1, fp);
        save_map(&game->map, fp);

        fwrite(",", 1, 1, fp);

        save_str("entities", fp);
        fwrite(":", 1, 1, fp);
        fwrite("{", 1, 1, fp);
        ht_iter_init(game->entities);
        struct kvp kvp = ht_iter_next(game->entities);
        if (kvp.key) {
            save_str(kvp.key, fp);
            fwrite(":", 1, 1, fp);
            save_entity(kvp.val, fp);
            kvp = ht_iter_next(game->entities);
        }
        while (kvp.key) {
            fwrite(",", 1, 1, fp);

            save_str(kvp.key, fp);
            fwrite(":", 1, 1, fp);
            save_entity(kvp.val, fp);
            kvp = ht_iter_next(game->entities);
        }
        fwrite("}", 1, 1, fp);

        fwrite("}\n", 2, 1, fp);
    }
}
