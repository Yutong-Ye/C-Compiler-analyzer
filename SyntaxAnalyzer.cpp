//These headers enable reading input files, printing output, storing tokens and expressions, and evaluating postfix notation using a stack.
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
//These headers support parsing by enabling character checks, string handling, formatted output, and mapping token types to readable names for lexical analysis.
#include <map>
#include <cctype>
#include <string>
#include <cstdio>

using namespace std;

//This enum defines all possible token types used by the lexical analyzer to classify parts of the input, like identifiers, operators, and parentheses.
enum TokenType {
    IDENT, INT_CONST, ASSIGN_OP, ADD_OP, SUB_OP, MULT_OP, DIV_OP,
    UNARY_MINUS, BNOT_OP, REQ_OP, RNEQ_OP, RLT_OP, RLEQ_OP, RGT_OP, RGEQ_OP,
    BAND_OP, BOR_OP, LEFT_PAREN, RIGHT_PAREN, END_OF_FILE
};

//This struct Token stores the type and actual text lexeme of each token found during lexical analysis, allowing the parser to understand and process input code.
struct Token {
    TokenType type;
    string lexeme;

    Token() {}
    Token(TokenType t, const string& l) : type(t), lexeme(l) {}
};

//These variables store the list of tokens from the input, track the current parsing position, number syntax rules for debugging, and build the postfix expression for intermediate code generation.
vector<Token> tokens;
int currentIndex = 0;
int ruleId = 1;
vector<string> postfix;

//This function prints the token’s type and its actual text to see exactly what the parser is processing.
void printToken(const Token& tok) {
    printf("Token Num Code =\t%d\tLexeme =\t%s\n", tok.type + 10, tok.lexeme.c_str());
}

//These functions print messages when entering and exiting grammar rules, helping you trace the parser step by step during syntax analysis.
void enterRule(const string& name) {
    cout << "enter<" << name << ">(" << ruleId++ << ")" << endl;
}

void exitRule(const string& name) {
    cout << "exit<" << name << ">(" << (ruleId - 1) << ")" << endl;
}

//This function returns the current token and then shifts the position to the following token in the input stream.
Token nextToken() {
    if (currentIndex >= (int)tokens.size()) {
        return Token(END_OF_FILE, "EOF");
    }
    return tokens[currentIndex];
}

//This function retrieves the current token and moves the pointer forward to prepare for reading the next token.
Token lookahead() {
    if (currentIndex >= (int)tokens.size()) {
        return Token(END_OF_FILE, "EOF");
    }
    return tokens[currentIndex++];
}

//This function checks if the current token matches the expected type, prints it if correct, moves to the next token, or reports a syntax error if it doesn't match.
void match(TokenType expected) {
    Token t = nextToken();
    if (t.type == expected) {
        printToken(t);
        lookahead();
    } else {
        cerr << "Syntax error at assignment " << ruleId << ": Expected ";
        cerr << expected << " but got " << t.type << " (" << t.lexeme << ")" << endl;
        exit(1);
    }
}

//These functions parse and evaluate arithmetic expressions, factor() handles variables and constants, term() manages multiplication and division, and expr() processes addition and subtraction.
void factor();
void term();
void expr();
void rel_expr();
void eq_expr();
void bool_and_expr();
void bool_or_expr();

//The factor() function handles the smallest units in an expression, such as identifiers or constants, and prints the token if it's valid.
void factor() {
    enterRule("factor");
    Token t = nextToken();
    if (t.type == IDENT || t.type == INT_CONST) {
        printToken(t);
        postfix.push_back(t.lexeme);
        lookahead();
    //This line handles expressions wrapped in parentheses by recursively parsing the inner expression using bool_or_expr() and ensuring it is properly closed with a matching right parenthesis.
    } else if (t.type == LEFT_PAREN) {
        printToken(t);
        lookahead();
        bool_or_expr();
        match(RIGHT_PAREN);
    //Handles a unary minus (-) or logical NOT (!) by consuming the operator, recursively parsing the following factor, and appending the operator to the postfix expression.
    } else if (t.type == SUB_OP || t.type == BNOT_OP) { 
        printToken(t);
        lookahead();
        factor();
        postfix.push_back(t.lexeme);
    // Reports a syntax error and terminates the program if the token does not match any valid <factor> 
    } else {
        cerr << "Syntax error in factor: Unexpected token " << t.lexeme << endl;
        exit(1);
    }
    exitRule("factor");
}

