///file:processor.h
///description:function declarations for processing
///  Fred program statements
///author: avv8047 : Azhur Viano


#ifndef PROCESSOR_H
#define PROCESSOR_H
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "symbolTable.h"
#include "evaluate.h"


//Process a file of symbols and store them in the table
//@param table the table to store symbols in
//@param symbolfile the filestream to read symbols from
void processSymbolFile(SymbolTable* table, FILE* symbolFile);


///Process statements from an input stream
///@param table the symbol table to use while processing
///@param input the input stream to read from
void processStatements(SymbolTable* table, FILE* input);

#endif
