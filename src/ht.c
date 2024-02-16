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
#include "ht.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    char        *key;
    void        *val;
    struct node *next;
};

struct hash_table {
    struct node **vals;
    int           max_size;
    int           size;

    int          curr_index;
    struct node *curr_node;
    bool         iterating;
};

static void rehash(ht_t *h, int newsize)
{
    ht_t *new_h = ht_create(newsize);

    ht_iter_init(h);

    struct kvp kvp = ht_iter_next(h);
    while (kvp.key) {
        ht_insert(new_h, kvp.key, kvp.val);
        kvp = ht_iter_next(h);
    }

    // only destroy vals if it isn't NULL
    if (h->vals) {
        // iterate through the hash values
        for (int i = 0; i < h->max_size; ++i) {
            // iterate through the linked list and remove the value
            struct node *curr = h->vals[i];
            struct node *prev;
            while (curr) {
                prev = curr;
                curr = curr->next;
                free(prev->key);
                free(prev);
            }
        }
        free(h->vals);
    }

    h->max_size   = newsize;
    h->vals       = new_h->vals;
    h->curr_index = 0;
    h->curr_node  = NULL;
    h->iterating  = false;

    free(new_h);
}

static unsigned long djb2_hash(char *str)
{
    unsigned long hash = 5381;
    int           c;

    // hash * 33 + c
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;

    return hash;
}

ht_t *ht_create(int max_size)
{
    ht_t *h       = malloc(sizeof(ht_t));
    h->max_size   = max_size;
    h->size       = 0;
    h->vals       = malloc(sizeof(struct node) * max_size);
    h->curr_index = 0;
    h->curr_node  = NULL;
    h->iterating  = false;

    return h;
}

void ht_destroy(ht_t *h)
{
    // only destroy if h isn't NULL
    if (h) {
        // only destroy vals if it isn't NULL
        if (h->vals) {
            // iterate through the hash values
            for (int i = 0; i < h->max_size; ++i) {
                // iterate through the linked list and remove the value
                struct node *curr = h->vals[i];
                struct node *prev;
                while (curr) {
                    prev = curr;
                    curr = curr->next;
                    free(prev->key);
                    free(prev);
                }
            }
            free(h->vals);
        }
        free(h);
    }
}

void *ht_find(ht_t *h, char *key)
{
    unsigned int hash = djb2_hash(key) % h->max_size;

    // exit early if hash isn't in table
    if (!h->vals[hash])
        return NULL;

    // iterate through values stored at `hash`; exit early if found
    struct node *curr = h->vals[hash];
    while (curr) {
        if (strcmp(key, curr->key) == 0)
            return curr->val;
        curr = curr->next;
    }

    return NULL;
}

void ht_insert(ht_t *h, char *key, void *val)
{
    if (h->size > h->max_size * 0.75)
        rehash(h, h->max_size * 2);

    unsigned int hash = djb2_hash(key) % h->max_size;

    // create a node
    int len          = strlen(key);
    struct node *new = malloc(sizeof(struct node));
    new->key         = malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; ++i) {
        new->key[i] = key[i];
    }
    new->key[len] = 0;
    new->val      = val;

    // insert node at beginning of list
    if (h->vals[hash]) {
        new->next = h->vals[hash];
    }
    h->vals[hash] = new;

    ++(h->size);
}

void ht_delete(ht_t *h, char *key)
{
    if (h->size < h->max_size * 0.25)
        rehash(h, h->max_size * 0.5);

    unsigned int hash = djb2_hash(key) % h->max_size;

    if (!h->vals[hash])
        return; // exit if the hash is not found

    // remove the key from the front of the list if it's there
    if (strcmp(key, h->vals[hash]->key) == 0) {
        struct node *temp = h->vals[hash];
        h->vals[hash]     = h->vals[hash]->next;
        free(temp->key);
        free(temp);
    } else {
        // iterate through list and remove once a match is found
        struct node *prev = h->vals[hash];
        struct node *curr = h->vals[hash]->next;
        while (curr) {
            if (strcmp(key, curr->key) == 0) {
                prev->next = curr->next;
                free(curr->key);
                free(curr);
                return;
            }

            prev = curr;
            curr = curr->next;
        }
    }
}

int ht_size(ht_t *h)
{
    return h->size;
}

void ht_iter_init(ht_t *h)
{
    h->curr_index = 0;
    h->curr_node  = h->vals[0];
}

struct kvp ht_iter_next(ht_t *h)
{
    struct kvp out = {
        .key = NULL,
        .val = NULL,
    };

    // return NULL if we've reached the end
    if (!h->curr_node && h->curr_index >= h->max_size)
        return out;

    // look for next index with node if current node is NULL
    if (!h->curr_node) {
        while (!h->curr_node) {
            h->curr_node = h->vals[h->curr_index++];
            if (h->curr_index >= h->max_size)
                return out;
        }
    }

    // get the value and move to the next node
    out.key      = h->curr_node->key;
    out.val      = h->curr_node->val;
    h->curr_node = h->curr_node->next;
    return out;
}
