///file:stack.h
///description:declarations for an stack data structure
///author: avv8047 : Azhur Viano
#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdbool.h>


///Stack struct has an array of data, a size, and a capacity
typedef struct {
  void** data;
  size_t size;
  size_t capacity;
} Stack;


///Create a new empty stack
///@returns a pointer to the new empty stack
Stack* CreateStack(void);

///Delete the stack
///@param stack the stack to be deleted
void DeleteStack(Stack* stack);

///Check whether the stack is empty
///@param stack a pointer to the stack to check
///@returns whether or not the stack is empty
int EmptyStack(Stack* stack);

///Pop the top element of the stack
///@param stack a pointer to the stack to op from
void* PopStack(Stack* stack);

///Push an element onto the stack
///@param stack a pointer to the stack
///@param data the data to push
void PushStack(Stack* stack, void* data);

#endif
