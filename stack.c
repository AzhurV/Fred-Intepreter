///file:stack.c
///description: implementation of an abstract stack data type
///author: avv8047 : Azhur Viano


#include "stack.h"
#include <assert.h>

#define INITIAL_CAPACITY 20


///Create a new stack
Stack* CreateStack(void){
  Stack* stack = malloc(sizeof(Stack));
  stack->size = 0;
  stack->capacity = INITIAL_CAPACITY;
  stack->data = malloc(sizeof(void*) * INITIAL_CAPACITY);

  return stack;
}


///Destroy a stack
void DestroyStack(Stack* stack){
  free(stack->data);
  free(stack);
  return;
}


///Check whether a stack is empty
int EmptyStack(Stack* stack){
  return (stack->size == 0);
}


///Pop the top of the stack and return it
void* PopStack(Stack* stack){
  void* data;
  
  assert(!EmptyStack(stack));
  
  stack->size--;
  data = stack->data[stack->size];

  return data;
}


///Push onto the top of the stack
void PushStack(Stack* stack, void* data){
  if(stack->size == stack->capacity){
    stack->data = realloc(stack->data, stack->size * 2);
    stack->capacity = stack->size * 2;
  }

  stack->data[stack->size] = data;
  stack->size++;

  return;
}

  