//The term() function parses a term, starting with a factor, and repeatedly handles multiplication or division operations if they follow.
void term() {
    enterRule("term");
    factor();
    while (nextToken().type == MULT_OP || nextToken().type == DIV_OP) {
        Token op = nextToken();
    //This block processes a multiplication or division operator by printing it, consuming it, parsing the next factor, and appending the operator to the postfix expression.
        printToken(op);
        lookahead();
        factor();
        postfix.push_back(op.lexeme);
    }
    exitRule("term");
}

//The expr() function begins parsing an expression by handling terms and continues processing as long as the next token is an addition or subtraction operator.
void expr() {
    enterRule("expr");
    term();
    while (nextToken().type == ADD_OP || nextToken().type == SUB_OP) {
        Token op = nextToken();
    //This block handles addition or subtraction by printing and consuming the operator, parsing the next term, and appending the operator to the postfix expression.
        printToken(op);
        lookahead();
        term();
        postfix.push_back(op.lexeme);
    }
    exitRule("expr");
}

//The rel_expr() function enters the relational expression rule, parses an expression, and processes relational operators if present.
void rel_expr() {
    enterRule("rel_expr");
    expr();
    if (nextToken().type >= RLT_OP && nextToken().type <= RGEQ_OP) {
        Token op = nextToken();
    //This block processes a relational operator by printing and consuming it, parsing the right-hand expression, and appending the operator to the postfix expression.
        printToken(op);
        lookahead();
        expr();
        postfix.push_back(op.lexeme);
    }
    exitRule("rel_expr");
}

//The eq_expr() function starts the equality expression rule and checks for == or != comparisons.
void eq_expr() {
    enterRule("equal_expr");
    rel_expr();
    if (nextToken().type == REQ_OP || nextToken().type == RNEQ_OP) {
        Token op = nextToken();
    //This code handles an equality operator (== or !=) by printing and consuming it, parsing the right-hand relational expression, and appending the operator to the postfix expression.
        printToken(op);
        lookahead();
        rel_expr();
        postfix.push_back(op.lexeme);
    }
    exitRule("equal_expr");
}

//The bool_and_expr() function handles logical AND (&&) expressions.
void bool_and_expr() {
    enterRule("bool_and_expr");
    eq_expr();
    while (nextToken().type == BAND_OP) {
        Token op = nextToken();
    //This code processes the logical AND (&&) operator by printing and consuming it, parsing the next equality expression, and adding the operator to the postfix expression.
        printToken(op);
        lookahead();
        eq_expr();
        postfix.push_back(op.lexeme);
    }
    exitRule("bool_and_expr");
}

//The bool_or_expr() function handles logical OR (||) expressions.
void bool_or_expr() {
    enterRule("bool_or_expr");
    bool_and_expr();
    while (nextToken().type == BOR_OP) {
        Token op = nextToken();
    //This code handles the logical OR (||) operator by printing and consuming it, parsing the next boolean AND expression, and appending the operator to the postfix expression.
        printToken(op);
        lookahead();
        bool_and_expr();
        postfix.push_back(op.lexeme);
    }
    exitRule("bool_or_expr");
}

//The assign() function parses an assignment statement and handles the full boolean expression on the right-hand side.
void assign() {
    enterRule("assign");
    Token id = nextToken();
    if (id.type != IDENT) {
        cerr << "Syntax error: Expected identifier at start of assignment" << endl;
        exit(1);
    }
    //This code handles an assignment statement by printing and consuming the identifier, matching the assignment operator (=), parsing the right-hand expression, and appending the identifier and = symbol to the postfix expression to reflect the assignment.
    printToken(id);
    lookahead();
    match(ASSIGN_OP);
    bool_or_expr();
    postfix.push_back(id.lexeme);
    postfix.push_back("=");
    exitRule("assign");
}

