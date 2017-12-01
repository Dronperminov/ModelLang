#include "syntaxAnalyzer.h"

using namespace std;

SyntaxAnalyzer::SyntaxAnalyzer(vector<string> &lines, vector<Lexeme> &lexemes, IdentifiersTable& identifiersTable) {
	this->lines = lines;
	this->lexemes = lexemes;
	this->identifiersTable = identifiersTable;

	this->posLexeme = 0;
}

Lexeme &SyntaxAnalyzer::currLexeme() {
	return lexemes[posLexeme];
}

Lexeme &SyntaxAnalyzer::nextLexeme() {
	return lexemes[++posLexeme];
}

bool SyntaxAnalyzer::haveLexemes() {
	return currLexeme().getType() != LexemeT::null;
}

void SyntaxAnalyzer::checkLexemes() {
	bool haveUnknownLexemes = false;
	Lexeme unknownLex;

	for(auto i = lexemes.begin(); i < lexemes.end(); i++) {
		if (i->getType() == LexemeT::unknown) {
			cout << BOLDRED << endl << "Lexical error: " << i->getValue() << " on line " << (i->getLineNumber() + 1) << RESET;
			haveUnknownLexemes = true;
			unknownLex = *i;
		}
		else if (i->getType() == LexemeT::constant_real) {
			string& value = i->getValue();

			int points = 0;

			for (size_t index = 0; index < value.length(); index++) {
				if (value[index] == '.')
					points++;

				if (points > 1)
					throw Lexeme(LexemeT::constant_real, "real constant can't contain more than one point", i->getLineNumber());
			}
		}
	}

	if (haveUnknownLexemes) {
		unknownLex.setValue("There are unknown lexemes in file.");
		throw unknownLex;
	}
}

void SyntaxAnalyzer::checkProgram() {
	size_t lexemesSize = lexemes.size();

	if (lexemesSize < 4)
		throw string("Fatal error: program space not found");
	
	if (!lexemes.at(0).check(LexemeT::keyword, LEX_PROGRAM) || 
		!lexemes.at(1).check(LexemeT::delimeter, LEX_LFIG_BRACKET) ||
		!lexemes.at(lexemesSize - 2).check(LexemeT::delimeter, LEX_RFIG_BRACKET))
		throw string("Fatal error: program template (") + string(LEX_PROGRAM) + string(" { ... }) is incorrect");

	// start after 'program' without {}
	lexemes.erase(lexemes.begin());
	lexemes.erase(lexemes.begin());
	lexemes.erase(lexemes.end() - 2);
}

string SyntaxAnalyzer::printIdentifierT(IdentifierT identT) {
	switch (identT) {
		case IdentifierT::identifier_int:
			return LEX_INT;

		case IdentifierT::identifier_real:
			return LEX_REAL;
		
		case IdentifierT::identifier_string:
			return LEX_STRING;
		
		case IdentifierT::identifier_boolean:
			return LEX_BOOLEAN;

		default:
			return "unknown";
	}
}

void SyntaxAnalyzer::declaration(IdentifierT identT) {
	Lexeme lexeme = nextLexeme();

	if (lexeme.getType() == LexemeT::identifier) {
		Identifier* identifier = identifiersTable.getByName(lexeme.getValue());

		if (identifier == nullptr)
			throw "Unexpected identifier. '" + lexeme.getValue() + "' does not exist in identifier table";
		
		if (identifier->getDeclared())
			throw "'" + identifier->getName() + "' is already declared";
		
		identifier->setDeclared(true);
		identifier->setType(identT);
		identifier->setDefaultValue();

		lexeme = nextLexeme();

		if (lexeme.check(LexemeT::delimeter, LEX_EQUALS)) {
			lexeme = nextLexeme();

			string sign;

			// if initialize int constant
			if ((identT == IdentifierT::identifier_int || identT == IdentifierT::identifier_real) && 
				(lexeme.check(LexemeT::delimeter, LEX_PLUS) || lexeme.check(LexemeT::delimeter, LEX_MINUS))) {

				if (lexeme.getValue() != LEX_PLUS)
					sign = lexeme.getValue();

				lexemes.erase(lexemes.begin() + posLexeme); // delete sign lexeme
				lexeme = currLexeme();
			}

			if (!lexeme.isConstant())
				throw "Constant value expected. " + lexeme.getValue() + " is not a constant.";
			else if (lexeme.equalTypes(identT)) {
				lexeme.setValue(sign + lexeme.getValue());
				(&(*identifier))->setValue(lexeme.getValue());
				nextLexeme();
			} else 
				throw "Type of constant value (" + lexeme.getValue() + ") doesn't match with identifier type (" + identifier->printType() + ")";
		}
	} else
		throw "Identifier name expected. " + lexeme.getValue() + " is not a name.";
}

