#ifndef SYM_TABLE_H
#define SYM_TABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SYM_LEN 7

///Types a symbol can have
typedef enum types_enum {
  Integer, Float, Unknown
} Type;

///Value of a symbol can be either an int or a float
typedef union val_union {
  int iVal;
  float fVal;
} Value;



///Symbol has a name, a type, and a value
typedef struct Symbol_ {
  char* name;
  Type type;
  Value value;
} Symbol;

///Node within the symbol table
typedef struct SymbolNode_ {
  struct SymbolNode_* next;
  Symbol* symbol;
} SymbolNode;


///The symbol table
typedef struct SymbolTable_ {
  SymbolNode* head;
  size_t size;
} SymbolTable;


///Create a new empty symbol table
///@returns a pointer to the new symbol table
SymbolTable* CreateTable(void);


///Free all memory associated with a symbol table
///@param table a pointer to the table to free
void DestroyTable(SymbolTable* table);


///Add a new symbol to the table
///@param table the table to add a symbol to
///@param symbol a pointer to the symbol to add
///@returns 1 if the symbol was successfully added, 0 if
///  the symbol already existed in the table
int AddSymbol(SymbolTable* table, Symbol* symbol);



Symbol* GetSymbol(SymbolTable*, char*);


///Print the symbol table contents to stdout
///@param table a pointer to the table
void dumpTable(SymbolTable* table);

#endif
