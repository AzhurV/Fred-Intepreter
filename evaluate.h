#ifndef EVALUATE_H
#define EVALUATE_H

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "stack.h"
#include "symbolTable.h"

#define INITIAL_SIZE 20

typedef enum token_type {Operator, Operand, LParenthesis,
RParenthesis} TokenType;


typedef struct Token_ {
  TokenType type;
  Type valType;
  Value value;
} Token;



#endif 
