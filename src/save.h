#ifndef SAVE_H_
#define SAVE_H_

struct game *game_load(char *savegame);
void         game_save(struct game *game, char *savegame);

#endif // SAVE_H_
