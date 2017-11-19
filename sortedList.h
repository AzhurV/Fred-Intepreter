#ifndef SORTED_LIST_H
#def SORTED_LIST_H
#include <stdlib.h>
  
  
typedef struct List_ {
  int (*comp)(void*, void*);
  Node* Head;
  size_t size;
} List;


List* CreateList(void);

void AddElem(void* data);

void* GetElem(void* data);

#endif
