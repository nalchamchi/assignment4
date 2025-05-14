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
    assert(hash_table);
    assert(key && key[0]);                

    unsigned int len   = (unsigned int)strlen(key);
    unsigned int first = (unsigned char)key[0];
    unsigned int last  = (unsigned char)key[len - 1];

    unsigned int hash  = len + first + last;
    return hash % hash_table->size;
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

void hash_table_reset(struct hash_table* hash_table) {
  assert(hash_table);
  for(int i = 0; i < hash_table->size; i++) {
    struct node* current = hash_table->array[i];
    while (current != NULL) {
      hash_table->array[i] = current->next;
      free(current->key);
      free(current);
      current = hash_table->array[i];
      hash_table->total--;
    }
  }
}

void hash_table_add(struct hash_table* hash_table,
                    int (*hf)(struct hash_table*, char*),
                    char* key,
                    int value) {
  assert(hash_table);

  struct node* new_node = malloc(sizeof(struct node));
  assert(new_node);

  new_node->key = malloc(strlen(key) + 1);
  strcpy(new_node->key, key);
  new_node->value = value;

  int hash_index = (*hf)(hash_table, key);

  new_node->next = hash_table->array[hash_index];
  hash_table->array[hash_index] = new_node;
  hash_table->total++;
}

int hash_table_remove(struct hash_table* hash_table,
                      int (*hf)(struct hash_table*, char*),
                      char* key) {
  assert(hash_table && hash_table->array);

  int hash_index = (*hf)(hash_table, key);
  struct node* current = hash_table->array[hash_index];
  struct node* prev = NULL;

  while (current != NULL && strcmp(current->key, key) != 0) {
    prev = current;
    current = current->next;
  }

  if (current == NULL) {
    return 0;               
  }

  if (prev == NULL) {       
    hash_table->array[hash_index] = current->next;
  } else {
    prev->next = current->next;
  }

  free(current->key);
  free(current);
  hash_table->total--;
  return 1;
}

int hash_table_collisions(struct hash_table* hash_table)
{
    assert(hash_table);

    int num_col = 0;

    for (int i = 0; i < hash_table->size; ++i) {
        int bucket_count = 0;
        struct node* cur = hash_table->array[i];

        while (cur) {                
            ++bucket_count;
            cur = cur->next;
        }

        if (bucket_count > 1)
            num_col += bucket_count - 1;
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

