///file:symbolTable.c
///description: functions for the symbol table of the Fred interpreter
///author: avv8047 : Azhur Viano


#include "symbolTable.h"


///Create a new table
SymbolTable* CreateTable(void){
  SymbolTable* table = malloc(sizeof(SymbolTable));
  
  table->head = NULL;
  table->size = 0;

  return table;
}


///Free all memory associated with a symbol
///@param symbol a pointer to the symbol to free
static void DestroySymbol(Symbol* symbol){
  free(symbol->name);
  free(symbol);
  return;
}


///Destroy a table
void DestroyTable(SymbolTable* table){
  SymbolNode* cur = table->head;
  SymbolNode* next;

  while(cur){
    next = cur->next;
    DestroySymbol(cur->symbol);
    free(cur);
    cur = next;
  }

  free(table);
}
    

///Add a symbol to the table
int AddSymbol(SymbolTable* table, Symbol* symbol){
  SymbolNode* cur = table->head;
  SymbolNode* newNode = malloc(sizeof(SymbolNode));
  newNode->symbol = symbol;
  int result = 1;
  
  if(!cur || (result = strcmp(cur->symbol->name, symbol->name)) >= 0){
    if(result == 0){
      free(newNode);
      return 0;
    }
    newNode->next = cur;
    table->head = newNode;
    return 1;
  }

  SymbolNode* prev = cur;
  cur = cur->next;

  while(cur && (result = strcmp(cur->symbol->name, symbol->name)) < 0){
    prev = cur;
    cur = cur->next;
  }

  if(result == 0){
    free(newNode);
    return 0;
  }

  newNode->next = cur;
  prev->next = newNode;

  return 1;
}
  

///Update a symbol in the table
int updateSymbol(SymbolTable* table, Symbol* symbol){
  SymbolNode* cur = table->head;

  while(cur){
    if(strcmp(cur->symbol->name, symbol->name) == 0){
      free(cur->symbol);
      cur->symbol = symbol;
      return 1;
    }
    cur = cur->next;
  }

  return 0;
}


///Get a symbol from the table
Symbol* GetSymbol(SymbolTable* table, char* name){
  SymbolNode* cur = table->head;
  while(cur){
if(strncmp(cur->symbol->name, name, MAX_SYM_LEN) == 0){
      return cur->symbol;
    }
    cur = cur->next;
  }

  return NULL;
}


///Dump the table and its contents to standard output
void dumpTable(SymbolTable* table){
  SymbolNode* cur = table->head;

  printf("Symbol Table Contents\n");
  printf("Name\tType\tValue\n");
  printf("=====================\n");

  while(cur){
    printf("%s\t", cur->symbol->name);
    switch(cur->symbol->type){
    case Integer:
      printf("integer\t%d\n", cur->symbol->value.iVal);
      break;
    case Float:
      printf("real\t%.3f\n", cur->symbol->value.fVal);
      break;
    default:
      printf("unknown\tunknown\n");
    }
    cur = cur->next;
  }

  return;
}
