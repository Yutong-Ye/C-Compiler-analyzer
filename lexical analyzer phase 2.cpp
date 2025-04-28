// Yutong Ye 12012842

/* 
use #include <stdio.h> to handle input/output, 
#include <ctype.h> to check character types like letters or digits, 
and #include <string.h> to work with strings like copying or comparing them.
*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* 
Global variable declarations 
*/
//These store the current character, its type, and build the token string.
int charClass;
char lexeme[100];
char nextChar;
//These track the token length and store current token codes.
int lexLen;
int token;
int nextToken;
//These are file pointers for reading input and writing output.
FILE *in_fp;

/* 
Global Function declarations 
*/

/* 
These three functions help prepare characters so the lexical analyzer can correctly build tokens.
getChar() reads and classifies characters, getNonBlank() skips spaces, 
and addChar() collects valid characters into the lexeme string.
*/
void addChar();
void getChar();
void getNonBlank();

/*
Declare int lex(); because it’s the main function that reads characters from the input 
and builds valid tokens like identifiers, numbers, or operators, returning a token type so the parser knows what to do next.
*/
int lex();


/* Character classes 
Define these to categorize each character as a letter, digit, 
or unknown so the lexical analyzer knows how to process it when building tokens.
*/
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Define these token codes to help the lexical analyzer recognize and label different types of words or numbers in the code, 
like variables, constants, arrays, decimals, and keywords.
*/
#define INT_LIT           10    
#define IDENT             11   
#define ARRAY_ID          12   
#define FLOAT_LIT         14   
#define KEYWORD           15   

/* 
Separator token codes 
*/

// These define tokens for parentheses, used to group expressions.
#define LEFT_PAREN        25   // (
#define RIGHT_PAREN       26   // )
// These define tokens for curly braces, used in blocks of code.
#define LEFT_BRACE        125  // {
#define RIGHT_BRACE       126  // }
//These define tokens for comma and semicolon, used to separate values and end statements.
#define COMMA             127  // ,
#define SEMICOLON         128  // ;


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

/******************************************************/
/* main driver
   This is the entry point of the program. 
*/

//This opens the input file for reading; if it fails, it prints an error and stops the program.
int main() {
    if ((in_fp = fopen("front.in", "r")) == NULL) {
        printf("ERROR - cannot open flie\n");
        return 1;
    }


/*Start by reading the first character. 
Then, keep calling the lexical analyzer to analyze and print each token until we hit the end of the input file.
*/
    getChar();
    do {
        lex();
    } while (nextToken != EOF_TOKEN);   

//Close both files and return 0 to show the program ran successfully.
    fclose(in_fp);
    return 0;
}

/*****************************************************/
/* 
Helper function: isEqual
This function compares two strings character by character to determine if they are exactly the same.
*/

//Start by setting up an index i to compare characters in both strings one by one.
int isEqual(const char *s1, const char *s2) {
    int i = 0;
//Loop through both strings and return false (0) if any character doesn’t match.
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return 0;  
        }
        i++;
    }
//After looping, return true 1 only if both are the same.
    return s1[i] == '\0' && s2[i] == '\0';  
}
/*
checkKeyword() ensures accurate token classification by checking if a lexeme is a keyword.
*/

//These lines check if the word is a type related keyword like int, short, or long.
int checkKeyword(const char* word) {
    if (isEqual(word, "int")) return KEYWORD;
    else if (isEqual(word, "short")) return KEYWORD;
    else if (isEqual(word, "long")) return KEYWORD;
//These check for numeric type keywords that tell how numbers should be stored.
    else if (isEqual(word, "signed")) return KEYWORD;
    else if (isEqual(word, "unsigned")) return KEYWORD;
    else if (isEqual(word, "float")) return KEYWORD;
//These lines look for more reserved words used in C like double, const, and do.
    else if (isEqual(word, "double")) return KEYWORD;
    else if (isEqual(word, "const")) return KEYWORD;
    else if (isEqual(word, "do")) return KEYWORD;
//These lines check if the word is a control flow keyword. 
    else if (isEqual(word, "else")) return KEYWORD;
    else if (isEqual(word, "for")) return KEYWORD;
    else if (isEqual(word, "if")) return KEYWORD;
//These check for switch and while, which are also control flow keywords.
    else if (isEqual(word, "switch")) return KEYWORD;
    else if (isEqual(word, "while")) return KEYWORD;
    else return IDENT;
}


