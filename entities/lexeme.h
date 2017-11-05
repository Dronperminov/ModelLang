#ifndef LEXEME_H
#define LEXEME_H

#include <string>
#include <iostream>

#include "functions.h"
#include "identifier.h"
#include "../enums.h"
#include "../colors.h"

class Lexeme {
	LexemeT type;
	std::string value;
	unsigned long lineNumber;

public:
	Lexeme(LexemeT type = LexemeT::unknown, std::string value = "", unsigned long lineNumber = 0);
	Lexeme(class Identifier* ident);

	void setType(LexemeT type = LexemeT::unknown);
	LexemeT getType() const;

	void setValue(const std::string& value);
	std::string& getValue();

	void setLineNumber(unsigned long lineNumber);
	unsigned long getLineNumber() const;

	bool check(LexemeT type, const std::string& value) const;

	bool isConstant() const;
	bool isDatatype() const;
	bool isIncrementOrDecrement() const;

	bool isComparison() const;
	bool isAriphmeticOp() const;
	bool isShortAriphmeticOp() const;

	IdentifierT constantToIdentifier() const;
	IdentifierT datatypeToIdentifier() const;
	bool equalTypes(IdentifierT identT) const;

	std::string printForRPN(bool colorful = true) const;

	friend std::ostream &operator<<(std::ostream &stream, const Lexeme &lexeme);
};

#endif