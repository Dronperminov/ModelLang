#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "../enums.h"
#include "../entities/lexeme.h"
#include "../entities/identifier.h"
#include "../entities/identifiersTable.h"

class LexicalAnalyzer {
	std::string progText;
	unsigned long charPosition, lineNumber;

	IdentifiersTable identifiersTable;
	std::vector<Lexeme> lexemes;
	std::vector<std::string> lines;

	Lexeme getLexem();

	Lexeme readFromLetter();
	Lexeme readFromDigit();
	Lexeme readFromQuote();
	Lexeme readFromSlash();
	Lexeme readDelimeter();

	void checkSource(std::string& source);

	std::string currChar();
	std::string nextChar(bool shift = true);

	bool isLetter(char symbol) const;
	bool isDigit(char symbol) const;
	bool isLetterOrDigit(char symbol) const;
	bool isAsDelimeter(char symbol) const;

	bool isKeyword(std::string& word) const;
	bool isDelimeter(std::string& word) const;

public:	
	LexicalAnalyzer(std::ifstream& sourceFile);
	LexicalAnalyzer(std::string& sourceText);

	std::vector<std::string>& getTextLines();
	std::vector<Lexeme>& analyze();
	IdentifiersTable& getIdentifiersTable();

	void printText() const;
	void printLexemes() const;
	void printIdentifiers() const;
};

#endif