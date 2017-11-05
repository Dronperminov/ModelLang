#include "identifier.h"

using namespace std;

Identifier::Identifier(IdentifierT type, const string& name, const string& value, bool declared) {
	this->type = type;
	this->name = name;
	this->value = value;
	this->isDeclared = declared;
}

const string& Identifier::getName() {
	return name;
}

void Identifier::setName(const string& name) {
	this->name = name;
}

const string& Identifier::getValue() {
	return value;
}

void Identifier::setValue(const string& value) {
	this->value = value;
}

void Identifier::setDefaultValue() {
	switch (type) {
		case IdentifierT::identifier_int:
		case IdentifierT::identifier_real:
			value = "0";
			break;

		case IdentifierT::identifier_boolean:
			value = LEX_FALSE;
			break;

		case IdentifierT::identifier_string:
			value.clear();
			break;

		default:
			value.clear();
			break;
	}
}

IdentifierT Identifier::getType() const {
	return type;
}

void Identifier::setType(IdentifierT type) {
	this->type = type;
}

bool Identifier::getDeclared() const {
	return isDeclared;
}

void Identifier::setDeclared(bool declared) {
	this->isDeclared = declared;
}

LexemeT Identifier::toConstantLexemeT() const {
	switch (type) {
		case IdentifierT::identifier_int:
			return LexemeT::constant_int;

		case IdentifierT::identifier_real:
			return LexemeT::constant_real;

		case IdentifierT::identifier_string:
			return LexemeT::constant_string;

		case IdentifierT::identifier_boolean:
			return LexemeT::constant_boolean;

		case IdentifierT::identifier_unknown:
		default:
			return LexemeT::unknown;
	}
}

const string Identifier::printType() const {
	if(type == IdentifierT::identifier_int)
		return LEX_INT;

	if(type == IdentifierT::identifier_real)
		return LEX_REAL;
	
	if (type == IdentifierT::identifier_string)
		return LEX_STRING;
	
	if (type == IdentifierT::identifier_boolean)
		return LEX_BOOLEAN;
	 
	return "unknown";
}