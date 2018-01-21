#include "my_malloc.h"
#include <unistd.h>

extern Node * allocatedListHead;
extern void * heapTop;

void addToLinkedList(Node * prev, Node * toBeAdded) {
  Node * temp = prev->next;
  prev->next = toBeAdded;
  toBeAdded->next = temp;
}

void deleteFromLinkedList(Node * prev, Node * toBeDeleted) {
  prev->next = toBeDeleted->next;
}

void initNode(Node * node, Node * prev, size_t size) {
  node->start_address = (void *)((size_t)node + (size_t)sizeof(*node));
  node->end_address = (void *)((size_t)node->start_address + size);
  addToLinkedList(prev, node);
}

//First Fit malloc/free
void *ff_malloc(size_t size) {
  if (allocatedListHead == NULL) {
    allocatedListHead= sbrk(sizeof(*allocatedListHead));
    allocatedListHead->next = NULL;
    heapTop = allocatedListHead + 1;//exclusive
    allocatedListHead->start_address = heapTop;
    allocatedListHead->end_address = heapTop;
    //printf("heapTop = %p\n", heapTop);
  }
  Node * curr = allocatedListHead->next; 
  Node * prev = allocatedListHead;
  Node * temp = NULL;
  while (curr != NULL) {
    if (((size_t)curr - (size_t)prev->end_address) >= (size + sizeof(*curr))) {
      temp = (Node *)((size_t)curr - size - sizeof(*curr));
      break;
    }
    prev = curr;
    curr = curr->next;      
  }
  if (temp == NULL) {
    if (((size_t)heapTop - (size_t)prev->end_address) >= (size + sizeof(*curr))) {
      temp = heapTop - size - sizeof(*curr);
    } else {
      size_t increment = size + sizeof(*curr) - ((size_t)heapTop - (size_t)prev->end_address);
      temp = sbrk(increment);
      heapTop = (void *)((size_t)temp + increment);
      //printf("heapTop = %p\n", heapTop);
    }
  }
  initNode(temp, prev, size);
  printf("heapTop = %p\n", heapTop);  
  printf("start_address = %p\n", (void *)temp->start_address);
  return (void *)(temp->start_address);
}

void ff_free(void *ptr) {
  Node * curr = allocatedListHead->next; 
  Node * prev = allocatedListHead;
  while (curr != NULL) {
    if (curr->start_address == ptr) {
      deleteFromLinkedList(prev, curr);
      break;
    }
    prev = curr;
    curr = curr->next;
  }
}

//Best Fit malloc/free
void *bf_malloc(size_t size) {
  return NULL;
}
void bf_free(void *ptr) {
}
unsigned long get_data_segment_size() {
  Node * curr = allocatedListHead;
  unsigned long dataSize = 0;
  while (curr != NULL) {
    dataSize += curr->start_address - curr->end_address;
    curr = curr->next;
  }
  return dataSize;
}
unsigned long get_data_segment_free_space_size() {
  Node * curr = allocatedListHead;
  size_t counter = 0;
  while (curr != NULL) {
    counter++;
    curr = curr->next;
  }
  return (get_data_segment_size() - counter * sizeof(*curr));
}
