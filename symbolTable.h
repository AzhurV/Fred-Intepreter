#ifndef SYM_TABLE_H
#define SYM_TABLE_H

#include "sorted_list.h"


typedef enum types_enum {
  Integer, Float, Unknown
} Type;

typedef union val_union {
  int iVal;
  float fVal;
} Value;


typedef struct Symbol_ {
  char* name;
  Type type;
  Value vale;
} Symbol;


typedef List SymTable;


void AddSymbol(SymTable, Symbol);

void GetSymbol(SymTable, char*);

