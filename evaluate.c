///file:evaluate.c
///description:functions for evaluating arithmetic and boolean expressions
///author: Azhur Viano : avv8047


#include "evaluate.h"

static Token** convertToPostfix(SymbolTable* table){
  Token** postExpression = malloc(sizeof(Token*) * INIT_SIZE);
  Stack* stack = CreateStack();

  char* tokString;
  const char* delim = " \t\n";

  Token* token;

  //while there are still tokens remaining
  while((tokString == strtok(NULL, delim)){

    }

}
Value evaluateExpression(SymbolTable* table){
  
  Token** postFixExpression = convertToPostfix(table);

  
}
