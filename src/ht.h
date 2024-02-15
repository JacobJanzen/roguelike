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
