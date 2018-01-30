#include "my_malloc.h"
#include <unistd.h>

void deleteFromAllocatedList(Node * toBeDeleted) {
  Node * curr = allocatedListHead;
  while (curr != NULL && curr->next != toBeDeleted) {
    curr = curr->next;
  }
  if (curr != NULL) {
    curr->next = curr->next->next;
  }
}

void initListHead() {
  if (allocatedListHead == NULL) {
    allocatedListHead= sbrk(sizeof(Node));
    allocatedListHead->next = NULL;
    allocatedListHead->size = 0;
    heapTop = allocatedListHead + 1;//exclusive
  }
  if (freedListHead == NULL) {
    freedListHead= sbrk(sizeof(Node));
    freedListHead->next = NULL;
    freedListHead->size = 0;
    heapTop = freedListHead + 1;//exclusive
    freedListTail = freedListHead;
  }
}
  
void addToFreedList(Node * prev, Node * toBeAdded) {
  if (prev == NULL) {
    Node * curr = freedListHead;
    while (curr != NULL && (size_t)curr < (size_t)toBeAdded) {
      prev = curr;
      curr = curr->next;
    }
  }
  Node * newAddedNode = NULL;
  if ((size_t)prev + prev->size + sizeof(Node) == (size_t)toBeAdded) {
    prev->size += toBeAdded->size + sizeof(Node);
    newAddedNode = prev;
  } else {
    toBeAdded->next = prev->next;
    prev->next = toBeAdded;
    newAddedNode = toBeAdded;
  }
  if ((size_t)newAddedNode + newAddedNode->size + sizeof(Node) == (size_t)newAddedNode->next) {
    newAddedNode->size += newAddedNode->next->size + sizeof(Node);
    newAddedNode->next = newAddedNode->next->next;
  }
  if (freedListTail->next != NULL) {
    freedListTail = freedListTail->next;
  }
}

void addToAllocatedList(Node * toBeAdded) {
  Node * curr = allocatedListHead->next;
  Node * prev = allocatedListHead;
  while (curr != NULL && (size_t)curr < (size_t)toBeAdded) {
    prev = curr;
    curr = curr->next;
  }
  toBeAdded->next = prev->next;
  prev->next = toBeAdded;
}

Node * initNewAllocatedNode(Node * newAllocatedNode, size_t size) {
  if (newAllocatedNode == NULL) {//if the free list has no data segment that satisfies the needs
    size_t increment = 0;
    if ((size_t)freedListTail + freedListTail->size + sizeof(Node) == (size_t)heapTop) {
      increment = size + sizeof(Node) - freedListTail->size;//the freedListTail get expanded
      heapTop = (void *)((size_t)sbrk(increment) + increment);
      newAllocatedNode = (Node *)((size_t)heapTop - size - sizeof(Node));
      freedListTail->size = (size_t)heapTop - (size_t)freedListTail - size - 2 * sizeof(Node);
    } else {
      increment = size + sizeof(Node);//a new freeListTail
      Node * potentialFreeNode = heapTop;
      heapTop = (void *)((size_t)sbrk(increment) + increment);
      newAllocatedNode = (Node *)((size_t)heapTop - size - sizeof(Node));
      potentialFreeNode->next = NULL;
      potentialFreeNode->size = (size_t)heapTop - (size_t)potentialFreeNode - sizeof(Node) - size;
      if (potentialFreeNode->size > 0) {
	addToFreedList(freedListTail, potentialFreeNode);
      }
    }
  }
  newAllocatedNode->size = size;
  return newAllocatedNode;
}

//First Fit malloc/free
void *ff_malloc(size_t size) {
  initListHead();
  Node * curr = freedListHead;
  Node * newAllocatedNode = NULL;
  while (curr != NULL) {
    if (curr->size >= size + sizeof(Node)) {
      curr->size -= size + sizeof(Node);
      newAllocatedNode = (Node *)((size_t)curr + curr->size + sizeof(Node));
      break;
    }
    curr = curr->next;
  }
  newAllocatedNode = initNewAllocatedNode(newAllocatedNode, size);
  addToAllocatedList(newAllocatedNode);
  return (void *)(newAllocatedNode + 1);
}

void ff_free(void *ptr) {
  if (ptr != NULL) {
    Node * toBeDeleted = (Node *)((size_t)ptr - (size_t)sizeof(Node));
    deleteFromAllocatedList(toBeDeleted);
    addToFreedList(NULL, toBeDeleted);
  }
}

unsigned long get_data_segment_size() {
  Node * curr = allocatedListHead;
  unsigned long dataSize = 0;
  while (curr != NULL) {
    dataSize += curr->size + sizeof(Node);
    curr = curr->next;
  }
  return dataSize + get_data_segment_free_space_size();
}

unsigned long get_data_segment_free_space_size() {
  Node * curr = freedListHead;
  unsigned long dataSize = 0;
  while (curr != NULL) {
    dataSize += curr->size;
    curr = curr->next;
  }
  return dataSize;
}
