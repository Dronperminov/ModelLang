#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H

#include <string>
#include <vector>
#include <algorithm>

#include "../enums.h"
#include "../entities/lexeme.h"
#include "../entities/identifier.h"
#include "../entities/identifiersTable.h"

class SyntaxAnalyzer {
	std::vector<Lexeme> lexemes;
	IdentifiersTable identifiersTable;
	std::vector<std::string> lines;
	std::vector<Lexeme> rpn; // reverse Polish notation

	size_t posLexeme;

	Lexeme& currLexeme();
	Lexeme& nextLexeme();
	bool haveLexemes();

	void checkLexemes();
	void checkProgram();

	std::string printIdentifierT(IdentifierT identT);

	void declaration(IdentifierT identT);
	void declarations(IdentifierT identT);

	void checkDelimeter(const std::string& delimeter);
	void checkKeyword(const std::string& keyword);

	IdentifierT identifier();
	void assignment();

	IdentifierT invertEntity(IdentifierT identT);
	IdentifierT overallType(IdentifierT leftType, Lexeme& sign, IdentifierT rightType);

	IdentifierT unaryMinus();
	IdentifierT ternaryOp(IdentifierT identT);

	IdentifierT expression();
	IdentifierT term();
	IdentifierT term1();
	IdentifierT entity();

	// RPN
	void insertBreakLabels(unsigned long startIndex, unsigned long endIndex, unsigned long labelIndex);

	void parseIf();
	void parseFor();
	void parseWhile();
	void parseDoWhile();
	void parseRead();
	void parseWrite();
	void parseBreak();

	void parse();

public:
	SyntaxAnalyzer(std::vector<std::string> &lines, std::vector<Lexeme> &lexemes, IdentifiersTable &identifiersTable);

	void printIdentifiers() const;
	void printReversePolishNotation() const;

	std::vector<Lexeme>& getReversePolishNotation();
	IdentifiersTable& getIdentifiersTable();

	bool analyze();
};

#endif