void SyntaxAnalyzer::declarations(IdentifierT identT) {	
	declaration(identT);

	if (!currLexeme().check(LexemeT::delimeter, LEX_SEMICOLON)) {
		if (currLexeme().check(LexemeT::delimeter, LEX_COMMA))
			declarations(identT);
		else
			throw "Incorrect lexeme for declaration '" + currLexeme().getValue() + "'.";
	} else 
		nextLexeme();
}

void SyntaxAnalyzer::checkDelimeter(const string& delimeter) {
	if (!currLexeme().check(LexemeT::delimeter, delimeter))
		throw "'" + delimeter + "' expected before " + currLexeme().getValue();
}

void SyntaxAnalyzer::checkKeyword(const string& keyword) {
	if (!currLexeme().check(LexemeT::keyword, keyword))
		throw "'" + keyword + "' expected before " + currLexeme().getValue();
}

IdentifierT SyntaxAnalyzer::identifier() {
	if (currLexeme().getType() != LexemeT::identifier)
		throw "'" + currLexeme().getValue() + "' is not an identifier";
	
	Identifier* identifier = identifiersTable.getByName(currLexeme().getValue());

	if (identifier == nullptr)
		throw "Unexpected identifier. '" + currLexeme().getValue() + "' does not exist in identifier table";
	
	if (!identifier->getDeclared())
		throw "'" + currLexeme().getValue() + "' is not declared.";
	
	nextLexeme();

	return identifier->getType();
}

void SyntaxAnalyzer::assignment() {
	Lexeme leftLexeme = currLexeme();
	IdentifierT leftType = identifier();

	rpn.push_back(Lexeme(LexemeT::rpn_address, leftLexeme.getValue()));

	if (currLexeme().isIncrementOrDecrement()) {
		string lexV = currLexeme().getValue(); 

		if (leftType != IdentifierT::identifier_int)
			throw "'" + lexV + "' is incompatible with " + printIdentifierT(leftType);

		rpn.push_back(Lexeme(lexV == LEX_INCREMENT ? LexemeT::rpn_increment : LexemeT::rpn_decrement, lexV, currLexeme().getLineNumber()));
		nextLexeme();
	}
	else if (currLexeme().isShortAriphmeticOp()) {
		//rpn.push_back(Lexeme(LexemeT::identifier, leftLexeme.getValue()));
		
		Lexeme sign;
		string& value = currLexeme().getValue();

		if(value == LEX_ADD_VALUE)
			sign = Lexeme(LexemeT::delimeter, LEX_PLUS);
		else if (value == LEX_SUB_VALUE)
			sign = Lexeme(LexemeT::delimeter, LEX_MINUS);
		else if (value == LEX_MULT_VALUE) 
			sign = Lexeme(LexemeT::delimeter, LEX_MULT);
		else
			sign = Lexeme(LexemeT::delimeter, LEX_DIV);

		nextLexeme();
		IdentifierT rightType = expression();

		overallType(leftType, sign, rightType);

		rpn.push_back(Lexeme(LexemeT::rpn_short_op, sign.getValue()));
		//rpn.push_back(Lexeme(LexemeT::delimeter, sign.getValue()));
		//rpn.push_back(Lexeme(LexemeT::delimeter, LEX_EQUALS));
	}
	else {
		checkDelimeter(LEX_EQUALS);
		nextLexeme();
	
		Lexeme& rightLexeme = currLexeme();
		IdentifierT rightType = expression();

		bool canAssign = false;

		if ((leftType == IdentifierT::identifier_real || leftType == IdentifierT::identifier_int) && 
			(rightType == IdentifierT::identifier_real || rightType == IdentifierT::identifier_int))
			canAssign = true;
		else 
			canAssign = leftType == rightType;

		if (!canAssign) {
			throw "'" + leftLexeme.getValue() + "' has type " + printIdentifierT(leftType) + " which is incompatible with '"
				  + rightLexeme.getValue() + "' that has type " + printIdentifierT(rightType);
		}

		rpn.push_back(Lexeme(LexemeT::delimeter, LEX_EQUALS));
	}
}

