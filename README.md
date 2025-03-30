# C Lexical Analyzer

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

## How to Compile

To compile the program using `g++`, run the following command in your terminal:

```bash
g++ "lexical analyzer.cpp" -o lexer


