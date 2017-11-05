#include "lexeme.h"

using namespace std;

Lexeme::Lexeme(LexemeT type, string value, unsigned long lineNumber) {
	this->type = type;
	this->lineNumber = lineNumber;
	this->value = value;
}

Lexeme::Lexeme(Identifier* ident) {
	this->value = ident->getValue();

	IdentifierT identT = ident->getType();

	if (identT == IdentifierT::identifier_int)
		this->type = LexemeT::constant_int;
	else if (identT == IdentifierT::identifier_real)
		this->type = LexemeT::constant_real;	
	else if (identT == IdentifierT::identifier_boolean)
		this->type = LexemeT::constant_boolean;	
	else if (identT == IdentifierT::identifier_string)
		this->type = LexemeT::constant_string;
	else
		throw string("Can't convert identifier to constant lexeme");
}

void Lexeme::setType(LexemeT type) {
	this->type = type;
}

LexemeT Lexeme::getType() const {
	return type;
}


void Lexeme::setValue(const string& value) {
	this->value = value;
}

string& Lexeme::getValue() {
	return value;
}

void Lexeme::setLineNumber(unsigned long lineNumber) {
	this->lineNumber = lineNumber;
}

unsigned long Lexeme::getLineNumber() const {
	return lineNumber;
}

bool Lexeme::check(LexemeT type, const string &value) const {
	return this->type == type && this->value == value;
}

bool Lexeme::isConstant() const {
	return type == LexemeT::constant_int || type == LexemeT::constant_real || type == LexemeT::constant_string || type == LexemeT::constant_boolean;
}

bool Lexeme::isDatatype() const {
	return type == LexemeT::datatype_int || type == LexemeT::datatype_real || type == LexemeT::datatype_string || type == LexemeT::datatype_boolean;
}

bool Lexeme::isIncrementOrDecrement() const {
	return type == LexemeT::delimeter && (value == LEX_INCREMENT || value == LEX_DECREMENT);
}

bool Lexeme::isComparison() const {
	return type == LexemeT::delimeter && 
		(value == LEX_EQUAL || value == LEX_NEQUAL || 
		 value == LEX_LESS || value == LEX_GREATER ||
		 value == LEX_LESS_EQUAL || value == LEX_GREATER_EQUAL);
}

bool Lexeme::isAriphmeticOp() const {
	return type == LexemeT::delimeter && (value == LEX_PLUS || value == LEX_MINUS || value == LEX_MULT || value == LEX_DIV || value == LEX_MOD);
}

bool Lexeme::isShortAriphmeticOp() const {
	return type == LexemeT::delimeter && (value == LEX_ADD_VALUE || value == LEX_SUB_VALUE || value == LEX_MULT_VALUE || value == LEX_DIV_VALUE);
}

IdentifierT Lexeme::constantToIdentifier() const {
	if (type == LexemeT::constant_int)
		return IdentifierT::identifier_int;
	
	if (type == LexemeT::constant_real)
		return IdentifierT::identifier_real;
	
	if (type == LexemeT::constant_string)
		return IdentifierT::identifier_string;
	
	if (type == LexemeT::constant_boolean)
		return IdentifierT::identifier_boolean;
	 
	return IdentifierT::identifier_unknown;
}

IdentifierT Lexeme::datatypeToIdentifier() const {
	if (type == LexemeT::datatype_int)
		return IdentifierT::identifier_int;
	
	if (type == LexemeT::datatype_real)
		return IdentifierT::identifier_real;
	
	if (type == LexemeT::datatype_string)
		return IdentifierT::identifier_string;
	
	if (type == LexemeT::datatype_boolean)
		return IdentifierT::identifier_boolean;
	
	return IdentifierT::identifier_unknown;
}

bool Lexeme::equalTypes(IdentifierT identT) const {
	return ((type == LexemeT::constant_int || type == LexemeT::datatype_int) && (identT == IdentifierT::identifier_int || identT == IdentifierT::identifier_real)) ||
		((type == LexemeT::constant_real || type == LexemeT::datatype_real) && identT == IdentifierT::identifier_real) ||
		((type == LexemeT::constant_string || type == LexemeT::datatype_string) && identT == IdentifierT::identifier_string) ||
		((type == LexemeT::constant_boolean || type == LexemeT::datatype_boolean) && identT == IdentifierT::identifier_boolean);
}

