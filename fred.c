#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "symbolTable.h"
#include "processor.h"

///Print the usage message for the main program
void printUsage(){
  fprintf(stderr, "Usage:  fred [ -s symbol-table-file ]"
	  "[ -f fred-program-file ]");
  return;
}


int main(int argc, char** argv){
  int c;
  SymbolTable* table = CreateTable();
  FILE* input = NULL;
  FILE* symbolInput = NULL;
  

  while((c = getopt(argc, argv, "f:s:")) != -1){
    switch(c){
    case 'f':
      input = fopen(optarg, "r");
      if(!input){
	fprintf(stderr, "Error opening program file %s\n", optarg);
	return EXIT_FAILURE;
      }
      break;
    case 's':
      symbolInput = fopen(optarg, "r");
      if(!symbolInput){
	fprintf(stderr, "Error in opening symbol file %s\n", optarg);
	return EXIT_FAILURE;
      }
      processSymbolFile(table, symbolInput);
      fclose(symbolInput);
      symbolInput = NULL;
      break;
    default:
      printUsage();
      return EXIT_FAILURE;
    }
  }

  if(!input){
    input = stdin;
  }

  processStatements(table, input);
  
  dumpTable(table);
  DestroyTable(table);
}
