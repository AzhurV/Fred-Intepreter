#ifndef PROCESSOR_H
#define PROCESSOR_H
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "symbolTable.h"
#include "stack.h"

void processSymbolFile(SymbolTable* table, FILE* symbolFile);

#endif
