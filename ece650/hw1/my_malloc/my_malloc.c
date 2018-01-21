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
  node->start_address = (void *)((size_t)node - (size_t)sizeof(*node)*8);
  node->end_address = (void *)((size_t)node->start_address - size*8);
  addToLinkedList(prev, node);
}

//First Fit malloc/free
void *ff_malloc(size_t size) {
  if (allocatedListHead == NULL) {
    allocatedListHead= sbrk(sizeof(*allocatedListHead));
    allocatedListHead->next = NULL;
    heapTop = allocatedListHead;
    printf("heapTop = %p", heapTop);
  }
  Node * curr = allocatedListHead->next; 
  Node * prev = allocatedListHead;
  Node * temp = NULL;
  while (curr != NULL) {
    if (((size_t)curr->end_address - (size_t)prev) >= 8 * (size + sizeof(*curr))) {
      temp = (Node *)(curr->end_address - 8 * sizeof(*curr));
      break;
    }
    prev = curr;
    curr = curr->next;      
  }
  if (temp == NULL) {
    if (((size_t)heapTop - (size_t)prev) >= (size + sizeof(*curr)) * 8) {
      temp = heapTop;
    } else {
      temp = sbrk(size + sizeof(*curr) - ((size_t)heapTop - (size_t)prev)/8);
      heapTop = temp;
      printf("heapTop = %p", heapTop);
    }
  }
  initNode(temp, prev, size);
  printf("start_address = %p", (void *)temp->start_address);
  return (void *)(temp->end_address);
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
