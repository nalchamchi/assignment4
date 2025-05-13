/*
 * hash_table.c
 *
 * Implements a simple hash table with chaining via linked lists.
 * #1) Implements hash_table_collisions() to count collisions correctly.
 * #2) Provides hash_function2() that on 11 keys and 8 slots yields 3 collisions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "hash_table.h"

/*
 * Definition of the hash_table structure.
 */
struct hash_table {
    struct node** array;
    int size;
    int total;
};

/*
 * hash_function1: basic hash on the first character.
 */
int hash_function1(struct hash_table* hash_table, char* key) {
    return ((int)key[0]) % hash_table->size;
}

/*
 * hash_function2: improved hash for 11 elements in 8 slots → 3 collisions.
 * Uses (first + last + length) & (size - 1) to guarantee 0..size-1.
 */
int hash_function2(struct hash_table* hash_table, char* key) {
    int len   = (int)strlen(key);
    int first = (unsigned char)key[0];
    int last  = (unsigned char)key[len - 1];

    /* combine and mask to 0..size-1 (size==8 → mask &7) */
    unsigned long h = (unsigned long)first + (unsigned long)last + (unsigned long)len;
    int idx = (int)(h & (hash_table->size - 1));
    /* safety: idx < 0 cannot happen here, but just in case */
    if (idx < 0) idx += hash_table->size;
    return idx;
}

/*
 * Create a new, empty hash_table of the given size.
 */
struct hash_table* hash_table_create(int array_size) {
    struct hash_table* ht = malloc(sizeof(struct hash_table));
    assert(ht);
    ht->size  = array_size;
    ht->total = 0;
    ht->array = malloc(array_size * sizeof(struct node*));
    assert(ht->array);
    for (int i = 0; i < array_size; i++) {
        ht->array[i] = NULL;
    }
    return ht;
}

/*
 * Free all memory associated with the hash_table.
 */
void hash_table_free(struct hash_table* hash_table) {
    assert(hash_table);
    for (int i = 0; i < hash_table->size; i++) {
        struct node* cur = hash_table->array[i];
        while (cur) {
            struct node* tmp = cur;
            cur = cur->next;
            free(tmp->key);
            free(tmp);
        }
    }
    free(hash_table->array);
    free(hash_table);
}

/*
 * Reset the hash table: remove all nodes but keep the structure intact.
 */
void hash_table_reset(struct hash_table* hash_table) {
    assert(hash_table);
    for (int i = 0; i < hash_table->size; i++) {
        struct node* cur = hash_table->array[i];
        while (cur) {
            struct node* tmp = cur;
            cur = cur->next;
            free(tmp->key);
            free(tmp);
            hash_table->total--;
        }
        hash_table->array[i] = NULL;
    }
}

/*
 * Add a key-value pair to the hash table using the given hash function.
 */
void hash_table_add(struct hash_table* hash_table,
                    int (*hf)(struct hash_table*, char*),
                    char* key, int value) {
    assert(hash_table);
    struct node* new_node = malloc(sizeof(struct node));
    assert(new_node);
    new_node->key   = malloc(strlen(key) + 1);
    assert(new_node->key);
    strcpy(new_node->key, key);
    new_node->value = value;
    new_node->next  = NULL;

    int idx = hf(hash_table, key);
    new_node->next = hash_table->array[idx];
    hash_table->array[idx] = new_node;
    hash_table->total++;
}

/*
 * Remove a key from the hash table; return 1 if removed, 0 if not found.
 */
int hash_table_remove(struct hash_table* hash_table,
                      int (*hf)(struct hash_table*, char*),
                      char* key) {
    assert(hash_table);
    int idx = hf(hash_table, key);
    struct node* cur = hash_table->array[idx];
    struct node* prev = NULL;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev)
                prev->next = cur->next;
            else
                hash_table->array[idx] = cur->next;
            free(cur->key);
            free(cur);
            hash_table->total--;
            return 1;
        }
        prev = cur;
        cur  = cur->next;
    }
    return 0;
}

/*
 * #1: Count total collisions.
 * For each bucket with count>1, add (count-1) to num_col.
 */
int hash_table_collisions(struct hash_table* hash_table) {
    int num_col = 0;
    for (int i = 0; i < hash_table->size; i++) {
        int count = 0;
        struct node* cur = hash_table->array[i];
        while (cur) {
            count++;
            cur = cur->next;
        }
        if (count > 1) {
            num_col += (count - 1);
        }
    }
    return num_col;
}

/*
 * Display the hash table contents (for debugging).
 */
void display(struct hash_table* hash_table) {
    printf("Hash table, size=%d, total=%d\n",
           hash_table->size, hash_table->total);
    for (int i = 0; i < hash_table->size; i++) {
        struct node* cur = hash_table->array[i];
        printf("array[%d]", i);
        while (cur) {
            printf("->(key=%s,value=%d)", cur->key, cur->value);
            cur = cur->next;
        }
        printf("-|\n");
    }
    printf("\n");
}

