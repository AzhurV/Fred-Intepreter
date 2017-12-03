///file:evaluate.c
///description:functions for evaluating arithmetic and boolean expressions
///author: avv8047 : Azhur Viano


#include "evaluate.h"


//struct to represent a sequence of tokens
typedef struct TokenList_ {
  //sequence of tokens
  Token** list;
  //number of tokens in sequence
  size_t size;
  //max capacity of sequence
  size_t capacity;
} TokenList;


//Create a new token list
//@returns a new token list
static TokenList* CreateTokenList(void){
  TokenList* tokList = malloc(sizeof(TokenList));
  tokList->size = 0;
  tokList->capacity = INITIAL_SIZE;
  tokList->list = malloc(sizeof(Token*) * INITIAL_SIZE);

  return tokList;
}


//Free a token list, including the memory allocated for the tokens
//@param tokList the list of tokens to destroy
static void DestroyTokenList(TokenList* tokList){
  size_t i;
  for(i = 0; i < tokList->size; i++){
    free(tokList->list[i]);
  }
  free(tokList->list);
  free(tokList);
  return;
}


//Add a token to the list
//@param tokList the list of tokens to add to
//@param token the token to add
static void AddToken(TokenList* tokList, Token* token){
  tokList->list[tokList->size] = token;
  tokList->size++;

  if(tokList->size == tokList->capacity){
    tokList->list = realloc(tokList->list, tokList->capacity * 2);
    tokList->capacity *= 2;
  }

  return;
}


//Check whether a string is a float
int isFloat(char* str){
  int i;

  for(i = 0; str[i]; i++){
    if(str[i] == '.'){
      return 1;
    }
  }
  return 0;
}


//Check whether two floats can have the modulo operation performed, i.e
//  that the fractional part of both is 0
//@param dividend the dividend token
//@param divisor the divisor token
//@returns 1 if modulo can be performed, 0 otherwise
static int verifyModulo(Token* dividend, Token* divisor){
  float f = dividend->value.fVal;
  if((f - (int) f) != 0){
    return 0;
  }
  f = divisor->value.fVal;
  if((f - (int) f) != 0){
    return 0;
  }
  return 1;
}


//Convert a string to a sequence of tokens in postfix notation
//@param table the symbol table to use
//@param expression the expression as a null-terminated string
//@returns a sequence of tokens in postfix notation or NULL if any token is not recognized
static TokenList* convertToPostfix(SymbolTable* table, char* expression){
  //used as the output for the postfix token sequence
  TokenList* postExpression = CreateTokenList();
  //stack to push operators on
  Stack* stack = CreateStack();

  //string for the next token
  char* tokString;
  const char* delim = " \t\n";
  char firstCh;

  //used to store tokens that will be appended to the output list
  Token* token = NULL;
  //used to store tokens popped from the stack
  Token* tempToken = NULL;

  //while there are still tokens remaining, read the next
  for(tokString = strtok(expression, delim);
      tokString;
      tokString = strtok(NULL, delim)){
    firstCh = tokString[0];
    token = malloc(sizeof(Token));
      

    //token is a number
    if(isdigit(firstCh)){
      token->type = Operand;
      if(isFloat(tokString)){
	token->valType = Float;
	token->value.fVal = atof(tokString);
      }
      else{
	token->valType = Integer;
	token->value.iVal = atoi(tokString);
      }
      AddToken(postExpression, token);
    }
    //token is a symbol identifier
    else if(isalpha(firstCh)){
      Symbol* symbol = GetSymbol(table, tokString);
      //symbol does not exist in table
      if(!symbol){
	fprintf(stderr, "Error: symbol %s not found in table\n",
		tokString);
	free(token);
	DestroyTokenList(postExpression);
	return NULL;
      }
      token->valType = symbol->type;
      token->value = symbol->value;
      token->type = Operand;
      AddToken(postExpression, token);
    }
    //token is an operator or parenthesis
    else{
      switch(firstCh){
      case '(':
	token->type = LParenthesis;
	token->valType = Integer;
	token->value.iVal = '(';
	PushStack(stack, (void*) token);
	break;
      case ')':
	free(token);
	token = (Token*) PopStack(stack);
	//pop operators from the stack until the left paranthesis is reached
	while(token->type != LParenthesis){
	  AddToken(postExpression, token);
	  token = (Token*) PopStack(stack);
	}
	//free the left parenthesis token
	free(token);
	break;
      case '+':
      case '-':
	token->type = Operator;
	token->valType = Integer;
	token->value.iVal = firstCh;
	while(!EmptyStack(stack)){
	  tempToken = PopStack(stack);
	  if(tempToken->type != LParenthesis){
	    AddToken(postExpression, tempToken);
	  }
	  else{
	    PushStack(stack, (void*) tempToken);
	    break;
	  }
	}
	PushStack(stack, (void*) token);
	tempToken = NULL;
	token = NULL;
	break;
      case '*':
      case '/':
      case '%':
	token->type = Operator;
	token->valType = Integer;
	token->value.iVal = firstCh;
	while(!EmptyStack(stack)){
	  tempToken = (Token*) PopStack(stack);
	  if(tempToken->type != LParenthesis && tempToken->value.iVal != '+' &&
	     tempToken->value.iVal != '-'){
	    AddToken(postExpression, tempToken);
	  }
	  else{
	    PushStack(stack, (void*) tempToken);
	    break;
	  }
	}
	PushStack(stack, (void*) token);
	tempToken = NULL;
	token = NULL;
	break;
      default:
	fprintf(stderr, "Unknown operator %s\n", tokString);
	DestroyTokenList(postExpression);
	free(token);
	DestroyStack(stack);
	return NULL;
      }
    }
  }

  while(!EmptyStack(stack)){
    token = (Token*) PopStack(stack);
    AddToken(postExpression, token);
  }

  DestroyStack(stack);
  return postExpression;
}


