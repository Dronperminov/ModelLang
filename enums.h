#ifndef LEX_TYPES_H
#define LEX_TYPES_H

#include <vector>
#include <string>

// OPTIONS KEY
const std::string optPrintText = "-T";
const std::string optPrintLexemes = "-L";
const std::string optPrintIdentifiers = "-I";
const std::string optPrintRpn = "-R";
const std::string optPrintStack = "-S";
const std::string optPrintStackStepByStep = "-SS";
const std::string optPrintAll = "-A";
const std::string optPrintTime = "-t";
const std::string optExecute = "-e";

// KEYWORDS
#define LEX_PROGRAM "program"

#define LEX_READ "read"
#define LEX_WRITE "write"

#define LEX_NOT "not"
#define LEX_AND "and"
#define LEX_OR "or"

#define LEX_IF "if"
#define LEX_ELSE "else"

#define LEX_FOR "for"
#define LEX_WHILE "while"
#define LEX_DO "do"

#define LEX_BREAK "break"

#define LEX_FALSE "false"
#define LEX_TRUE "true"

#define LEX_INT "int"
#define LEX_BOOLEAN "bool"
#define LEX_STRING "string"
#define LEX_REAL "real"

// DELIMETERS
#define LEX_ML_COMMENT_BEGIN "/*"
#define LEX_ML_COMMENT_END "*/"
#define LEX_QUOTE "\""

#define LEX_SEMICOLON ";"
#define LEX_POINT "."
#define LEX_COMMA ","
#define LEX_QUESTION "?"
#define LEX_COLON ":"

#define LEX_LFIG_BRACKET "{"
#define LEX_RFIG_BRACKET "}"

#define LEX_LPARENTHESIS "("
#define LEX_RPARENTHESIS ")"

#define LEX_PLUS "+"
#define LEX_MINUS "-"
#define LEX_DIV "/"
#define LEX_MULT "*"
#define LEX_MOD "%"

#define LEX_EQUALS "="
#define LEX_EQUAL "=="
#define LEX_NEQUAL "!="
#define LEX_LESS "<"
#define LEX_GREATER ">"
#define LEX_LESS_EQUAL "<="
#define LEX_GREATER_EQUAL ">="

#define LEX_INCREMENT "++"
#define LEX_DECREMENT "--"

#define LEX_ADD_VALUE "+="
#define LEX_SUB_VALUE "-="
#define LEX_MULT_VALUE "*="
#define LEX_DIV_VALUE "/="
#define LEX_MOD_VALUE "%="

enum class LexemeT {
	unknown = -1,
	null,
	comment,
	keyword,
	delimeter,
	identifier,

	datatype_int,
	datatype_real,
	datatype_string,
	datatype_boolean,
	
	constant_int,
	constant_real,
	constant_string,
	constant_boolean,

	rpn_minus,

	rpn_increment,
	rpn_decrement,

	rpn_short_op,

	rpn_go,
	rpn_fgo,
	rpn_label,
	rpn_address
};

enum class IdentifierT {
	identifier_unknown,
	identifier_int,
	identifier_real,
	identifier_string,
	identifier_boolean
};

const std::vector <std::string> keywords = {
	LEX_PROGRAM,
	// io
	LEX_READ, LEX_WRITE,
	// logical operators
	LEX_NOT, LEX_AND, LEX_OR,

	// I. Conditional operator
	LEX_IF, LEX_ELSE,

	// II. Loop operators
	LEX_FOR, LEX_WHILE, LEX_DO,

	// III. Jump operators
	LEX_BREAK,

	// IV. boolean constant values
	LEX_FALSE, LEX_TRUE
};

const std::vector <std::string> delimeters = {
	// best delimeters
	LEX_ML_COMMENT_BEGIN, LEX_ML_COMMENT_END, LEX_QUOTE, 

	// assignment
	LEX_EQUALS,

	// main delimeter
	LEX_SEMICOLON, LEX_COMMA, LEX_QUESTION, LEX_COLON,
	// brackets
	LEX_LFIG_BRACKET, LEX_RFIG_BRACKET, LEX_LPARENTHESIS, LEX_RPARENTHESIS,
	// ariphmetic delimeters
	LEX_PLUS, LEX_MINUS, LEX_DIV, LEX_MULT, LEX_MOD, 
	// compare delimeters
	LEX_EQUAL, LEX_NEQUAL, LEX_LESS, LEX_GREATER, LEX_LESS_EQUAL, LEX_GREATER_EQUAL,
	// increment and decrement integer for one point
	LEX_INCREMENT, LEX_DECREMENT,
	// short ariphmetic operations
	LEX_ADD_VALUE, LEX_SUB_VALUE, LEX_MULT_VALUE, LEX_DIV_VALUE, LEX_MOD_VALUE
};

#endif