IdentifierT SyntaxAnalyzer::invertEntity(IdentifierT identT) {
	if (identT == IdentifierT::identifier_boolean)
		return IdentifierT::identifier_boolean;
	
	throw string("'not' can't invert non boolean expression");
}

IdentifierT SyntaxAnalyzer::overallType(IdentifierT leftType, Lexeme& sign, IdentifierT rightType) {
	LexemeT lexT = sign.getType();
	string& lexV = sign.getValue();
	
	// ariphmetic operations can be applied only on integer types (and '+' on string for concatination)
	if (sign.isAriphmeticOp()) {
		if (leftType == IdentifierT::identifier_string && lexV == LEX_PLUS) {
			if (rightType == IdentifierT::identifier_string)
				return IdentifierT::identifier_string;
			
			if (rightType == IdentifierT::identifier_boolean)
				throw string("Can't concatenate string with boolean type");
			
			return IdentifierT::identifier_string;
		}

		if (leftType == IdentifierT::identifier_real && (rightType == IdentifierT::identifier_real || rightType == IdentifierT::identifier_int)) {
			if (lexV != LEX_MOD)
				return IdentifierT::identifier_real;
			else
				throw string("Can't not take the remainder of the division by a real type");
		}

		if (leftType == rightType && (leftType == IdentifierT::identifier_int))
			return IdentifierT::identifier_int;
		
		if ((leftType == IdentifierT::identifier_real || leftType == IdentifierT::identifier_int) &&
			(rightType == IdentifierT::identifier_real || rightType == IdentifierT::identifier_int))
			return IdentifierT::identifier_real;
		
		throw "'" + sign.getValue() + "' incompatible with given types ('" + printIdentifierT(leftType) + "' and '" + printIdentifierT(rightType) + "')";
	}

	// boolean operations OR and AND can be applyed only on boolean types
	if (lexT == LexemeT::keyword && (lexV == LEX_OR || lexV == LEX_AND)) {
		if (leftType == rightType && leftType == IdentifierT::identifier_boolean)
			return IdentifierT::identifier_boolean;
		
		throw string("logic operation can't be applied to non-boolean variables");
	} 
	
	// ==, !=, <, >, <=, >= operations can be applied on int, real and string types (string without <=, >=)
	if (sign.isComparison()) {
		if (leftType == IdentifierT::identifier_real && (rightType == IdentifierT::identifier_real || rightType == IdentifierT::identifier_int))
			return IdentifierT::identifier_boolean;

		if (leftType == rightType && leftType == IdentifierT::identifier_int)
			return IdentifierT::identifier_boolean;

		if ((leftType == IdentifierT::identifier_real || leftType == IdentifierT::identifier_int) &&
		   (rightType == IdentifierT::identifier_real || rightType == IdentifierT::identifier_int))
			return IdentifierT::identifier_boolean;

		if (lexV != LEX_LESS_EQUAL && lexV != LEX_GREATER_EQUAL) {
			if (leftType == rightType && leftType == IdentifierT::identifier_string)
				return IdentifierT::identifier_boolean;	
		}

		throw "'" + sign.getValue() + "' incompatible with given types ('" + printIdentifierT(leftType) + "' and '" + printIdentifierT(rightType) + "')";
	}
	
	throw "'" + sign.getValue() + "' is not a proper sign";
}

