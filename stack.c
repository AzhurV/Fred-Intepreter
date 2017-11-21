///file:stack.c
///description: implementation of an abstract stack data type
///author: avv8047 : Azhur Viano


#include "stack.h"
#include <assert.h>

#define INITIAL_CAPACITY 20


///Create a new empty stack
///@returns a pointer to the new empty stack
Stack* CreateStack(void){
  Stack* stack = malloc(sizeof(Stack));
  stack->size = 0;
  stack->capacity = INITIAL_CAPACITY;
  stack->data = malloc(sizeof(void*) * INITIAL_CAPACITY);

  return stack;
}


///Delete the stack
///@param stack pointer to the stack to be deleted
void DestroyStack(Stack* stack){
  free(stack->data);
  free(stack);
  return;
}

///Check whether the stack is empty
///@param stack a pointer to the stack to check
///@returns whether or not the stack is empty
int EmptyStack(Stack* stack){
  return (stack->size == 0);
}


///Pop the top element of the stack
///@param stack a pointer to the stack to op from
void* PopStack(Stack* stack){
  void* data;
  
  assert(!EmptyStack(stack));
  
  stack->size--;
  data = stack->data[stack->size];

  return data;
}


///Push an element onto the stack
///@param stack a pointer to the stack
///@param data the data to push
void PushStack(Stack* stack, void* data){
  if(stack->size == stack->capacity){
    stack->data = realloc(stack->data, stack->size * 2);
    stack->capacity = stack->size * 2;
  }

  stack->data[stack->size] = data;
  stack->size++;

  return;
}

  