//The generateIC() function generates intermediate code from the postfix expression.
void generateIC(const vector<string>& postfix) {
    cout << "\n-----------------------INTERMEDIATE CODE GENERATOR-----------------------" << endl;
    stack<string> s;
    char tempName = 'A';

//This block initializes a map that associates arithmetic operator symbols with their corresponding intermediate code operation names for use during code generation.
    map<string, string> opMap;
    opMap["+"] = "add";
    opMap["-"] = "sub";
    opMap["*"] = "mul";
    opMap["/"] = "div";
//This block maps logical and equality operators to their corresponding intermediate code instruction names used in the Intermediate Code Generator.
    opMap["&&"] = "band";
    opMap["||"] = "bor";
    opMap["!"] = "bnot";
    opMap["=="] = "req";
    opMap["!="] = "rneq";
//This block maps relational operators to their intermediate code instruction names used for generating intermediate representation.
    opMap["<"] = "rlt";
    opMap["<="] = "rleq";
    opMap[">"] = "rgt";
    opMap[">="] = "rgeq";
//This loop goes through each item in the postfix expression, and when it sees an assignment (=), it takes two values from the stack—one for the right-hand side and one for the left-hand side—to create an assignment instruction.
    for (size_t i = 0; i < postfix.size(); ++i) {
        string token = postfix[i];
        if (token == "=") {
            string rhs = s.top(); s.pop();
            string lhs = s.top(); s.pop();
            //This prints the top two values being removed from the stack and shows the assignment instruction being created from them.
            cout << "top " << rhs << endl << "pop()\n";
            cout << "top " << lhs << endl << "pop()\n";
            cout << "assign " << lhs << ", " << rhs << endl;
        //This checks if the token is an operator, and if it's a logical NOT (!), it pops one value from the stack, creates a temporary variable, and prepares to generate intermediate code for the NOT operation.
        } else if (opMap.count(token)) {
            if (token == "!") {
                string a = s.top(); s.pop();
                string temp = string(1, tempName++);
            //This prints the top value being popped from the stack, generates the intermediate code for the logical NOT (bnot) operation, and then pushes the result as a temporary variable back onto the stack
                cout << "top " << a << "\npop()\n";
                cout << "bnot, " << a << ", " << temp << endl;
                cout << "push " << temp << endl;
            //This checks if the minus sign is used as a unary operator, then pops that item, creates a temporary variable for the result, and prepares to generate the intermediate code for unary minus.
                s.push(temp);
            } else if (token == "-" && s.size() == 1) {
                string a = s.top(); s.pop();
                string temp = string(1, tempName++);
            //This prints the steps for applying unary minus to a value: showing the value being popped, generating the uminus operation, pushing the result to the stack, and storing it in a temporary variable.
                cout << "top " << a << "\npop()\n";
                cout << "uminus, " << a << ", " << temp << endl;
                cout << "push " << temp << endl;
                s.push(temp);
            //This handles binary operators by popping two values from the stack, creating a new temporary variable to store the result, and preparing for intermediate code generation.
            } else {
                string b = s.top(); s.pop();
                string a = s.top(); s.pop();
                string temp = string(1, tempName++);
            //This prints the operation steps: shows which operands are popped from the stack, prints the intermediate code instruction using the mapped operator name, pushes the result temp variable back onto the stack for future use.
                cout << token << "\ntop " << b << "\npop()\ntop " << a << "\npop()\n";
                cout << opMap[token] << ", " << a << ", " << b << ", " << temp << endl;
                cout << "push " << temp << endl;
                s.push(temp);
            }
            //This handles operands by printing a push instruction and placing them onto the stack for later operations.
        } else {
            cout << "push " << token << endl;
            s.push(token);
        }
    }
}