string Lexeme::printForRPN(bool colorful) const {
	string out("[");
	string lexT;
	string color;

	switch (type) {
		case LexemeT::rpn_increment:
			lexT = "++";
			color = BLUE;
			break;

		case LexemeT::rpn_decrement:
			lexT = "--";
			color = BLUE;
			break;

		case LexemeT::rpn_short_op:
			lexT = value;
			lexT += "@";

			color = BLUE;
			break;

		case LexemeT::rpn_minus:
			lexT = "-#";
			color = BLUE;
			break;

		case LexemeT::rpn_go:
			lexT = "go";
			color = BLUE;
			break;

		case LexemeT::rpn_fgo:
			lexT = "fgo";
			color = BLUE;
			break;

		case LexemeT::rpn_label:
			lexT = "L";
			lexT += value;

			color = BLUE;
			break;

		case LexemeT::rpn_address:
			lexT = "&";
			lexT += value;

			color = BLUE;
			break;

		case LexemeT::constant_int:
			lexT = value;
			lexT += " (int)";

			color = GREEN;
			break;

		case LexemeT::constant_real:
			lexT = value;
			lexT += " (real)";

			color = GREEN;
			break;

		case LexemeT::constant_boolean:
			lexT = value;
			lexT += " (boolean)";

			color = GREEN;
			break;

		default:
			for (size_t i = 0; i < value.length(); i++)
				if (value[i] == '\n')
					lexT += "\\n";
				else
					lexT += value[i];

			color = GREEN;
			break;
	}

	if (colorful)
		out += color;

	out += lexT;

	if (colorful)
		out += RESET;

	out += "]";
	
	return out;
}

ostream &operator<<(ostream &stream, const Lexeme &lexeme) {
	string lexT, color;

	switch (lexeme.type) {
		case LexemeT::keyword:
			lexT = "Keyword";
			color = BOLDCYAN;
			break;

		case LexemeT::delimeter:
			lexT = "Delimeter";
			break;

		case LexemeT::identifier:
			lexT = "Identifier";
			color = BOLDYELLOW;
			break;

		case LexemeT::constant_int:
			lexT = "Constant (INT)";
			color = BOLDGREEN;
			break;

		case LexemeT::constant_real:
			lexT = "Constant (REAL)";
			color = BOLDGREEN;
			break;

		case LexemeT::constant_string:
			lexT = "Constant (STRING)";
			color = BOLDGREEN;
			break;

		case LexemeT::constant_boolean:
			lexT = "Constant (BOOLEAN)";
			color = BOLDGREEN;
			break;


		case LexemeT::datatype_int:
			lexT = "Datatype_int";
			color = BLUE;
			break;

		case LexemeT::datatype_real:
			lexT = "Datatype_real";
			color = BLUE;
			break;

		case LexemeT::datatype_string:
			lexT = "Datatype_string";
			color = BLUE;
			break;

		case LexemeT::datatype_boolean:
			lexT = "Datatype_boolean";
			color = BLUE;
			break;

		case LexemeT::comment:
			lexT = "Comment";
			color = GREEN;
			break;

		case LexemeT::null:
			lexT = "Null";
			color = BOLDBLACK;
			break;

		case LexemeT::rpn_go:
			lexT = "GO";
			color = YELLOW;
			break;

		case LexemeT::rpn_fgo:
			lexT = "FGO";
			color = YELLOW;
			break;

		case LexemeT::rpn_label:
			lexT = "Label";
			color = YELLOW;
			break;

		case LexemeT::rpn_address:
			lexT = "ADDRESS";
			color = YELLOW;
			break;

		case LexemeT::rpn_minus:
			lexT = "-#";
			color = YELLOW;
			break;

		case LexemeT::rpn_increment:
			lexT = "INCREMENT";
			color = YELLOW;
			break;

		case LexemeT::rpn_decrement:
			lexT = "DECREMENT";
			color = YELLOW;
			break;

		case LexemeT::unknown:
		default:
			lexT = "Unknown";
			color = RED;
			break;
	}

	string value;

	if (lexeme.type == LexemeT::delimeter) {
		value = "[";
		value += lexeme.value;
		value += "]";
	} 
	else
		value = lexeme.value;

	stream << "|";
	printCenter(stream, value, 32, color);
	stream << "|";
	printCenter(stream, lexT, 18);
	stream << "|";
	printCenter(stream, string("line ") + to_string(lexeme.getLineNumber() + 1), 11);
	stream << "|";

    return stream;
}