///file:evaluate.c
///description:functions for evaluating arithmetic and boolean expressions
///author: avv8047 : Azhur Viano


#include "evaluate.h"


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
//@param str a null terminated string
//@returns 1 if the string is a float, 0 otherwise
static int isFloat(char* str){
  int i;

  for(i = 0; str[i]; i++){
    if(str[i] == '.'){
      return 1;
    }
  }
  return 0;
}


//Convert a string to a sequence of tokens in postfix notation
//@param table the symbol table to use
//@returns a sequence of tokens in postfix notation or NULL if any token is not recognized
static TokenList* convertToPostfix(SymbolTable* table){
  TokenList* postExpression = CreateTokenList();
  Stack* stack = CreateStack();

  char* tokString;
  const char* delim = " \t\n";
  char firstCh;

  //used to store tokens that will be appended to the output list
  Token* token = NULL;
  //used to store tokens popped from the stack
  Token* tempToken = NULL;

  //while there are still tokens remaining
  while((tokString == strtok(NULL, delim))){
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
    }
    //token is a symbol identifier
    else if(isalpha(firstCh)){
      Symbol* symbol = GetSymbol(table, tokString);
      //symbol does not exist in table
      if(!symbol){
	fprintf(stderr, "Error: symbol %s not found in table\n",
		tokString);
	DestroyTokenList(postExpression);
	return NULL;
      }
      token->valType = symbol->type;
      token->value = symbol->value;
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
	free(stack);
	return NULL;
      }
    }
  }

  free(stack);
  return postExpression;
}



//Evaluate an infix expression
//@param table the symbol table to use
//@returns a value struct containing an int or float, or NULL if the evaluation failed
Value evaluateExpression(SymbolTable* table){
  
  TokenList* postExpression = convertToPostfix(table);
  Token* token;
  Stack* stack = CreateStack();

  int i;

  for(i = 0; i < postExpression->size; i++){
    token = postExpression->list[i];
    if(token->type == Operand){
      PushStack(stack, (void*) token);
    }
    else{
      switch(token->value.iVal){
	
      }
  }
}
