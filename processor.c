#include "processor.h"

void processSymbolFile(SymbolTable* table, FILE* symbolFile){
  const char* delim = " \t";
  char* line = NULL;
  size_t len = 0;

  
  Symbol* symbol;
  
  char* tok;

  

  while(getline(&line, &len, symbolFile) != -1){
    symbol = malloc(sizeof(Symbol));

    tok = strtok(line, delim);

    if(strcmp("integer", tok) == 0){
      symbol->type = Integer;
    }
    else if(strcmp("real", tok) == 0){
      symbol->type = Float;
    }
    else{
      symbol->type = Unknown;
    }

    tok = strtok(NULL, delim);
    symbol->name = strdup(tok);

    tok = strtok(NULL, delim);
    
    if(symbol->type == Integer){
      symbol->value.iVal = atoi(tok);
    }
    else if(symbol->type == Float){
      symbol->value.fVal = atof(tok);
    }

    AddSymbol(table, symbol);
    
    free(line);
    line = NULL;

  }

  free(line);

  return;
}


///Process a define statement, putting each symbol into the table
///with an initial value of 0
///@param table a pointer to the sybol table to use
static void processDefine(SymbolTable* table){
  const char* delim = " ,\t\n";
  char* tok;
  Type type;
  Symbol* symbol;

  tok = strtok(NULL, delim);

  if(strcmp("integer", tok) == 0){
    type = Integer;
  }
  else if(strcmp("real", tok) == 0){
    type = Float;
  }
  else{
    fprintf(stderr, "Unknown type: %s\n", strtok);
    return;
  }

  while((tok = strtok(NULL, delim)) != NULL){
    //move past white space and commas or quit if we reach the end
    while(tok && !tok[0]){
      tok = strtok(NULL, delim);
    }
    if(tok == NULL){
      return;
    }
    symbol = malloc(sizeof(Symbol));
    symbol->name = strdup(tok);
    symbol->type = type;
    if(type == Integer){
      symbol->value.iVal = 0;
    }
    else{
      symbol->value.fVal = 0;
    }

    ///Symbol already exists
    if(!AddSymbol(table, symbol)){
      free(symbol->name);
      free(symbol);
      fprintf(stderr, "Symbol %s already exists in table\n", tok);
    }
  }
  return;
}


///Process a let statement
///@param table a pointer to the symbol table to use
static void processLet(SymbolTable* table){
  const char* delim = " ,\t\n";
  char* tok;
  Symbol* symbol;
  Token* returnToken;

  tok = strtok(NULL, delim);
  symbol = GetSymbol(table, tok);

  if(!symbol){
    fprintf(stderr, "let error: no symbol %s in table\n", tok);
    return;
  }

  ///skip past :=
  tok = strtok(NULL, delim);

  returnToken = evaluateExpression(table);

  if(symbol->type == Integer){
    if(returnToken->valType != Integer){
      symbol->value.iVal = (int) returnToken->value.fVal;
    }
    else{
      symbol->value.iVal = returnToken->value.iVal;
    }
  }
  else{
    if(returnToken->valType != Float){
      symbol->value.fVal = (float) returnToken->value.iVal;
    }
    else{
      symbol->value.fVal = returnToken->value.fVal;
    }
  }

  free(returnToken);
  return;
}


///Process an if statement
///@param table a pointer to the symbol table to use
static void processIf(SymbolTable* table){
  return;
}


///Process a print statement
///@param table a pointer to the symbol table to use
static void processPrint(SymbolTable* table){
  return;
}


///Process a display statement
///@param table a pointer to the symbol table to use
static void processDisplay(SymbolTable* table){
  return;
}


///Process statements from an input stream
///@param table the symbol table to use while processing
///@param input the input stream to read from
void processStatements(SymbolTable* table, FILE* input){
  char* line = NULL;
  size_t len = 0;
  
  char* tok;
  const char* delim = " \t";

  printf(">");
  
  while(getline(&line, &len, input) != -1){
    printf(":::%s\n", line);
    
    tok = strtok(line, delim);

    if(strcmp("define", tok) == 0){
      processDefine(table);
    }
    else if(strcmp("let", tok) == 0){
      processLet(table);
    }
    else if(strcmp("if", tok) == 0){
      processIf(table);
    }
    else if(strcmp("prt", tok) == 0){
      processPrint(table);
    }
    else if(strcmp("display", tok) == 0){
      processDisplay(table);
    }
    else{
      fprintf(stderr, "Unknown statement %s\n", tok);
    }

    free(line);
    line = NULL;

    printf(">");
  }

  free(line);

  return;
}