/*****************************************************/
/* 
Lexical analyzer functions 
*/

// Starts a switch to match symbols like +, -, etc. to their token codes.
int lookup(char ch) {
    switch (ch) {
//Handles all versions of +: regular +, ++, and += so we know what kind of operation it is.
        case '+':
            addChar(); getChar();
            if (nextChar == '+') { addChar(); nextToken = INC_OP; getChar(); }
            else if (nextChar == '=') { addChar(); nextToken = ADD_ASSIGN_OP; getChar(); }
            else nextToken = ADD_OP;
            break;
//Same idea for -: it checks for --, -=, or just -.
        case '-':
            addChar(); getChar();
            if (nextChar == '-') { addChar(); nextToken = DEC_OP; getChar(); }
            else if (nextChar == '=') { addChar(); nextToken = SUB_ASSIGN_OP; getChar(); }
            else nextToken = SUB_OP;
            break;
//Handles multiplication * and multiply-assign *=.
        case '*':
            addChar(); getChar();
            if (nextChar == '=') { addChar(); nextToken = MULT_ASSIGN_OP; getChar(); }
            else nextToken = MULT_OP;
            break;
//Handles / and /=, so we know if it's division or assign divide.
        case '/':
            addChar(); getChar();
            if (nextChar == '=') { addChar(); nextToken = DIV_ASSIGN_OP; getChar(); }
            else nextToken = DIV_OP;
            break;
//Handles modulo % and %= so we can tokenize them properly.
        case '%':
            addChar(); getChar();
            if (nextChar == '=') { addChar(); nextToken = MOD_ASSIGN_OP; getChar(); }
            else nextToken = MOD_OP;
            break;

//Handles power ^ and power-assign ^=.
        case '^':
            addChar(); getChar();
            if (nextChar == '=') { addChar(); nextToken = POW_ASSIGN_OP; getChar(); }
            else nextToken = POW_OP;
//Handles the ~ unary minus operator and sets the right token.
            break;
        case '~':
            addChar(); getChar();
            nextToken = UNARY_MINUS;
            break;
//Handles single equal sign =, used for assignments.
        case '=':
            addChar(); getChar();
            nextToken = ASSIGN_OP;
            break;
//Marks ( as a left parenthesis separator.
        case '(':
            addChar(); getChar();
            nextToken = LEFT_PAREN;
            break;
//Marks ) as a right parenthesis separator.
        case ')':
            addChar(); getChar();
            nextToken = RIGHT_PAREN;
            break;
//Handles { for code block opening.
        case '{':
            addChar(); getChar();
            nextToken = LEFT_BRACE;
            break;
//Handles } for code block closing.
        case '}':
            addChar(); getChar();
            nextToken = RIGHT_BRACE;
            break;
//Handles comma ,, often used in argument lists or variable declarations.
        case ',':
            addChar(); getChar();
            nextToken = COMMA;
            break;
//Handles semicolon ;, used to end statements.
        case ';':
            addChar(); getChar();
            nextToken = SEMICOLON;
            break;
//Catches anything not defined above and marks it as UNKNOWN.
        default:
            addChar(); getChar();
            nextToken = UNKNOWN;
            break;
    }
//Returns the final token code so it can be used by lex() and printed/output.
    return nextToken;
}

