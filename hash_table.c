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


int hash_function2(struct hash_table* hash_table, char* key) {
  int len = strlen(key);
  int first = key[0];
  int last = key[len - 1];
  int mid = key[len / 2];


  int hash = (first * 7 + last * 11 + mid * 13 + len * 17);

 
  return hash % hash_table->size;
}


struct hash_table* hash_table_create(int array_size) {
  struct hash_table* hash_table = malloc(sizeof(struct hash_table));
  assert(hash_table);
  hash_table->total = 0;
  hash_table->size = array_size;
  hash_table->array = malloc(array_size * sizeof(struct node*));

  for (int i = 0; i < hash_table->size; i++) {
    hash_table->array[i] = NULL;
  }

  return hash_table;
}


void hash_table_free(struct hash_table* hash_table) {
  assert(hash_table);

  for (int i = 0; i < hash_table->size; i++) {
    struct node* current = hash_table->array[i];
    while (current != NULL) {
      struct node* temp = current;
      current = current->next;
      free(temp->key);
      free(temp);
    }
  }

  free(hash_table->array);
  free(hash_table);
}


void hash_table_reset(struct hash_table* hash_table) {
  assert(hash_table);

  for (int i = 0; i < hash_table->size; i++) {
    struct node* current = hash_table->array[i];
    while (current != NULL) {
      struct node* temp = current;
      current = current->next;
      free(temp->key);
      free(temp);
      hash_table->total--;
    }
    hash_table->array[i] = NULL;
  }
}


void hash_table_add(struct hash_table* hash_table, int (*hf)(struct hash_table*, char*), char* key, int value) {
  assert(hash_table);

  struct node* new_node = malloc(sizeof(struct node));
  assert(new_node);

  new_node->key = malloc(strlen(key) + 1);
  strcpy(new_node->key, key);
  new_node->value = value;
  new_node->next = NULL;

  int hash_index = (*hf)(hash_table, key);

  new_node->next = hash_table->array[hash_index];
  hash_table->array[hash_index] = new_node;

  hash_table->total++;
}


int hash_table_remove(struct hash_table* hash_table, int (*hf)(struct hash_table*, char*), char* key) {
  assert(hash_table);

  int hash_index = (*hf)(hash_table, key);
  struct node* current = hash_table->array[hash_index];
  struct node* prev = NULL;

  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
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

    prev = current;
    current = current->next;
  }

  return 0;
}


int hash_table_collisions(struct hash_table* hash_table) {
  int num_col = 0;

  for (int i = 0; i < hash_table->size; i++) {
    struct node* current = hash_table->array[i];
    int count = 0;

    while (current != NULL) {
      count++;
      current = current->next;
    }

    if (count > 1) {
      num_col += (count - 1);
    }
  }

  return num_col;
}


void display(struct hash_table* hash_table) {
  printf("Hash table, size=%d, total=%d\n", hash_table->size, hash_table->total);

  for (int i = 0; i < hash_table->size; i++) {
    struct node* current = hash_table->array[i];
    printf("array[%d]", i);

    while (current != NULL) {
      printf(" -> (key=%s, value=%d)", current->key, current->value);
      current = current->next;
    }

    printf(" -|\n");
  }

  printf("\n");
}

