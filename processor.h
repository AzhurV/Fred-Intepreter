#ifndef PROCESSOR_H
#define PROCESSOR_H
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "symbolTable.h"
#include "evaluate.h"

void processSymbolFile(SymbolTable* table, FILE* symbolFile);

void processStatements(SymbolTable* table, FILE* input);

#endif
