#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>

#include "../enums.h"

#include "../entities/functions.h"
#include "../entities/lexeme.h"
#include "../entities/identifier.h"
#include "../entities/identifiersTable.h"

#include "../ariphmetics/LongInt.h"
#include "../ariphmetics/LongDouble.h"

class Executor {
	IdentifiersTable identifiersTable;
	std::vector<Lexeme> rpn; // reverse polish notation
	std::stack<Lexeme> stack;

	std::string realToString(LongDouble v) const;
	std::string intToString(LongInt v) const;
	LongDouble stringToReal(const std::string& v) const;
	LongInt stringToInt(const std::string& v) const;

	LongInt lexemeToInt(Lexeme& lexeme);
	LongDouble lexemeToReal(Lexeme& lexeme);
	bool lexemeToBool(Lexeme& lexeme);
	std::string lexemeToString(Lexeme& lexeme);

	IdentifierT getLexemeType(Lexeme& lexeme);

	// execute functions
	void executeIdentifiers(std::string& name);
	void executeUnaryMinus();
	void executeIncDec(int value);
	void executeShortOp(std::string& lexV);

	void executeNot();
	void executeOr();
	void executeAnd();

	void executeAriphmetics(std::string& lexV);
	void executeEqualities(std::string& lexV);
	void executeComparsions(std::string& lexV);

	void executeGo(unsigned long& rpnIndex);
	void executeFgo(unsigned long& rpnIndex);

	void executeRead();
	void executeWrite();
	void executeAssign();

	void printStackDump(unsigned long rpnIndex, const std::string& lexV);

public:
	Executor(IdentifiersTable& identifiersTable, std::vector<Lexeme>& rpn);
	void execute(int printStack);
};

#endif