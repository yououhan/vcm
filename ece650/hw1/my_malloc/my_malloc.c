#include "my_malloc.h"
#include <unistd.h>

extern Node * allocatedListHead;
extern void * heapTop;

void addToLinkedList(Node * prev, Node * toBeAdded) {
  Node * temp = prev->next;
  prev->next = toBeAdded;
  toBeAdded->next = temp;
  toBeAdded->prev = prev;
  if (temp != NULL) {
    temp->prev = toBeAdded;
  }
}

void deleteFromLinkedList(Node * toBeDeleted) {
  Node * prev = toBeDeleted->prev;
  prev->next = toBeDeleted->next;
  if (toBeDeleted->next != NULL) {
    toBeDeleted->next->prev = prev;
  }
}

void initNode(Node * node, Node * prev, size_t size) {
  node->start_address = (void *)((size_t)node + (size_t)sizeof(*node));
  node->end_address = (void *)((size_t)node->start_address + size);
  addToLinkedList(prev, node);
}

void initListHead() {
  allocatedListHead= sbrk(sizeof(*allocatedListHead));
  allocatedListHead->next = NULL;
  allocatedListHead->prev = NULL;  
  heapTop = allocatedListHead + 1;//exclusive
  allocatedListHead->start_address = heapTop;
  allocatedListHead->end_address = heapTop;
}
//First Fit malloc/free
void *ff_malloc(size_t size) {
  if (allocatedListHead == NULL) {
    initListHead();
  }
  Node * curr = allocatedListHead->next; 
  Node * prev = allocatedListHead;
  Node * newAllocatedNode = NULL;
  while (curr != NULL) {
    if (((size_t)curr - (size_t)prev->end_address) >= (size + sizeof(*curr))) {
      newAllocatedNode = (Node *)((size_t)curr - size - sizeof(*curr));
      break;
    }
    prev = curr;
    curr = curr->next;      
  }
  if (newAllocatedNode == NULL) {
    if (((size_t)heapTop - (size_t)prev->end_address) >= (size + sizeof(*curr))) {
      newAllocatedNode = heapTop - size - sizeof(*curr);
    } else {
      size_t increment = size + sizeof(*curr) - ((size_t)heapTop - (size_t)prev->end_address);
      heapTop = (void *)((size_t)sbrk(increment) + increment);
      newAllocatedNode = (Node *)((size_t)heapTop - size - sizeof(*curr));
      //printf("heapTop = %p\n", heapTop);
    }
  }
  //  if (allocatedListHead->next != NULL)printf("allocatedListHead = %p, heapTop = %p, itself = %p, prev = %p, next = %p\n",allocatedListHead, heapTop, allocatedListHead->next, allocatedListHead->next->prev, allocatedListHead->next->next);
  //printf("newAllocatedNode = %p, heapTop = %p, itself = %p, prev = %p, next = %p\n",newAllocatedNode, heapTop, prev, prev->prev, prev->next);
  initNode(newAllocatedNode, prev, size);
  //  printf("prev = %p, next = %p\n", newAllocatedNode->prev, newAllocatedNode->next);
  //  printf("heapTop = %p\n", heapTop);  
  //  printf("allocating = %p\n", (void *)newAllocatedNode->start_address);
  return (void *)(newAllocatedNode->start_address);
}

void ff_free(void *ptr) {
  if (ptr != NULL) {
    //printf("deleting %p\n", ptr);
    Node * toBeDeleted = (Node *)((size_t)ptr - (size_t)sizeof(Node));
    deleteFromLinkedList(toBeDeleted);
  }
}

//Best Fit malloc/free
void *bf_malloc(size_t size) {
  if (allocatedListHead == NULL) {
    initListHead();
  }
  Node * curr = allocatedListHead->next; 
  Node * prev = allocatedListHead;
  Node * newAllocatedNode = NULL;
  Node * bestFitPrev = NULL;
  size_t minSize = SIZE_MAX;
  while (curr != NULL) {
    size_t currSize = (size_t)curr - (size_t)prev->end_address;
    if (currSize >= (size + sizeof(*curr)) && currSize < minSize) {
	newAllocatedNode = (Node *)((size_t)curr - size - sizeof(*curr));
	minSize = currSize;
	bestFitPrev = prev;
    }
    prev = curr;
    curr = curr->next;      
  }
  if (newAllocatedNode == NULL) {
    bestFitPrev = prev;
    if (((size_t)heapTop - (size_t)prev->end_address) >= (size + sizeof(*curr))) {
      newAllocatedNode = heapTop - size - sizeof(*curr);
    } else {
      size_t increment = size + sizeof(*curr) - ((size_t)heapTop - (size_t)prev->end_address);
      newAllocatedNode = sbrk(increment);
      heapTop = (void *)((size_t)newAllocatedNode + increment);
      //printf("heapTop = %p\n", heapTop);
    }
  }
  initNode(newAllocatedNode, bestFitPrev, size);
  //printf("heapTop = %p\n", heapTop);  
  //printf("start_address = %p\n", (void *)newAllocatedNode->start_address);
  return (void *)(newAllocatedNode->start_address);
}
void bf_free(void *ptr) {
  ff_free(ptr);
}
unsigned long get_data_segment_size() {
  Node * curr = allocatedListHead;
  unsigned long dataSize = 0;
  while (curr != NULL) {
    dataSize += curr->end_address - curr->start_address;
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
