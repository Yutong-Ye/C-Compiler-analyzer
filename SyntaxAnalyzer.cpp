// C++ Lexical Analyzer + Syntax Analysis + Intermediate Code Generator 
// Project 2: Yutong Ye 12012842


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include <cctype>
#include <string>
#include <stdio.h>

using namespace std;

// ------------------------ Token Definitions --------------------------
enum TokenType {
    IDENT, INT_CONST, ASSIGN_OP, ADD_OP, SUB_OP, MULT_OP, DIV_OP,
    UNARY_MINUS, BNOT_OP, REQ_OP, RNEQ_OP, RLT_OP, RLEQ_OP, RGT_OP, RGEQ_OP,
    BAND_OP, BOR_OP, LEFT_PAREN, RIGHT_PAREN, END_OF_FILE
};

struct Token {
    TokenType type;
    string lexeme;

    Token() {}
    Token(TokenType t, const string& l) : type(t), lexeme(l) {}
};

vector<Token> tokens;
int currentIndex = 0;
int ruleId = 1;
vector<string> postfix;

void printToken(const Token& tok) {
    printf("Token Num Code =\t%d\tLexeme =\t%s\n", tok.type + 10, tok.lexeme.c_str());
}

void enterRule(const string& name) {
    cout << "enter<" << name << ">(" << ruleId++ << ")" << endl;
}

void exitRule(const string& name) {
    cout << "exit<" << name << ">(" << (ruleId - 1) << ")" << endl;
}

Token peek() {
    if (currentIndex >= (int)tokens.size()) {
        return Token(END_OF_FILE, "EOF");
    }
    return tokens[currentIndex];
}

Token advance() {
    if (currentIndex >= (int)tokens.size()) {
        return Token(END_OF_FILE, "EOF");
    }
    return tokens[currentIndex++];
}

void match(TokenType expected) {
    Token t = peek();
    if (t.type == expected) {
        printToken(t);
        advance();
    } else {
        cerr << "Syntax error at assignment " << ruleId << ": Expected ";
        cerr << expected << " but got " << t.type << " (" << t.lexeme << ")" << endl;
        exit(1);
    }
}

void factor();
void term();
void expr();
void rel_expr();
void eq_expr();
void bool_and_expr();
void bool_or_expr();

void factor() {
    enterRule("factor");
    Token t = peek();
    if (t.type == IDENT || t.type == INT_CONST) {
        printToken(t);
        postfix.push_back(t.lexeme);
        advance();
    } else if (t.type == LEFT_PAREN) {
        printToken(t);
        advance();
        bool_or_expr();
        match(RIGHT_PAREN);
    } else {
        cerr << "Syntax error in factor: Unexpected token " << t.lexeme << endl;
        exit(1);
    }
    exitRule("factor");
}

void term() {
    enterRule("term");
    factor();
    while (peek().type == MULT_OP || peek().type == DIV_OP) {
        Token op = peek();
        printToken(op);
        advance();
        factor();
        postfix.push_back(op.lexeme);
    }
    exitRule("term");
}

void expr() {
    enterRule("expr");
    term();
    while (peek().type == ADD_OP || peek().type == SUB_OP) {
        Token op = peek();
        printToken(op);
        advance();
        term();
        postfix.push_back(op.lexeme);
    }
    exitRule("expr");
}

void rel_expr() {
    enterRule("rel_expr");
    expr();
    if (peek().type >= RLT_OP && peek().type <= RGEQ_OP) {
        Token op = peek();
        printToken(op);
        advance();
        expr();
        postfix.push_back(op.lexeme);
    }
    exitRule("rel_expr");
}

void eq_expr() {
    enterRule("equal_expr");
    rel_expr();
    if (peek().type == REQ_OP || peek().type == RNEQ_OP) {
        Token op = peek();
        printToken(op);
        advance();
        rel_expr();
        postfix.push_back(op.lexeme);
    }
    exitRule("equal_expr");
}

void bool_and_expr() {
    enterRule("bool_and_expr");
    eq_expr();
    while (peek().type == BAND_OP) {
        Token op = peek();
        printToken(op);
        advance();
        eq_expr();
        postfix.push_back(op.lexeme);
    }
    exitRule("bool_and_expr");
}

void bool_or_expr() {
    enterRule("bool_or_expr");
    bool_and_expr();
    while (peek().type == BOR_OP) {
        Token op = peek();
        printToken(op);
        advance();
        bool_and_expr();
        postfix.push_back(op.lexeme);
    }
    exitRule("bool_or_expr");
}

void assign() {
    enterRule("assign");
    Token id = peek();
    if (id.type != IDENT) {
        cerr << "Syntax error: Expected identifier at start of assignment" << endl;
        exit(1);
    }
    printToken(id);
    advance();
    match(ASSIGN_OP);
    bool_or_expr();
    postfix.push_back("=");
    exitRule("assign");
}