IdentifierT SyntaxAnalyzer::unaryMinus() {
	LexemeT lexT = currLexeme().getType();

	if (currLexeme().isConstant()) {
		if (lexT == LexemeT::constant_real || lexT == LexemeT::constant_int) {
			rpn.push_back(Lexeme(currLexeme().getType(), currLexeme().getValue()));
			rpn.push_back(Lexeme(LexemeT::rpn_minus, LEX_MINUS));

			Lexeme& curLex = currLexeme();
			nextLexeme();

			return curLex.constantToIdentifier();
		}

		throw "unary minus is incompatible with type " + printIdentifierT(currLexeme().constantToIdentifier());
	} else if (currLexeme().getType() == LexemeT::identifier) {
		rpn.push_back(Lexeme(LexemeT::identifier, currLexeme().getValue()));

		Identifier* identifier = identifiersTable.getByName(currLexeme().getValue());

		if (identifier == nullptr)
			throw "Unexpected identifier. '" + currLexeme().getValue() + "' does not exist in identifier table";
		
		if (!identifier->getDeclared())
			throw "'" + currLexeme().getValue() + "' is not declared.";

		IdentifierT identT = identifier->getType();

		if (identT == IdentifierT::identifier_real || identT == IdentifierT::identifier_int)
			rpn.push_back(Lexeme(LexemeT::rpn_minus, LEX_MINUS));
		else 
			throw "unary minus is incompatible with type " + printIdentifierT(identT);
		
		nextLexeme();

		return identT;
	}
	else if (currLexeme().check(LexemeT::delimeter, LEX_LPARENTHESIS)) {
		nextLexeme();
		IdentifierT entT = expression();

		checkDelimeter(LEX_RPARENTHESIS);
		nextLexeme();

		rpn.push_back(Lexeme(LexemeT::rpn_minus, LEX_MINUS));

		return entT;
	}

	throw "'" + currLexeme().getValue() + "' is not an entity";	
}

IdentifierT SyntaxAnalyzer::ternaryOp(IdentifierT identT) {
	if (identT != IdentifierT::identifier_boolean)
		throw string("expected boolean expression for ternary operator");

	unsigned long expIndex = rpn.size();
	rpn.push_back(Lexeme(LexemeT::rpn_label, "0"));
	rpn.push_back(Lexeme(LexemeT::rpn_fgo, "0"));

	IdentifierT firstExprT = expression();

	checkDelimeter(LEX_COLON);
	nextLexeme();

	unsigned long exp2Index = rpn.size();
	rpn.push_back(Lexeme(LexemeT::rpn_label, "0"));
	rpn.push_back(Lexeme(LexemeT::rpn_go, "0"));
	rpn[expIndex].setValue(to_string(rpn.size()));

	IdentifierT secondExprT = expression();

	rpn[exp2Index].setValue(to_string(rpn.size()));

	if (secondExprT == firstExprT)
		return firstExprT;

	if ((firstExprT == IdentifierT::identifier_int || firstExprT == IdentifierT::identifier_real) &&
		(secondExprT == IdentifierT::identifier_int || secondExprT == IdentifierT::identifier_real))
		return IdentifierT::identifier_real;

	throw string("The types of expressions in the ternary operator must match");
}

IdentifierT SyntaxAnalyzer::expression() {
	IdentifierT leftType = term();

	if (currLexeme().check(LexemeT::delimeter, LEX_EQUAL) ||
		currLexeme().check(LexemeT::delimeter, LEX_GREATER) ||
		currLexeme().check(LexemeT::delimeter, LEX_LESS) ||
		currLexeme().check(LexemeT::delimeter, LEX_GREATER_EQUAL) ||
		currLexeme().check(LexemeT::delimeter, LEX_LESS_EQUAL) ||
		currLexeme().check(LexemeT::delimeter, LEX_NEQUAL)) {

		Lexeme& sign = currLexeme();
		nextLexeme();

		IdentifierT rightType = term();

		rpn.push_back(sign);

		IdentifierT overall = overallType(leftType, sign, rightType);

		if (currLexeme().check(LexemeT::delimeter, LEX_QUESTION)) {
			nextLexeme();
			overall = ternaryOp(overall);
		}

		return overall;
	}

	if (currLexeme().check(LexemeT::delimeter, LEX_QUESTION)) {
		nextLexeme();
		leftType = ternaryOp(leftType);
	}

	return leftType;
}

IdentifierT SyntaxAnalyzer::term() {
	IdentifierT leftType = term1();

	while (currLexeme().check(LexemeT::delimeter, LEX_PLUS) ||
		currLexeme().check(LexemeT::delimeter, LEX_MINUS) || 
		currLexeme().check(LexemeT::keyword, LEX_OR)) {

		Lexeme& sign = currLexeme();
		nextLexeme();
		IdentifierT rightType = term1();

		rpn.push_back(sign);

		leftType = overallType(leftType, sign, rightType);
	}

	return leftType;
}

