#include "processor.h"

//constants for boolean operators in if statements
typedef enum bool_ops {GT, LT, EQ}
  BoolOperator;

static int roundEven(float f){
  float diff = f - (int) f;
  int addend = 1;

  //make diff positive
  if(diff < 0){
    diff *= -1.0f;
    addend = -1;
  }
  
  if(diff < 0.5f){
    return (int) f;
  }
  else if(diff > 0.5f){
    return (int) (f + addend);
  }
  else{
    int remainder = ((int) (f + 1.0f)) % 2;
    if(remainder){
      return (int) (f - addend);
    }
    else{
      return (int) (f + addend);
    }
  }
}

//Process a file of symbols and store them in the table
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
      fprintf(stderr, "Error processing symbol file: unknown type - %s\n", tok);
      free(symbol);
      continue;
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
    fprintf(stderr, "Unknown type: %s\n", tok);
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
static void processLet(SymbolTable* table, char* expression){
  const char* delim = " ,\t\n";
  char* tok;
  Symbol* symbol;
  Token* returnToken;

  tok = strtok(expression, delim);
  symbol = GetSymbol(table, tok);

  if(!symbol){
    fprintf(stderr, "let error: no symbol %s in table\n", tok);
    return;
  }

  ///skip past :=
  tok = strtok(NULL, delim);
  //get rest of line to evaluate
  tok = strtok(NULL, "\n");
  

  returnToken = evaluateExpression(table, tok);

  //error processing let expression; return
  if(!returnToken){
    return;
  }

  if(symbol->type == Integer){
    if(returnToken->valType != Integer){
      symbol->value.iVal = roundEven(returnToken->value.fVal);
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


//Process an if statement
//@param table a pointer to the symbol table to use
//@param clause the conditional clause to use
//@returns 1 if statement is true, else 0 
static int processIf(SymbolTable* table, char* clause){
  char* compOperator = clause;
  BoolOperator operator;
  //indicates whether or not to invert the result with the ! operator
  int invert = 0;
  //truth value to be returned
  int returnVal = 0;
  //find the boolean operator in the clause
  while(*compOperator != '!' && *compOperator != '='
	&& *compOperator != '>' && *compOperator != '<'){
    compOperator++;
  }

  if(*compOperator == '!'){
    invert = 1;
    compOperator = '\0';
    compOperator++;
  }
  switch(*compOperator){
  case '=':
    operator = EQ;
    break;
  case '<':
    operator = LT;
    break;
  case '>':
    operator = GT;
    break;
  default:
    fprintf(stderr, "Unknown boolean operator %c\n", *compOperator);
    return 0;
  }

  *compOperator = '\0';
  compOperator += 2;

  Token* leftResult = evaluateExpression(table, clause);
  Token* rightResult = evaluateExpression(table, compOperator);
  int isFloat = 0;

  //perform type conversions if necessary
  if(leftResult->valType != rightResult->valType){
    if(leftResult->valType == Float){
      rightResult->valType = Float;
      rightResult->value.fVal = (float) rightResult->value.iVal;
    }
    else{
      leftResult->valType = Float;
      leftResult->value.fVal = (float) leftResult->value.iVal;
    }
    isFloat = 1;
  }

  //perform comparision and set returnVal
  switch(operator){
  case EQ:
    if(isFloat){
      returnVal = (leftResult->value.fVal == rightResult->value.fVal);
    }
    else{
      returnVal = (leftResult->value.iVal == rightResult->value.iVal);
    }
    break;
  case GT:
    if(isFloat){
      returnVal = (leftResult->value.fVal > rightResult->value.fVal);
    }
    else{
      returnVal = (leftResult->value.iVal > rightResult->value.iVal);
    }
    break;
  case LT:
    if(isFloat){
      returnVal = (leftResult->value.fVal < rightResult->value.fVal);
    }
    else{
      returnVal = (leftResult->value.iVal < rightResult->value.iVal);
    }
    break;
  default:
    break;
  }

  free(leftResult);
  free(rightResult);
  //! operator, so invert truth value
  if(invert){
    return (!returnVal);
  }
  
  return returnVal;
}



///Process a print statement
static void processPrint(void){
  //get entire line
  char* str = strtok(NULL, "\n");
  if(!str){
    return;
  }
  
  int i;

  for(i = 0; str[i] != '\0'; i++){
    if(str[i] == '\\'){
      i++;
      switch(str[i]){
      case 'n':
	putchar((unsigned char) '\n');
	break;
      case 't':
	putchar((unsigned char) '\t');
	break;
      case '\\':
	putchar((unsigned char) '\t');
	break;
      default:
	putchar((unsigned char) ' ');
      }
    }
    else{
      putchar((unsigned char) str[i]);
    }
  }

  return;
}


//Process a display statement
//@param table a pointer to the symbol table to use
//@param expression the string containing the elements to display
static void processDisplay(SymbolTable* table, char* expression){
  char* delim = " \t,\n";

  char* tokString;

  int multiplier = 1;

  for(tokString = strtok(expression, delim);
      tokString;
      tokString = strtok(NULL, delim)){
    //token is a variable identifier
    if(isalpha(tokString[0])){
      Symbol* symbol = GetSymbol(table, tokString);
      if(symbol){
	if(symbol->type == Float){
	  printf(" %.3f ", symbol->value.fVal);
	}
	else{
	  printf(" %d ", symbol->value.iVal);
	}
      }
      else{
	fprintf(stderr, "\nError: symbol %s not found in symbol table\n", tokString);
      }
    }
    //token is a number
    else if(isdigit(tokString[0]) || tokString[0] == '-'){
      if(tokString[0] == '-'){
	multiplier = -1;
	tokString++;
      }
      if(isFloat(tokString)){
	float fval = atof(tokString);
        printf(" %.3f ", multiplier * fval);
      }
      else{
	int ival = atoi(tokString);
	printf(" %d ", multiplier * ival);
      }
    }
    else{
      fprintf(stderr, "\nError: invalid token %s\n", tokString);
    }
  }
  printf("\n");
  return;
}


static void executeStatement(SymbolTable* table, char* statement){

  const char* delim = " \t\n";
  char* tok = strtok(statement, delim);

  if(strcmp("define", tok) == 0){
    processDefine(table);
  }
  else if(strcmp("let", tok) == 0){
    char* expression = strtok(NULL, "\n");
    processLet(table, expression);
  }
  else if(strcmp("if", tok) == 0){
    char* ifClause = strtok(NULL, "\n");
    char* thenClause = strstr(ifClause, " then ");
    if(!thenClause){
      fprintf(stderr, "No then clause found for if clause\n");
      return;
    }
    //seperate then clause from if clause so processing with strtok behaves correctly
    *thenClause = '\0';
    //move past then statement to beginning of clause
    thenClause += 6;
    if(processIf(table, ifClause)){
      executeStatement(table, thenClause);
    }
  }
  else if(strcmp("prt", tok) == 0){
    processPrint();
  }
  else if(strcmp("display", tok) == 0){
    char* expression = strtok(NULL, "\n");
    processDisplay(table, expression);
  }
  //unknown statement keyword; print error and do nothing
  else{
    fprintf(stderr, "Unknown statement %s\n", tok);
  }
}


  ///Process statements from an input stream
  ///@param table the symbol table to use while processing
  ///@param input the input stream to read from
  void processStatements(SymbolTable* table, FILE* input){
    char* line = NULL;
    size_t len = 0;
  
    printf(">");

    //get lines from input; lines are dynamically allocated by getline
    while(getline(&line, &len, input) != -1){
      printf(":::%s\n", line);

      executeStatement(table, line);

      //free dynamically allocated line
      free(line);
      line = NULL;

      printf(">");
    }

    free(line);

    return;
  }
