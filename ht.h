#ifndef HT_H_
#define HT_H_

#include <stdbool.h>

typedef struct hash_table ht_t;

ht_t *ht_create(void);
void  ht_destroy(ht_t *h);

void *ht_find(ht_t *h, char *key);
void  ht_insert(ht_t *h, char *key, void *val);
void  ht_delete(ht_t *h, char *key);

int ht_size(ht_t *h);

void  ht_iter_init(ht_t *h);
void *ht_iter_next(ht_t *h);

#endif // HT_H_