//Perform the operation specified by operator on the 2 operands
//@param operator the token with the operation to perform
//@param operand1 the token with the first operand value
//@param operand2 the token with the second operand value
static void performOperation(Token* operator,
			     Token* operand1,
			     Token* operand2){
  //perform type conversions if necessary
  if(operand1->valType != operand2->valType){
    operator->valType = Float;
    if(operand2->valType == Integer){
      operand2->valType = Float;
      operand2->value.fVal = (float) operand2->value.iVal;
    }
    else{
      operand1->valType = Float;
      operand1->value.fVal = (float) operand1->value.iVal;
    }
  }
  else{
    operator->valType = operand1->valType;
  }

  int isFloat = (operand1->valType == Float);

  switch(operator->value.iVal){
  case '+':
    if(isFloat){
      operator->value.fVal =
	operand1->value.fVal + operand2->value.fVal;
    }
    else{
      operator->value.iVal =
	operand1->value.iVal + operand2->value.iVal;
    }
    break;
  case '-':
    if(isFloat){
      operator->value.fVal =
	operand1->value.fVal - operand2->value.fVal;
    }
    else{
      operator->value.iVal =
	operand1->value.iVal - operand2->value.iVal;
    }
    break;
  case '*':
    if(isFloat){
      operator->value.fVal =
	operand1->value.fVal * operand2->value.fVal;
    }
    else{
      operator->value.iVal =
	operand1->value.iVal * operand2->value.iVal;
    }
    break;
  case '/':
    if(isFloat){
      operator->value.fVal =
	operand1->value.fVal / operand2->value.fVal;
    }
    else{
      operator->value.iVal =
	operand1->value.iVal / operand2->value.iVal;
    }
    break;
  case '%':
    if(isFloat){
      if(verifyModulo(operand1, operand2)){
	operator->value.iVal =
	  (int) operand1->value.fVal %
	  (int) operand2->value.fVal;
	operator->valType = Integer;
      }
      else{
	operator->valType = Unknown;
	fprintf(stderr, "Error: modulo operator used on float operands %f and %f\n",
		operand1->value.fVal,
		operand2->value.fVal);
	return;
      }
    }
    else{
      operator->value.iVal =
	operand1->value.iVal % operand2->value.iVal;
    }
    break;
  default:
    break;
  }

  operator->type = Operand;

  return;
}


//Evaluate an arithmetic expression and return the result as a token
Token* evaluateExpression(SymbolTable* table, char* expression){
  
  TokenList* postExpression = convertToPostfix(table, expression);

  //error converting to postfix; return
  if(!postExpression){
    return NULL;
  }
  
  Token* token;
  Token* operand1;
  Token* operand2;
  Stack* stack = CreateStack();

  size_t i;

  for(i = 0; i < postExpression->size; i++){
    token = postExpression->list[i];
    if(token->type == Operand){
      PushStack(stack, (void*) token);
    }
    else{
      operand2 = (Token*) PopStack(stack);
      operand1 = (Token*) PopStack(stack);
      //perform operation and store value in operator token
      performOperation(token, operand1, operand2);

      //error in operation, free all memory and return NULL to indicate error
      if(token->valType == Unknown){
	DestroyTokenList(postExpression);
	DestroyStack(stack);
	return NULL;
      }
	
      //operator token now has new value; push it onto the stack
      PushStack(stack, (void*) token);
      }
  }

  
  token = (Token*) PopStack(stack);

  Token* returnToken = malloc(sizeof(Token));
  returnToken->valType = token->valType;
  returnToken->value = token->value;

  DestroyTokenList(postExpression);
  DestroyStack(stack);
  return returnToken;
}
