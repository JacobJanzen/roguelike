#include "ht.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 1024

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

static unsigned long djb2_hash(char *str)
{
    unsigned long hash = 5381;
    int           c;

    // hash * 33 + c
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;

    return hash;
}

ht_t *ht_create(void)
{
    ht_t *h       = malloc(sizeof(ht_t));
    h->max_size   = SIZE;
    h->size       = 0;
    h->vals       = malloc(sizeof(struct node) * h->max_size);
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

void *ht_iter_next(ht_t *h)
{
    // return NULL if we've reached the end
    if (!h->curr_node && h->curr_index >= h->max_size)
        return NULL;

    if (!h->curr_node) {
        // look for next index with node if current node is NULL
        while (!h->curr_node) {
            h->curr_node = h->vals[h->curr_index++];
            if (h->curr_index >= h->max_size)
                return NULL;
        }
        return h->curr_node->val;
    } else {
        // move to the next node
        h->curr_node = h->curr_node->next;
        if (h->curr_node)
            return h->curr_node->val;

        // look for next index with node if necessary
        while (!h->curr_node) {
            h->curr_node = h->vals[h->curr_index++];
            if (h->curr_index >= h->max_size)
                return NULL;
        }
        return h->curr_node->val;
    }
}
