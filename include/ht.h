#ifndef HT_H_
#define HT_H_

#include <stdbool.h>

typedef struct hash_table ht_t;

struct kvp {
    char *key;
    void *val;
};

// construct and destructor
ht_t *ht_create(int size);
void  ht_destroy(ht_t *h);

// accessors
void *ht_find(ht_t *h, char *key);
void  ht_insert(ht_t *h, char *key, void *val);
void  ht_delete(ht_t *h, char *key);

// queries
int ht_size(ht_t *h);

// iterator
void       ht_iter_init(ht_t *h);
struct kvp ht_iter_next(ht_t *h);

#endif // HT_H_