IdentifierT SyntaxAnalyzer::term1() {
	IdentifierT leftType = entity();

	while (currLexeme().check(LexemeT::delimeter, LEX_MULT) ||
		   currLexeme().check(LexemeT::delimeter, LEX_DIV) ||
		   currLexeme().check(LexemeT::delimeter, LEX_MOD) ||
		   currLexeme().check(LexemeT::keyword, LEX_AND)) {

		Lexeme& sign = currLexeme();
		nextLexeme();
		IdentifierT rightType = entity();

		rpn.push_back(sign);

		leftType = overallType(leftType, sign, rightType);
	}

	return leftType;
}

IdentifierT SyntaxAnalyzer::entity() {
	if (currLexeme().getType() == LexemeT::identifier) {
		rpn.push_back(currLexeme());

		return identifier();
	} else if (currLexeme().isConstant()) {
		rpn.push_back(currLexeme());

		Lexeme& curLex = currLexeme();
		nextLexeme();

		return curLex.constantToIdentifier();
	}
	else if (currLexeme().check(LexemeT::keyword, LEX_NOT)) {
		nextLexeme();
		IdentifierT entT = entity();

		rpn.push_back(Lexeme(LexemeT::keyword, LEX_NOT));

		return invertEntity(entT);
	} else if (currLexeme().check(LexemeT::delimeter, LEX_LPARENTHESIS)) {
		nextLexeme();
		IdentifierT eType = expression();

		checkDelimeter(LEX_RPARENTHESIS);
		nextLexeme();

		return eType;
	} else if (currLexeme().check(LexemeT::delimeter, LEX_MINUS)) {
		nextLexeme();

		return unaryMinus();
	}
	else
		throw "'" + currLexeme().getValue() + "' is not an entity";
}

/************************************* OPERATORS PARSERS *****************************************/
void SyntaxAnalyzer::parseIf() {
	nextLexeme();

	checkDelimeter(LEX_LPARENTHESIS);	   
	nextLexeme();

	IdentifierT expT = expression();

	if (expT != IdentifierT::identifier_boolean)
		throw "'" + currLexeme().getValue() + "' expected boolean expression";

	unsigned long ifIndex = rpn.size();
	rpn.push_back(Lexeme(LexemeT::rpn_label, "0"));
	rpn.push_back(Lexeme(LexemeT::rpn_fgo, "0"));

	checkDelimeter(LEX_RPARENTHESIS);

	nextLexeme();
	parse();

	if (currLexeme().check(LexemeT::keyword, LEX_ELSE)) {
		unsigned long elseIndex = rpn.size();
		rpn.push_back(Lexeme(LexemeT::rpn_label, "0"));
		rpn.push_back(Lexeme(LexemeT::rpn_go, "0"));
		rpn[ifIndex].setValue(to_string(rpn.size()));

		nextLexeme();
		parse();

		rpn[elseIndex].setValue(to_string(rpn.size()));

		return;
	}

	rpn[ifIndex].setValue(to_string(rpn.size()));
}

