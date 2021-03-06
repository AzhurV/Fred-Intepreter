///file:processor.c
///description:functions for reading and executing Fred program
///  statements
///@author: avv8047 : Azhur Viano


#include "processor.h"

//types for boolean operators in if statements
typedef enum bool_ops {GT, LT, EQ}
  BoolOperator;


///Round a float to an int using the even rounding method
///@param f the float number to round
///@returns f rounded to an integer using even rounding
static int roundEven(float f){
  //decimal part of the float
  float diff = f - (int) f;
  //used to round number away from 0
  int addend = 1;

  //make diff positive
  if(diff < 0){
    diff *= -1.0f;
    //addend must be negative to round a negative number away from 0
    addend = -1;
  }

  //round towards zero
  if(diff < 0.5f){
    return (int) f;
  }
  
  //round away from 0
  else if(diff > 0.5f){
    return (int) (f + addend);
  }
  
  //round to the nearest even int
  else{
    int remainder = ((int) (f + addend)) % 2;

    //integer towards 0 is even, so round to that
    if(remainder){
      return (int) f;
    }
    //integer away from 0 is even, so round to that
    else{
      return (int) (f + addend);
    }
  }
}


///Process a symbol file, storing the symbols and their values
///  in the table
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
    symbol->name = strndup(tok, MAX_SYM_LEN);

    tok = strtok(NULL, delim);
    
    if(symbol->type == Integer){
      symbol->value.iVal = (int) strtol(tok, NULL, 10);
    }
    else if(symbol->type == Float){
      symbol->value.fVal = strtof(tok, NULL);
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

  if(!tok){
    fprintf(stderr, "define error: no type or variable provided\n");
    return;
  }

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
    symbol = malloc(sizeof(Symbol));
    symbol->name = strndup(tok, MAX_SYM_LEN);
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

  if(!tok){
    fprintf(stderr, "Error: no symbol provided to let\n");
    return;
  }
  
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


///Process an if statement
///@param table a pointer to the symbol table to use
///@param clause the conditional clause to use
///@returns 1 if statement is true, else 0 
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
    *compOperator = '\0';
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
  compOperator++;

  
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
  //if ! was used, invert the truth value
  if(invert){
    return (!returnVal);
  }
  
  return returnVal;
}


///Validate that a print string is enclosed in quotes. Place a null terminator at the closing quote
///  and return the index of the beginning of the string if the string is properly quoted
///@parameter the ascii string to validate
///@returns the starting position of the string, or -1 if the string is not properly quoted
static int validatePrtString(char* str){
  int i = 0;
  int start;
  char mark;

  //move past whitespace
  while(str[i] == ' ' || str[i] == '\t'){
    i++;
  }

  mark = str[i];
  if(mark != '\'' && mark != '\"'){
    //no quote at beginning
    fprintf(stderr, "Error: no opening quotes for print statement string\n");
    return -1;
  }

  i++;
  //save start index of actual string
  start = i;

  i = strlen(str) - 1;
  
  while(str[i] == ' ' || str[i] == '\t'){
    i++;
  }
  
  if(str[i] != '\'' && str[i] != '\"'){
    fprintf(stderr, "Error: no closing quotes for print statement string\n");
    return -1;
  }

  if(mark != str[i]){
    //quotes don't match
    fprintf(stderr, "Error: mismatching quotes in print statement\n");
    return -1;
  }

  //null terminate to mark the end of the string
  str[i] = '\0';
  
  return start;
}


///Process a print statement
static void processPrint(void){
  //get entire line
  char* str = strtok(NULL, "\n");
  if(!str){
    return;
  }

  int i = validatePrtString(str);

  if(i == -1){
    return;
  }
    

  for(; str[i]; i++){
    if(str[i] == '\\'){
      i++;
      switch(str[i]){
      case 'n':
	//newline escape
	putchar((unsigned char) '\n');
	break;
      case 't':
	//tab escape
	putchar((unsigned char) '\t');
	break;
      case '\\':
	//backslack escape
	putchar((unsigned char) '\t');
	break;
      default:
	//unknown escape; just print a space
	putchar((unsigned char) ' ');
      }
    }
    else{
      //normal ASCII character, just put on the output stream
      putchar((unsigned char) str[i]);
    }
  }

  return;
}


///Process a display statement
///@param table a pointer to the symbol table to use
///@param expression the string containing the elements to display
static void processDisplay(SymbolTable* table, char* expression){
  //delimiters for strtok
  char* delim = " \t,\n";

  char* tokString;

  //used for correctly printing negative constants
  int multiplier;

  for(tokString = strtok(expression, delim);
      tokString;
      tokString = strtok(NULL, delim)){
    //reset multiplier each time
    multiplier = 1;
    
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
    //token is a numeric constant
    else if(isdigit(tokString[0]) || tokString[0] == '-'){
      if(isFloat(tokString)){
	//float constant
	float fval = strtof(tokString, NULL);
        printf(" %.3f ", multiplier * fval);
      }
      else{
	//integer constant
	int ival = (int) strtol(tokString, NULL, 10);
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


///Execute a Fred statement
///@param table the symbol table to use
///@param statement the statement to execute
static void executeStatement(SymbolTable* table, char* statement){

  const char* delim = " \t\n";
  char* tok = strtok(statement, delim);
  
  if(!tok){
    return;
  }

  

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



///Process Fred statements from an input
void processStatements(SymbolTable* table, FILE* input){
  char* line = NULL;
  size_t len = 0;
  
  printf(">");

  //get lines from input; lines are dynamically allocated by getline
  while(getline(&line, &len, input) != -1){
    printf(":::%s\n", line);

    if(strnlen(line, 1) != 0){
      executeStatement(table, line);
    }

    //free dynamically allocated line
    free(line);
    line = NULL;

    printf(">");
  }
  
  printf("\n");

  free(line);

  return;
}
