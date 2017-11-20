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
    else if(strcmp("float", tok) == 0){
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
