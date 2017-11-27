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


//Set up symbol table, reading symbols from the symbol file if provided, 
//  and then process statements from standard input or
//  a program fileif provided
int main(int argc, char** argv){
  //used to store options from getop
  int c;
  //table to use while processing
  SymbolTable* table = CreateTable();
  //stream for input statements
  FILE* input = NULL;
  //stream for symbols from a file
  FILE* symbolInput = NULL;
  

  //Check for the correct number of arguments
  if((argc - 1) % 2 != 0 || argc > 5){
    fprintf(stderr, "Wrong number of arguments\n");
    printUsage();
    return EXIT_FAILURE;
  }
  
  while((c = getopt(argc, argv, "f:s:")) != -1){
    switch(c){
    //program file
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
    //symbol file
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
      //read symbols from the file into the table
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

  //process program statements until EOF is reached 
  processStatements(table, input);

  //print table contents
  dumpTable(table);
  
  DestroyTable(table);

  //if a file was opened for reading statements from, close it
  if(input != stdin){
    fclose(input);
  }
}
