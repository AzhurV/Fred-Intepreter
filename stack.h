#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdbool.h>



typedef struct {
  void** data;
  size_t size;
  size_t capacity;
} Stack;


Stack* CreateStack(void);

bool EmptyStack(Stack*);

void* PopStack(Stack8);

void PushStack(Stack*, void*);

#endif
