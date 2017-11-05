#ifndef  IDENTIFIER_H
#define IDENTIFIER_H

#include <string>
#include <ostream>
#include <cstdio>

#include "lexeme.h"
#include "../enums.h"
#include "../colors.h"

class Identifier {
	std::string name;
	std::string value;
	IdentifierT type;

	bool isDeclared;

public:
	Identifier(IdentifierT type, const std::string& name, const std::string& value = "", bool isDeclared = false);

	const std::string &getName();
	void setName(const std::string& name);

	const std::string &getValue();
	void setValue(const std::string& value);
	void setDefaultValue();

	IdentifierT getType() const;
	void setType(IdentifierT type);

	bool getDeclared() const;
	void setDeclared(bool declared);

	LexemeT toConstantLexemeT() const;
	const std::string printType() const;
};

#endif