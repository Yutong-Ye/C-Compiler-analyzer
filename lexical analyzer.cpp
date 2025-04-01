
/* front.c - a lexical analyzer system for simple arithmetic expressions */

#include <stdio.h>
#include <ctype.h>
#include <string.h>


/*Global variables declarations*/

int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp;

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();
void expr();       // Add this
void term();       // Add this
void factor();     // Add this


/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define MOD_OP 27

/******************************************************/
/******************************************************/
/* main driver
   This is the entry point of the program. It returns an int, typically 0 for success or 1 for failure.
   Reads the first character from the input file by calling the getChar() function.
   Extracts the first meaningful token before parsing begins by calling the lex() function.
   Calls the expr() function to analyze the structure of the expression using recursive descent parsing,
   based on EBNF grammar rules and the token stream created by lex().
   Uses fclose(in_fp) and fclose(out_fp) to close the input and output files when done.
*/

int main() {
    if ((in_fp = fopen("ParseTree1.cpp", "r")) == NULL) {
        printf("ERROR - cannot open flie\n");
        return 1;
    }

    char outputFileName[] = "Ye_12012842.out";
    out_fp = fopen(outputFileName, "w");
    if (out_fp == NULL) {
        printf("ERROR - cannot open output file\n");
        fclose(in_fp);
        return 1;
    }

    getChar();   // initialize
    lex();       // fetch first token
    expr();      // parse the full expression

    fclose(in_fp);
    fclose(out_fp);
    return 0;
}

/*****************************************************/
/* expr - parses expressions according to EBNF rule:
   <expr> → <term> { (+ | -) <term> } */
void expr() {
    printf("Enter <expr>\n");
    term();
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        term();
    }
    printf("Exit <expr>\n");
}

/*****************************************************/
/* term - parses terms according to EBNF rule:
   <term> → <factor> { (* | / | %) <factor> } */
void term() {
    printf("Enter <term>\n");
    factor();
    while (nextToken == MULT_OP || nextToken == DIV_OP || nextToken == MOD_OP) {
        lex();
        factor();
    }
    printf("Exit <term>\n");
}

/*****************************************************/
/* factor - parses factors according to EBNF rule:
   <factor> → <ident> | <intconst> | (<expr>) */
void factor() {
    printf("Enter <factor>\n");
    if (nextToken == IDENT || nextToken == INT_LIT) {
        lex();
    } else if (nextToken == LEFT_PAREN) {
        lex();
        expr();
        if (nextToken == RIGHT_PAREN) {
            lex();
        } else {
            printf("Error - missing right parenthesis\n");
        }
    } else {
        printf("Error - invalid factor\n");
    }
    printf("Exit <factor>\n");
}

/*****************************************************/
/* lookup - extended to include modulus operator */
int lookup(char ch) {
    switch (ch) {
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        case '%':  // NEW: Modulus operator
            addChar();
            nextToken = MOD_OP;
            break;
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        default:
            addChar();
            nextToken = EOF;
            break;
    }
    return nextToken;
}

/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = '\0';
    } else {
        printf("Error - lexeme is too long\n");
    }
}

/*****************************************************/
/* getChar - a function to get the next character of input and determine its character class */
void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar)) {
            charClass = LETTER;
        } else if (isdigit(nextChar)) {
            charClass = DIGIT;
        } else {
            charClass = UNKNOWN;
        }
    } else {
        charClass = EOF;
    }
}

/*****************************************************/
/* getNonBlank - a function to call getChar until it returns a non-whitespace character */
void getNonBlank() {
    while (isspace(nextChar)) {
        getChar();
    }
}

/*****************************************************/
/* lex - an enhanced lexical analyzer for arithmetic expressions */
int lex() {
    lexLen = 0;          // Reset lexeme length for new token
    getNonBlank();       // Skip whitespace

    switch (charClass) {
        /* Parse identifiers */
        case LETTER:
            addChar();   // Add first alphabetic character
            getChar();   // Get next character
            // Continue while we have letters or digits
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }
            nextToken = IDENT;
            break;
        
        /* Parse integer literals */
        case DIGIT:
            addChar();   // Add first digit
            getChar();   // Get next character
            // Continue while we have digits
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
            nextToken = INT_LIT;
            break;
        
        /* Handle operators, parentheses, and unknown characters */
        case UNKNOWN:
            // Handle known operators/symbols
            if (nextChar == '+' || nextChar == '-' || nextChar == '*' || 
                nextChar == '/' || nextChar == '%' || 
                nextChar == '(' || nextChar == ')') {
                lookup(nextChar);  // Sets nextToken
                getChar();         // Move to next character
            } 
            // Handle unrecognized characters
            else {
                printf("Error: Unrecognized character '%c'\n", nextChar);
                addChar();
                nextToken = UNKNOWN;
                getChar();
            }
            break;
        
        /* Handle end of file */
        case EOF:
            nextToken = EOF;
            strcpy(lexeme, "EOF");  // More reliable than manual assignment
            break;
    }

    // Print token information (only if not EOF)
    if (nextToken != EOF) {
        printf("Token Code: %-3d\tLexeme: %s\n", nextToken, lexeme);
    } else {
        printf("End of file reached\n");
    }
    
    return nextToken;
}
