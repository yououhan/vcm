#include "my_malloc.h"
#include <unistd.h>

void initListHead() {
  if (head == NULL) {
    head= sbrk(sizeof(Node));
    head->next = NULL;
    head->size = 0;
    heapTop = head + 1;//exclusive
  }
}
  
void addToFreedList(Node * prev, Node * toBeAdded) {
  if (prev == NULL) {
    prev = head;
    Node * curr = head->next;
    while (curr != NULL && (size_t)curr < (size_t)toBeAdded) {
      prev = curr;
      curr = curr->next;
    }
  }
  Node * newAddedNode = NULL;
  if ((size_t)prev + prev->size + sizeof(Node) == (size_t)toBeAdded) {//combine with the previous node
    prev->size += toBeAdded->size + sizeof(Node);
    newAddedNode = prev;
  } else {
    toBeAdded->next = prev->next;
    prev->next = toBeAdded;
    newAddedNode = toBeAdded;
  }
  if ((size_t)newAddedNode + newAddedNode->size + sizeof(Node) == (size_t)newAddedNode->next) {//combine with the next node
    newAddedNode->size += newAddedNode->next->size + sizeof(Node);
    newAddedNode->next = newAddedNode->next->next;
  }
}

Node * allocateNewSpace(Node * prev, size_t size) {
  Node * newAllocatedNode = NULL;
  size_t increment = size + 2 * sizeof(Node);
  Node * newFreeNode = heapTop;
  heapTop = (void *)((size_t)sbrk(increment) + increment);
  newAllocatedNode = (Node *)((size_t)heapTop - size - sizeof(Node));
  newFreeNode->size = (size_t)newAllocatedNode - (size_t)newFreeNode - sizeof(Node);
  addToFreedList(prev, newFreeNode);
  return newAllocatedNode;
}

//First Fit malloc/free
void * ff_malloc(size_t size) {
  initListHead();
  Node * prev = head;
  Node * curr = head->next;
  Node * newAllocatedNode = NULL;
  while (curr != NULL) {
    if (curr->size >= size + sizeof(Node)) {
      curr->size -= size + sizeof(Node);
      newAllocatedNode = (Node *)((size_t)curr + curr->size + sizeof(Node));
      break;
    }
    prev = curr;
    curr = curr->next;
  }
  if (newAllocatedNode == NULL) {//if the free list has no data segment that satisfies the needs
    newAllocatedNode = allocateNewSpace(prev, size);
  }
  newAllocatedNode->size = size;
  return (void *) ((size_t)newAllocatedNode + sizeof(Node));
}

void ff_free(void *ptr) {
  if (ptr != NULL) {
    Node * newFreeNode = (Node *)((size_t)ptr - sizeof(Node));
    addToFreedList(NULL, newFreeNode);
  }
}

//Best Fit malloc/free
void *bf_malloc(size_t size) {
  initListHead();
  Node * prev = head;
  Node * curr = head->next;
  Node * newAllocatedNode = NULL;
  size_t minSize = SIZE_MAX;
  while (curr != NULL) {
    if (curr->size >= size + sizeof(Node) && curr->size < minSize) {
      curr->size -= size + sizeof(Node);
      newAllocatedNode = (Node *)((size_t)curr + curr->size + sizeof(Node));
      minSize = curr->size;
    }
    prev = curr;
    curr = curr->next;
  }
  if (newAllocatedNode == NULL) {//if the free list has no data segment that satisfies the needs
    newAllocatedNode = allocateNewSpace(prev, size);
  }
  newAllocatedNode->size = size;
  return (void *) ((size_t)newAllocatedNode + sizeof(Node));  
}

void bf_free(void *ptr) {
  ff_free(ptr);
}

unsigned long get_data_segment_size() {
  return (size_t)heapTop - (size_t)head;
}

unsigned long get_data_segment_free_space_size() {
  Node * curr = head;
  unsigned long dataSize = 0;
  while (curr != NULL) {
    dataSize += curr->size;
    curr = curr->next;
  }
  return dataSize;
}