/*
We need addChar() to safely add each valid character to the current token lexeme and make sure it forms a proper string for processing.
*/
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = '\0';
    } else {
        printf("Error - lexeme is too long\n");
    }
}

//The getChar() function is responsible for reading the next character from the input file and classifying it into one of the defined character classes
void getChar() {
//Reads the next character from the file and checks if it's not the end of the file.
    if ((nextChar = getc(in_fp)) != EOF) {
// Check if the character is a letter (A-Z or a-z)
        if ((nextChar >= 'a' && nextChar <= 'z') || 
            (nextChar >= 'A' && nextChar <= 'Z')) {
            charClass = LETTER;
// If not a letter, check if it's a digit (0-9)
        } else if (nextChar >= '0' && nextChar <= '9') {
            charClass = DIGIT;
// If it's neither a letter nor a digit, treat it as unknown 
        } else {
            charClass = UNKNOWN;
        }
    } else {
// If nothing left to read, mark it as end of file
        nextChar = EOF; 
        charClass = EOF;
    }
}

//This function getNonBlank() to skip over spaces, tabs, and newlines so the lexical analyzer only looks at real code, not empty space.
void getNonBlank() {
    while (isspace(nextChar)) {
        getChar();
    }
}

/*
This starts the lex() function to reset the lexeme length and skip any whitespace before starting to build the next token.
*/
int lex() {
    lexLen = 0;
    getNonBlank();

/*
This part checks if we reached the end of the file.
If so, we stop analyzing and mark it with a special EOF token so the parser knows there’s no more input to read.
*/

    if (charClass == EOF) {
        nextToken = EOF_TOKEN;
        strcpy(lexeme, "EOF");
        printf("Token Num Code =\t%d\tLexeme =\t%s\n", nextToken, lexeme);
        return nextToken;
    }


    // This starts the case where the current character is a letter, so we assume it could be an identifier or keyword and begin building the word.
    switch (charClass) {
        case LETTER:
            addChar();
            getChar();
    //This loop keeps adding characters to the word as long as they are letters or digits, forming the full identifier or keyword.
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }
    //After the word is built, this line checks if it's a keyword or just a regular variable name and assigns the right token code.
            nextToken = checkKeyword(lexeme);
            break;

    //Starts building a number by adding the first digit and continues adding digits if more are found.
        case DIGIT:
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
    // Checks if there’s a decimal point after the digits to see if it’s a floating point number.
            if (nextChar == '.') {
                addChar();
                getChar();
    //If there is a digit after the dot, continue collecting the rest of the floating point number.
                if (charClass == DIGIT) {
                    while (charClass == DIGIT) {
                        addChar();
                        getChar();
                    }
    //Sets the token type as a float if valid, or UNKNOWN if the dot isn't followed by a digit.
                    nextToken = FLOAT_LIT;
                } else {
                    nextToken = UNKNOWN;
                }
            } else {
    //If no dot is found, it's just an integer. If the character is not a digit or letter, handle it as UNKNOWN.
                nextToken = INT_LIT;
            }
            break;

        case UNKNOWN:
    //Looks up operators and punctuation using lookup() when the character type is unknown.
            nextToken = lookup(nextChar);
            break;
    }

    //To check if an identifier is immediately followed by a valid array index in square brackets
        if (nextToken == IDENT && nextChar == '[') {
            addChar(); getChar(); 
            if (charClass == DIGIT) {
                while (charClass == DIGIT) {
                    addChar(); getChar();
                }
                if (nextChar == ']') {
                    addChar(); getChar(); 
                    nextToken = ARRAY_ID;
                }
            }
        }


    // Print token type and lexeme
    printf("Token Num Code =\t%d\tLexeme =\t%s\n", nextToken, lexeme);

    // Print for array identifiers
    if (nextToken == ARRAY_ID) {
        printf(">> Found Array Identifier: %s\n", lexeme);
    }

    //Return nextToken at the end of the lex() function to send the identified token code back to the parser
    return nextToken;
}