#include "stack.h"
#include <assert.h>

#define INITIAL_CAPACITY 20


Stack* CreateStack(void){
  Stack* stack = malloc(sizeof(stack));
  stack->size = 0;
  stack->capacity = INITIAL_CAPACITY;
  stack->data = malloc(sizeof(void*) * 20);

  return stack;
}


bool EmptyStack(Stack* stack){
  return (stack->size == 0);
}


void* PopStack(Stack* stack){
  void* data;
  
  assert(!EmptyStack(stack));
  
  stack->size--;
  data = stack->data[stack->size];

  return data;
}


void PushStack(Stack* stack, void* data){
  if(stack->size == stack->capacity){
    stack->data = realloc(stack->data, stack->size * 2);
    stack->capacity = stack->size * 2;
  }

  stack->data[stack->size] = data;
  stack->size++;

  return;
}

  
