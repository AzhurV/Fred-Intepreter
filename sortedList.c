#include "sortedList.h"


typedef struct Node_ {
  void* data;
  Node_* next;
} Node;


List* CreateList(void){
  List* list = malloc(sizeof(List));
  list->head = NULL;
  list->size = 0;

  return list;
}


//TODO: add correct functionality for equal data
void AddElem(List* list, void* data){
  Node* newNode = malloc(sizeof(Node));
  newNode->data = elem;

  if(!list->head || list->comp(list->head->data, newNode->data) < 0){
    newNode->next = list->head;
    list->head = newNode;
    return;
  }

  Node* prev = list->head;
  Node* cur = list->head->next;

  while(cur && comp(cur->data, newNode->data) < 0){
    prev = cur;
    cur = cur->next;
  }

  prev->next = newNode;
  newNode->next = cur;

  return;
}
  

void* GetElem(List* list, void* data){
  Node* cur = list->head;
  while(cur){
    if(comp(cur->data, data) == 0){
      return cur->data;
    }
  }

  return NULL;
}