//This function breaks the input line into tokens by skipping spaces and preparing to identify identifiers, numbers, or symbols.
vector<Token> tokenize(const string& line) {
    vector<Token> result;
    size_t i = 0;
    while (i < line.length()) {
        if (isspace(line[i])) { i++; continue; }
    //This block identifies variable names made of letters, digits, or underscores by collecting characters into a string until the pattern ends.
        if (isalpha(line[i]) || line[i] == '_') {
            string id;
            while (i < line.length() && (isalnum(line[i]) || line[i] == '_')) {
                id += line[i++];
            }
        //This part checks if the input is a number, gathers the full numeric constant, and adds it to the list of tokens.
            result.push_back(Token(IDENT, id));
        } else if (isdigit(line[i])) {
            string num;
            while (i < line.length() && isdigit(line[i])) {
                num += line[i++];
            }
        //This section handles multi-character operators by checking the next two characters and identifying them as a single token if matched.
            result.push_back(Token(INT_CONST, num));
        } else {
            string twoChar = line.substr(i, 2);
            if (twoChar == "==" || twoChar == "!=" || twoChar == "<=" || 
                twoChar == ">=" || twoChar == "&&" || twoChar == "||") {
            //This block adds the correct token type to the result list based on which two-character operator is found.
                if (twoChar == "==") result.push_back(Token(REQ_OP, twoChar));
                else if (twoChar == "!=") result.push_back(Token(RNEQ_OP, twoChar));
                else if (twoChar == "<=") result.push_back(Token(RLEQ_OP, twoChar));
                else if (twoChar == ">=") result.push_back(Token(RGEQ_OP, twoChar));
                else if (twoChar == "&&") result.push_back(Token(BAND_OP, twoChar));
            //This part handles the remaining two-character case || as a binary OR operator, then checks for single-character operators like =, converting them into appropriate tokens.
                else result.push_back(Token(BOR_OP, twoChar));
                i += 2;
            } else {
                char c = line[i++];
                if (c == '=') result.push_back(Token(ASSIGN_OP, "="));
            //This section checks for single-character arithmetic and relational operators, converting each one into the correct token type for the analyzer to process.
                else if (c == '+') result.push_back(Token(ADD_OP, "+"));
                else if (c == '-') result.push_back(Token(SUB_OP, "-"));
                else if (c == '*') result.push_back(Token(MULT_OP, "*"));
                else if (c == '/') result.push_back(Token(DIV_OP, "/"));
                else if (c == '<') result.push_back(Token(RLT_OP, "<"));
            //This block handles remaining single-character operators and parentheses, and reports a lexical error if the character doesn't match any valid token type.
                else if (c == '>') result.push_back(Token(RGT_OP, ">"));
                else if (c == '(') result.push_back(Token(LEFT_PAREN, "("));
                else if (c == ')') result.push_back(Token(RIGHT_PAREN, ")"));
                else {
                    cerr << "Lexical error: Invalid character '" << c << "'" << endl;
                    exit(1);
                }
            }
        }
    }
    //This code adds an end_of_file token to signal the end of the input and then returns the full list of tokens for parsing.
    result.push_back(Token(END_OF_FILE, "EOF"));
    return result;
}
//This code starts the main function, opens the input file front.in, and checks if it's accessible and if not, it prints an error and exits the program.
int main() {
    ifstream fin("front.in");
    if (!fin.is_open()) {
        cerr << "Error: Could not open front.in" << endl;
        return 1;
    }
//These lines initialize a string to hold each input line and a counter for assignment blocks, then loop through each line in the file, skipping any empty lines.
    string line;
    int assignmentNumber = 1;

    while (getline(fin, line)) {
        if (line.empty()) continue;
//These lines print the current line being processed, mark the start of a new assignment, tokenize the line into tokens, reset parsing state, and clear the postfix expression list to prepare for the next analysis.
        cout << "\nProcessing line: " << line << endl;
        cout << "\n-----------------------START ASSIGNMENT " << assignmentNumber << "-----------------------" << endl;

        tokens = tokenize(line);
        currentIndex = 0;
        ruleId = 1;
        postfix.clear();
//This prints a header to indicate the start of parsing, then begins parsing the assignment expression using the assign() function.
        cout << "-----------------------PARSING TREE-----------------------" << endl;
        assign();
//This prints a header, then outputs the entire postfix expression generated during parsing by iterating through the postfix vector.
        cout << "\n-----------------------POSTFIX EXPRESSION-----------------------\n";
        for (size_t i = 0; i < postfix.size(); ++i) {
            cout << postfix[i] << " ";
        }
        cout << endl;
//This calls the intermediate code generator to simulate execution of the postfix expression, prints an end marker for the assignment, then continues to the next input line and closes the file when done.
        generateIC(postfix);
        cout << "\n-----------------------END Assignment " << assignmentNumber++ << "-----------------------\n";
    }

    fin.close();
    return 0;
}