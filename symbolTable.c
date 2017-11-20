///file:symbolTable.c
///description: functions for the symbol table of the Fred interpreter
///author: avv8047 : Azhur Viano

#include "symbolTable.h"


///Create a new empty symbol table
///@returns a pointer to the new symbol table
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

///Free all memory associated with a table
///@param table a pointer to the table to free
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
    


///Add a new symbol to the table
///@param table the table to add a symbol to
///@param symbol a pointer to the symbol to add
///@returns 1 if the symbol was successfully added, 0 if
///  the symbol already existed in the table
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
///@param table a pointer to the symbol table
///@param symbol a pointer to the symbol to update, including
///  the new symbol information
///@returns 1 if updated successfully, 0 if the symbol was not found
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


///Retrieve a symbol from the table
///@param table a pointer to the symbol table
///@param name the name of the symbol to get
///@returns a pointer to the symbol if found, or NULL otherwise
Symbol* GetSymbol(SymbolTable* table, char* name){
  SymbolNode* cur = table->head;
  while(cur){
    if(strcmp(cur->symbol->name, name) == 0){
      return cur->symbol;
    }
    cur = cur->next;
  }

  return NULL;
}

///Print the symbol table contents to stdout
///@param table a pointer to the table to print
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
      printf("float\t%.3f\n", cur->symbol->value.fVal);
      break;
    default:
      printf("unknown\tunknown\n");
    }
    cur = cur->next;
  }

  return;
}
