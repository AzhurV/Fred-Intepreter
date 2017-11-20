#ifndef EVALUATE_H
#define EVALUATE_H

#include <string.h>

#include "stack.h"
#include "symbolTable.h"

#define INIT_SIZE 20

typedef enum token_type {Operator, Operand, LParenthesis,
RParenthesis} TokenType;


typedef struct Token_ {
  TokenType type;
  Value value;
} Token;



#endif 
