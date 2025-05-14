#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "hash_table.h"


struct hash_table {
  struct node** array;
  int size;
  int total;
};


int hash_function1(struct hash_table* hash_table, char* key) {
  return ( (int) key[0] ) % hash_table->size;
}


int hash_function2(struct hash_table* hash_table, char* key)
{
    assert(hash_table && key && key[0]);

    unsigned int len   = (unsigned)strlen(key);
    unsigned int first = (unsigned char)key[0];
    unsigned int last  = (unsigned char)key[len - 1];

    return (len + first + last) % hash_table->size;
}



struct hash_table* hash_table_create(int array_size) {
  struct hash_table* hash_table = malloc(sizeof(struct hash_table));
  assert(hash_table);
  hash_table->total = 0;
  hash_table->size = array_size;

  hash_table->array = malloc(array_size * sizeof(struct node*));
  for(int i = 0; i < hash_table->size; i++) {
    hash_table->array[i] = NULL;
  }

  return hash_table;
}

void hash_table_free(struct hash_table* hash_table) {
  assert(hash_table);
  for(int i = 0; i < hash_table->size; i++) {
    struct node* current = hash_table->array[i];
    while (current != NULL) {
      hash_table->array[i] = current->next;
      free(current->key);
      free(current);
      current = hash_table->array[i];
    }
  }
  free(hash_table->array);
  free(hash_table);
}

void hash_table_reset(struct hash_table* hash_table)
{
    assert(hash_table);

    for (int i = 0; i < hash_table->size; ++i) {
        struct node* cur = hash_table->array[i];
        while (cur) {
            struct node* next = cur->next;
            free(cur->key);
            free(cur);
            cur = next;
        }
        hash_table->array[i] = NULL;
    }
    hash_table->total = 0;                  
}

void hash_table_add(struct hash_table* hash_table,
                    int (*hf)(struct hash_table*, char*),
                    char* key,
                    int value)
{
    assert(hash_table && hf && key);

    int hash_index = (*hf)(hash_table, key);

    for (struct node* cur = hash_table->array[hash_index]; cur; cur = cur->next) {
        if (strcmp(cur->key, key) == 0) {   
            cur->value = value;
            return;                        
        }
    }

    struct node* new_node = malloc(sizeof(struct node));
    assert(new_node);

    new_node->key = malloc(strlen(key) + 1);
    assert(new_node->key);
    strcpy(new_node->key, key);

    new_node->value = value;
    new_node->next  = hash_table->array[hash_index];
    hash_table->array[hash_index] = new_node;

    hash_table->total++;                    
}



int hash_table_remove(struct hash_table* hash_table,
                      int (*hf)(struct hash_table*, char*),
                      char* key)
{
    assert(hash_table && hf && key);

    int hash_index = (*hf)(hash_table, key);
    struct node* curr = hash_table->array[hash_index];
    struct node* prev = NULL;


    while (curr && strcmp(curr->key, key) != 0) {
        prev = curr;
        curr = curr->next;
    }
    if (!curr) return 0;                    


    if (prev)  prev->next = curr->next;     
    else       hash_table->array[hash_index] = curr->next; 

    free(curr->key);
    free(curr);

    hash_table->total--;                    
    return 1;
}


int hash_table_collisions(struct hash_table* hash_table)
{
    assert(hash_table);
    int num_col = 0;

    for (int i = 0; i < hash_table->size; ++i) {
        int bucket_cnt = 0;
        for (struct node* cur = hash_table->array[i]; cur; cur = cur->next)
            ++bucket_cnt;
        if (bucket_cnt > 1)
            num_col += bucket_cnt - 1;
    }
    return num_col;
}


void display(struct hash_table* hash_table) {
  printf("Hash table, size=%d, total=%d\n", hash_table->size, hash_table->total);
  for (int i = 0; i < hash_table->size; i++) {
    struct node *temp = hash_table->array[i];
    if (temp == NULL) {
      printf("array[%d]-|\n", i);
    } else {
      printf("array[%d]", i);
      while (temp != NULL) {
        printf("->(key=%s,value=%d)", temp->key, temp->value);
        temp = temp->next;
      }
      printf("-|\n");
    }
  }
  printf("\n");
}