void SyntaxAnalyzer::parseFor() {
	unsigned long startIndex = rpn.size();

	nextLexeme();

	checkDelimeter(LEX_LPARENTHESIS);		
	nextLexeme();

	if (!currLexeme().check(LexemeT::delimeter, LEX_SEMICOLON)) {
		assignment();
		checkDelimeter(LEX_SEMICOLON);
	}

	unsigned long expressionIndex = rpn.size();

	nextLexeme();

	if (!currLexeme().check(LexemeT::delimeter, LEX_SEMICOLON)) {
		IdentifierT expT = expression();

		unsigned long expressionLabel1 = rpn.size();
		rpn.push_back(Lexeme(LexemeT::rpn_label, "0"));
		rpn.push_back(Lexeme(LexemeT::rpn_fgo, "0"));

		if (expT != IdentifierT::identifier_boolean)
			throw "'" + currLexeme().getValue() + "' expected boolean expression";
		
		checkDelimeter(LEX_SEMICOLON);

		nextLexeme();

		vector <Lexeme> tmp;

		if (!currLexeme().check(LexemeT::delimeter, LEX_RPARENTHESIS)) {
			unsigned long assignmentIndex = rpn.size();
			assignment();
			unsigned long assignmentCount = rpn.size() - assignmentIndex;

			for (unsigned long i = 0; i < assignmentCount; i++) {
				tmp.push_back(rpn[assignmentIndex]);
				rpn.erase(rpn.begin() + assignmentIndex);
			}
		}

		checkDelimeter(LEX_RPARENTHESIS);

		nextLexeme();
		parse();
		
		rpn.insert(rpn.end(), tmp.begin(), tmp.end());
		rpn.push_back(Lexeme(LexemeT::rpn_label, to_string(expressionIndex)));
		rpn.push_back(Lexeme(LexemeT::rpn_go, "0"));
		rpn[expressionLabel1].setValue(to_string(rpn.size()));
	} else {
		nextLexeme();

		vector <Lexeme> tmp;

		if (!currLexeme().check(LexemeT::delimeter, LEX_RPARENTHESIS)) {
			unsigned long assignmentIndex = rpn.size();
			assignment();
			unsigned long assignmentCount = rpn.size() - assignmentIndex;

			for (unsigned long i = 0; i < assignmentCount; i++) {
				tmp.push_back(rpn[assignmentIndex]);
				rpn.erase(rpn.begin() + assignmentIndex);
			}
		}

		checkDelimeter(LEX_RPARENTHESIS);

		nextLexeme();
		parse();

		rpn.insert(rpn.end(), tmp.begin(), tmp.end());
		rpn.push_back(Lexeme(LexemeT::rpn_label, to_string(expressionIndex)));
		rpn.push_back(Lexeme(LexemeT::rpn_go, "0"));		
	}

	insertBreakLabels(startIndex, rpn.size(), rpn.size());
}

void SyntaxAnalyzer::parseWhile() {
	unsigned long startIndex = rpn.size();

	nextLexeme();

	checkDelimeter(LEX_LPARENTHESIS);		
	nextLexeme();

	unsigned long expressionIndex = rpn.size();

	IdentifierT expT = expression();

	unsigned long expressionLabel = rpn.size();
	rpn.push_back(Lexeme(LexemeT::rpn_label, "0"));
	rpn.push_back(Lexeme(LexemeT::rpn_fgo, "0"));

	if (expT != IdentifierT::identifier_boolean)
	  	throw "'" + currLexeme().getValue() + "' expected boolean expression";

	checkDelimeter(LEX_RPARENTHESIS);

	nextLexeme();
	parse();

	rpn.push_back(Lexeme(LexemeT::rpn_label, to_string(expressionIndex)));
	rpn.push_back(Lexeme(LexemeT::rpn_go, "0"));
	rpn[expressionLabel].setValue(to_string(rpn.size()));
	insertBreakLabels(startIndex, rpn.size(), rpn.size());
}

void SyntaxAnalyzer::parseDoWhile() {
	unsigned long startIndex = rpn.size();
	nextLexeme();
	unsigned long parseIndex = rpn.size();
	parse();

	checkKeyword(LEX_WHILE);
	nextLexeme();
	checkDelimeter(LEX_LPARENTHESIS);			
	nextLexeme();

	IdentifierT expT = expression();

	unsigned long expressionLabel = rpn.size();
	rpn.push_back(Lexeme(LexemeT::rpn_label, "0"));
	rpn.push_back(Lexeme(LexemeT::rpn_fgo, "0"));

	if (expT != IdentifierT::identifier_boolean)
		throw "'" + currLexeme().getValue() + "' expected boolean expression";

	checkDelimeter(LEX_RPARENTHESIS);
	nextLexeme();

	rpn.push_back(Lexeme(LexemeT::rpn_label, to_string(parseIndex)));
	rpn.push_back(Lexeme(LexemeT::rpn_go, "0"));
	rpn[expressionLabel].setValue(to_string(rpn.size()));
	insertBreakLabels(startIndex, rpn.size(), rpn.size());
}

void SyntaxAnalyzer::parseRead() {
	nextLexeme();

	checkDelimeter(LEX_LPARENTHESIS);
	nextLexeme();

	string address = currLexeme().getValue();
	identifier();

	rpn.push_back(Lexeme(LexemeT::rpn_address, address));
	rpn.push_back(Lexeme(LexemeT::keyword, LEX_READ));

	while (currLexeme().check(LexemeT::delimeter, LEX_COMMA)) {
		nextLexeme();
		string address1 = currLexeme().getValue();
		identifier();

		rpn.push_back(Lexeme(LexemeT::rpn_address, address1));
		rpn.push_back(Lexeme(LexemeT::keyword, LEX_READ));
	}

	checkDelimeter(LEX_RPARENTHESIS);
	nextLexeme();
}

