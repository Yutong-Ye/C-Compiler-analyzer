# C++ Lexical Analyzer

This is a simple lexical analyzer written in C++ that scans a source code file written in the C programming language and identifies various tokens such as:

- Identifiers
- Keywords
- Constants (integers and floating-point numbers)
- Operators
- Separators
- Array identifiers
- Special symbols

## Features

- Supports recognition of both standard identifiers and array identifiers (e.g., `arr[5]`)
- Detects floating-point constants alongside integer constants
- Identifies and classifies C operators, separators, and keywords
- Outputs a tokenized list of detected elements with corresponding token codes

## Token Codes

| Token Type           | Code |
|----------------------|------|
| Identifier           | 100  |
| Keyword              | 101  |
| Integer Constant     | 102  |
| Floating Constant    | 103  |
| Operator             | 104  |
| Separator            | 105  |
| Array Identifier     | 106  |
/* 
Operator token codes 
*/
//These define token codes for basic math and assignment operators so the program can recognize and label them during analysis.
#define ASSIGN_OP         220  // =
#define ADD_OP            221  // +
#define SUB_OP            222  // -
#define MULT_OP           223  // *
#define DIV_OP            224  // /
//These define token codes for less common operators like power, unary minus, and modulo.
#define POW_OP            227  // ^
#define UNARY_MINUS       328  // ~
#define MOD_OP            329  // %
//These identify increment and decrement operators, used often in loops and counters.
#define INC_OP            330  // ++
#define DEC_OP            331  // --
//These represent compound assignment operators that combine math and assignment in one step.
#define ADD_ASSIGN_OP     341  // +=
#define SUB_ASSIGN_OP     342  // -=
#define MULT_ASSIGN_OP    343  // *=
#define DIV_ASSIGN_OP     344  // /=
//These handle special compound operators like power-assign and modulo-assign.
#define POW_ASSIGN_OP     347  // ^=
#define MOD_ASSIGN_OP     349  // %=

//This line defines a special code −1 to represent the end of the input file, so the lexical analyzer knows when to stop reading characters.
#define EOF_TOKEN         -1   

enum TokenType is used in the lexical analyzer to tag input parts like IDENT or INT_CONST, so the syntax analyzer can correctly interpret them.

The Token struct stores each token’s type and text, while vector<Token> tokens holds all tokens in order for parsing.

The parser uses global variables:
tokens for the token stream,
currentIndex to track position,
ruleId for grammar rule tracing,
and postfix to build postfix expressions for code generation.

enterRule(), exitRule(), and printToken() help trace the parser’s step-by-step flow and show exactly what’s being processed.

nextToken() peeks at the current token, while lookahead() consumes it by advancing currentIndex.

Expression parsing follows operator precedence:
factor() handles basic elements (identifiers, numbers, parentheses),
term() handles * and /,
expr() handles + and -,
and the rest (rel_expr, eq_expr, bool_and_expr, bool_or_expr) handle comparison and logical operators.

assign() parses assignment statements like x = a + b * c, validates them, and converts them into postfix form for code generation.

generateIC() turns the postfix into intermediate code using a stack and prints instructions like add, assign, bnot, etc., simulating how machines evaluate expressions.

tokenize() is part of the lexical analyzer—it splits input into tokens by skipping spaces, grouping identifiers and numbers, recognizing multi- and single-character operators, and reporting any invalid characters.

The main() function opens the file front.in, reads each line, tokenizes it, resets the parser state, parses the assignment, prints the postfix expression, generates intermediate code, and moves to the next line.

## How to Compile

To compile the program using `g++`, run the following command in your terminal:

```bash
g++ "lexical analyzer.cpp" -o lexer


