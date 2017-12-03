///file:evaluate.h
///description:interface for evaluating arithmetic and boolean expressions
///author: avv8047 : Azhur Viano


#ifndef EVALUATE_H
#define EVALUATE_H

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "stack.h"
#include "symbolTable.h"

#define INITIAL_SIZE 20


//Types for a token, used for converting to postfix
typedef enum token_type {Operator, Operand, LParenthesis,
RParenthesis} TokenType;


//Token for an operand, operator, or parantheses
typedef struct Token_ {
  //type of the toke, operand, operator, or parenthesis
  TokenType type;
  //type of value, Float or Integer
  Type valType;
  //value of the token
  Value value;
} Token;


//Check whether a string is a float
//@param str a null terminated string
//@returns 1 if the string is a float, 0 otherwise
int isFloat(char* str);


//Evaluate an infix expression
//@param table the symbol table to use
//@returns a token struct  containing an int or float,
//  or NULL if the evaluation failed
Token* evaluateExpression(SymbolTable* table, char* expression);


#endif 