void SyntaxAnalyzer::parseWrite() {
	nextLexeme();

	checkDelimeter(LEX_LPARENTHESIS);			
	nextLexeme();
	expression();

	rpn.push_back(Lexeme(LexemeT::keyword, LEX_WRITE));

	while (currLexeme().check(LexemeT::delimeter, LEX_COMMA)) {
		nextLexeme();		
		expression();

		rpn.push_back(Lexeme(LexemeT::keyword, LEX_WRITE));
	}

	checkDelimeter(LEX_RPARENTHESIS);
	nextLexeme();
}

void SyntaxAnalyzer::parse() {
	if (currLexeme().check(LexemeT::delimeter, LEX_LFIG_BRACKET)) {
		nextLexeme();

		while (!currLexeme().check(LexemeT::delimeter, LEX_RFIG_BRACKET)) {
			parse();
		}

		nextLexeme();
	} else if (currLexeme().check(LexemeT::keyword, LEX_IF))
		parseIf();
	else if (currLexeme().check(LexemeT::keyword, LEX_WHILE))
		parseWhile();
	else if (currLexeme().check(LexemeT::keyword, LEX_FOR))
		parseFor();
	else if (currLexeme().check(LexemeT::delimeter, LEX_SEMICOLON))
		nextLexeme();
	else {
		if (currLexeme().check(LexemeT::keyword, LEX_DO))
			parseDoWhile();
		else if (currLexeme().check(LexemeT::keyword, LEX_READ))
			parseRead();
		else if (currLexeme().check(LexemeT::keyword, LEX_WRITE))
			parseWrite();
		else if (currLexeme().isDatatype()) // declaration
			throw string("All declarations must be in first section");
		else // assignment
			assignment();

		checkDelimeter(LEX_SEMICOLON);		
		nextLexeme();
	}
}

void SyntaxAnalyzer::insertBreakLabels(unsigned long startIndex, unsigned long endIndex, unsigned long labelIndex) {
	for (unsigned long i = startIndex; i < endIndex; i++)
		if (rpn[i].check(LexemeT::rpn_label, "-1"))
			rpn[i].setValue(to_string(labelIndex));
}

void SyntaxAnalyzer::printIdentifiers() const {
	if (identifiersTable.size()) {
		cout << BOLDMAGENTA << endl << "Processed identifiers after syntax analyze (" << identifiersTable.size() << "):" << RESET << endl;
		cout << identifiersTable << endl;
	}
	else 
		cout << BOLDMAGENTA << endl << "Identifiers table is empty" << RESET << endl;
}

void SyntaxAnalyzer::printReversePolishNotation() const {
	cout << BOLDMAGENTA << endl << "Reverse polish notation:" << RESET << endl;

	for (auto i = rpn.begin(); i < rpn.end(); i++)
		cout << i->printForRPN() << " ";

	cout << endl << endl;
}

vector<Lexeme>& SyntaxAnalyzer::getReversePolishNotation() {
	return rpn;
}

IdentifiersTable& SyntaxAnalyzer::getIdentifiersTable() {
	return identifiersTable;
}

bool SyntaxAnalyzer::analyze() {
	try {
		checkLexemes();
		checkProgram();

		while (haveLexemes() && currLexeme().isDatatype())
			declarations(currLexeme().datatypeToIdentifier());		

		while (haveLexemes())
			parse();

		insertBreakLabels(0, rpn.size(), rpn.size());
	}
	catch (string &e) {
		cout << endl << YELLOW << lines[currLexeme().getLineNumber()] << RESET << "\t (line " << (currLexeme().getLineNumber() + 1)<< ")" << endl;
		cout << BOLDRED << e << RESET << endl;

		return false;
	}
	catch (Lexeme &lex) {
		cout << endl << YELLOW << lines[lex.getLineNumber()] << RESET << "\t (line " << (lex.getLineNumber() + 1)<< ")" << endl;
		cout << BOLDRED << lex.getValue() << RESET << endl;

		return false;	
	}

	return true;
}