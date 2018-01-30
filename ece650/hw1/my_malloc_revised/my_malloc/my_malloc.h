#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
struct Node_t {
  struct Node_t * next;
  //  struct Node_t * next_block;
  size_t size;
};
typedef struct Node_t Node;
//First Fit malloc/free
void *ff_malloc(size_t size);
void ff_free(void *ptr);
//Best Fit malloc/free
void *bf_malloc(size_t size);
void bf_free(void *ptr);
unsigned long get_data_segment_size(); //in bytes
unsigned long get_data_segment_free_space_size(); //in bytes 
//global variable to store the freed memory list
Node * allocatedListHead = NULL;
Node * freedListHead = NULL;
Node * freedListTail = NULL;
void * heapTop = NULL;
#endif
