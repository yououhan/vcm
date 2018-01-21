#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
//First Fit malloc/free
void *ff_malloc(size_t size);
void ff_free(void *ptr);
//Best Fit malloc/free
void *bf_malloc(size_t size);
void bf_free(void *ptr);
unsigned long get_data_segment_size(); //in bytes
unsigned long get_data_segment_free_space_size(); //in bytes 
//global variable to store the freed memory list
struct Node_t {
  void * start_address;
  void * end_address;
  //  size_t size;
  struct Node_t * next;
};
typedef struct Node_t Node;
Node * allocatedListHead = NULL;
void * heapTop = NULL;
#endif
