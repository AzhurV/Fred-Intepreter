///file:fred.c
///description:main program file for the fred interpreter
///  reads a symbol file and program file if provided and
///  processes statements
///author: avv8047 : Azhur Viano


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
  

  //Check for the correct number of arguments
  if(argc - 1 % 2 != 0 || argc > 5){
    fprintf(stderr, "Wrong number of arguments\n");
    printUsage();
    return EXIT_FAILURE;
  }
  
  while((c = getopt(argc, argv, "f:s:")) != -1){
    switch(c){
    case 'f':
      //Check if a program file was already provided
      if(input){
	fprintf(stderr, "Duplicate argument for program file: %s\n", optarg);
	return EXIT_FAILURE;
      }
      input = fopen(optarg, "r");
      if(!input){
	fprintf(stderr, "Error opening program file %s\n", optarg);
	return EXIT_FAILURE;
      }
      break;
    case 's':
      //Check if a symbol file was already provided
      if(symbolInput){
	fprintf(stderr, "Duplicate argument for symbol file: %s\n", optarg);
	return EXIT_FAILURE;
      }
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

  //Read from stdin if no program file was provided
  if(!input){
    input = stdin;
  }

  processStatements(table, input);
  
  dumpTable(table);
  DestroyTable(table);
}
