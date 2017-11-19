#ifndef SYM_TABLE_H
#define SYM_TABLE_H

#include <stdlib.h>


///Node within the symbol table
typedef struct SymbolNode_ {
  SymbolNode_* next;
  Symbol symbol;
} SymbolNode;


///The symbol table
typedef struct SymbolTable_ {
  SymbolNode* head;
  size_t size;
} SymbolTable;


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


///Create a new empty symbol table
///@returns a pointer to the new symbol table
SymbolTable* CreateTable(void);


///Add a new symbol to the table
///@param table the table to add a symbol to
///@param symbol a pointer to the symbol to add
///@returns 1 if the symbol was successfully added, 0 if
///  the symbol already existed in the table
void AddSymbol(SymbolTable*, Symbol*);


///Update a symbol in the table
///@param table a pointer to the symbol table
///@param symbol a pointer to the symbol to update, including
///  the new symbol information
///@returns 1 if updated successfully, 0 if the symbol was not found
Symbol* GetSymbol(SymbolTable*, char*);
