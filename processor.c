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


///Process a define statement, putting each symbol into the table
///with an initial value of 0
static void processDefine(SymbolTable* table){
  const char* delim = " ,\t\n";
  char* tok;
  Type type;
  Symbol* symbol;

  tok = strtok(NULL, delim);

  if(strcmp("integer", tok) == 0){
    type = Integer;
  }
  else if(strcmp("float", tok) == 0){
    type = Float;
  }
  else{
    type = Unknown;
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

    if(!AddSymbol(table, symbol)){
      free(symbol->name);
      free(symbol);
      fprintf(stderr, "Symbol %s already exists in table\n", tok);
    }
  }
  return;
}


static void processLet(SymbolTable* table){
  return;
}


static void processIf(SymbolTable* table){
  return;
}


static void processPrint(SymbolTable* table){
  return;
}


static void processDisplay(SymbolTable* table){
  return;
}


void processStatements(SymbolTable* table, FILE* input){
  char* line = NULL;
  size_t len = 0;
  
  char* tok;
  const char* delim = " \t";

  while(getline(&line, &len, input) != -1){
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
  }

  free(line);

  return;
}