void generateIC(const vector<string>& postfix) {
    cout << "\n-----------------------INTERMEDIATE CODE GENERATOR-----------------------" << endl;
    stack<string> s;
    char tempName = 'A';

    map<string, string> opMap;
    opMap["+"] = "add";
    opMap["-"] = "sub";
    opMap["*"] = "mul";
    opMap["/"] = "div";
    opMap["&&"] = "band";
    opMap["||"] = "bor";
    opMap["!"] = "bnot";
    opMap["=="] = "req";
    opMap["!="] = "rneq";
    opMap["<"] = "rlt";
    opMap["<="] = "rleq";
    opMap[">"] = "rgt";
    opMap[">="] = "rgeq";

    for (int i = 0; i < (int)postfix.size(); i++) {
        string token = postfix[i];

        // Assignment operator is handled last
        if (token == "=") {
            string rhs = s.top(); s.pop();
            string lhs = s.top(); s.pop();
            cout << "top " << rhs << endl;
            cout << "pop()\n";
            cout << "top (" << lhs << ")\n";
            cout << "pop()\n";
            cout << "assign " << lhs << ", " << rhs << endl;
        }
        // Operator case
        else if (opMap.count(token)) {
            string b = s.top(); s.pop();
            string a = s.top(); s.pop();
            string temp = string(1, tempName++);
            cout << "" << token << "" << endl;
            cout << "top " << b << "\n";
            cout << "pop()\n";
            cout << "top " << a << "\n";
            cout << "pop()\n";
            cout << opMap[token] << ", " << a << ", " << b << ", " << temp << endl;
            cout << "push " << temp << endl;
            s.push(temp);
        }
        // Operand identifier or constant
        else {
            cout << "push " << token << endl;
            s.push(token);
        }
    }
}



vector<Token> tokenize(const string& line) {
    vector<Token> result;
    size_t i = 0;
    while (i < line.length()) {
        if (isspace(line[i])) { i++; continue; }

        if (isalpha(line[i]) || line[i] == '_') {
            string id;
            while (i < line.length() && (isalnum(line[i]) || line[i] == '_')) {
                id += line[i++];
            }
            result.push_back(Token(IDENT, id));
        } else if (isdigit(line[i])) {
            string num;
            while (i < line.length() && isdigit(line[i])) {
                num += line[i++];
            }
            result.push_back(Token(INT_CONST, num));
        } else {
            string twoChar = line.substr(i, 2);
            if (twoChar == "==" || twoChar == "!=" || twoChar == "<=" || 
                twoChar == ">=" || twoChar == "&&" || twoChar == "||") {
                if (twoChar == "==") result.push_back(Token(REQ_OP, twoChar));
                else if (twoChar == "!=") result.push_back(Token(RNEQ_OP, twoChar));
                else if (twoChar == "<=") result.push_back(Token(RLEQ_OP, twoChar));
                else if (twoChar == ">=") result.push_back(Token(RGEQ_OP, twoChar));
                else if (twoChar == "&&") result.push_back(Token(BAND_OP, twoChar));
                else result.push_back(Token(BOR_OP, twoChar));
                i += 2;
            } else {
                char c = line[i++];
                if (c == '=') result.push_back(Token(ASSIGN_OP, "="));
                else if (c == '+') result.push_back(Token(ADD_OP, "+"));
                else if (c == '-') result.push_back(Token(SUB_OP, "-"));
                else if (c == '*') result.push_back(Token(MULT_OP, "*"));
                else if (c == '/') result.push_back(Token(DIV_OP, "/"));
                else if (c == '<') result.push_back(Token(RLT_OP, "<"));
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
    result.push_back(Token(END_OF_FILE, "EOF"));
    return result;
}

int main() {
    ifstream fin("front.in");
    if (!fin.is_open()) {
        cerr << "Error: Could not open front.in" << endl;
        return 1;
    }

    string line;
    int assignmentNumber = 1;

    while (getline(fin, line)) {
        if (line.empty()) continue;

        cout << "\nProcessing line: " << line << endl;
        cout << "\n-----------------------START ASSIGNMENT " << assignmentNumber << "-----------------------" << endl;

        tokens = tokenize(line);
        currentIndex = 0;
        ruleId = 1;
        postfix.clear();

        // cout << "Tokens found (" << tokens.size() << "):" << endl;
        // for (size_t i = 0; i < tokens.size(); ++i) printToken(tokens[i]);

        cout << "-----------------------PARSING TREE-----------------------" << endl;
        assign();

        cout << "\n-----------------------POSTFIX EXPRESSION-----------------------\n";
        for (size_t i = 0; i < postfix.size(); ++i) cout << postfix[i] << " ";
        cout << endl;

        generateIC(postfix);
        cout << "\n-----------------------END Assignment " << assignmentNumber++ << "-----------------------\n";
    }

    fin.close();
    return 0;
}