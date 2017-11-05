#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

#include "analyzers/lexicalAnalyzer.h"
#include "analyzers/syntaxAnalyzer.h"
#include "analyzers/executor.h"
#include "enums.h"

using namespace std;
using namespace std::literals;

#define STATUS_OK 0
#define STATUS_INCORRECT_ARGS 1
#define STATUS_NO_LEXEMES 2
#define STATUS_SYNTAX_ERRORS 3

void print(string s1, string s2) {
	int len1 = 0, len2 = 0;

	for (size_t i = 0; i < s1.length(); i++)
		if (s1[i] != -47 && s1[i] != -48)
			len1++;

	for (size_t i = 0; i < s2.length(); i++)
		if (s2[i] != -47 && s2[i] != -48)
			len2++;

	printf("  %s%s%s%*s\n", BOLDBLUE, s1.c_str(), RESET, 16 - len1 + len2, s2.c_str());
}

void help() {
	cout << BOLDGREEN << "Usage:" << RESET << endl;
	cout << BOLDBLUE << "  ./modelLang [options] file" << RESET << endl << endl;

	cout << BOLDGREEN << "Options:" << RESET << endl;
	print(optPrintText, "print interpreted text (your program)");
	print(optPrintLexemes, "print processed lexemes");
	print(optPrintIdentifiers, "print processed in lexical and syntax alalyze table of identifiers");
	print(optPrintRpn, "print reverse polish notation of code");
	print(optPrintStack, "print stack during code execution");
	print(optPrintStackStepByStep, "print stack during code execution step by step");
	print(optPrintAll, "print all available information");
	print(optPrintTime, "print time for interpreting");
	print(optExecute, "execute interpreted program");
	cout << endl;

	cout << endl << "Additional information about language and syntax rules:" << endl << endl;

	cout << BOLDGREEN << "Data types:" << RESET << endl;
	print(LEX_INT, "integer data type");
	print(LEX_REAL, "real data type");
	print(LEX_BOOLEAN, "boolean data type");
	print(LEX_STRING, "string data type");
	cout << endl;

	cout << BOLDGREEN << "Keywords:" << RESET << endl;
	print(LEX_PROGRAM, "template keyword for source");
	cout << endl;
	print(LEX_READ, "read to identifier(s) values from standart input stream");
	print(LEX_WRITE, "write expression(s) to standart output stream");
	cout << endl;
	print(LEX_IF, "conditional operator (if)");
	print(LEX_ELSE, "else branch of conditional operator (else)");
	cout << endl;
	print(LEX_WHILE, "cycle operator (while)");
	print(LEX_DO, "cycle operator (do)");
	print(LEX_FOR, "cycle operator (for)");
	cout << endl;

	print(LEX_FALSE, "boolean constant (false)");
	print(LEX_TRUE, "boolean constant (true)");
	cout << endl;

	cout << BOLDGREEN << "Ariphmetics operations:" << RESET << endl;
	print(LEX_PLUS, "addition operation (only for integer, real and string data types)");
	print(LEX_MINUS, "subtraction operation (only for integer and real data types)");
	print(LEX_MULT, "multiplication operation (only for integer and real data types)");
	print(LEX_DIV, "division operation (only for integer and real data types)");
	print(LEX_MOD, "operation of taking the remainder from division (only for integer data type)");
	cout << endl;

	cout << BOLDGREEN << "Abridged arithmetic operation (perform an operation with an identifier and return result to it)" << RESET << endl;
	print(LEX_ADD_VALUE, "abridged addition (only for integer, real and string data types)");
	print(LEX_SUB_VALUE, "abridged subtraction (only for integer and real data types)");
	print(LEX_MULT_VALUE, "abridged multiplication (only for integer and real data types)");
	print(LEX_DIV_VALUE, "abridged division (only for integer and real data types)");
	cout<< endl;
	print(LEX_INCREMENT, "increment value of identifier (equal '+= 1')");
	print(LEX_DECREMENT, "decrement value of identifier (equal '-= 1')");
	cout << endl;

	cout << BOLDGREEN << "Logical operations:" << RESET << endl;
	print(LEX_NOT, "not operation (only for boolean data type)");
	print(LEX_AND, "and operation (only for boolean data type)");
	print(LEX_OR, "or operation (only for boolean data type)");
	cout << endl;

	cout << BOLDGREEN << "Comparison operations:" << RESET << endl;
	print(LEX_EQUAL, "operation 'equal'");
	print(LEX_NEQUAL, "operation 'not equal'");
	print(LEX_LESS, "operation 'less' (only for integer, real and string data types)");
	print(LEX_GREATER, "operation 'greater' (only for integer, real and string data types)");
	print(LEX_LESS_EQUAL, "operation 'less or equal' (only for integer and real data types)");
	print(LEX_GREATER_EQUAL, "operation 'greater ot equal' (only for integer and real data types)");
	cout << endl;

	cout << BOLDGREEN << "Template of the interpreted program (source code):" << RESET << endl;
	cout << "  " << LEX_PROGRAM << " {" << endl << "      [declaration(s)]" << endl << "      [operator(s)]" << endl << "  }" << endl;
	cout << endl;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Incorrect arguments of program" << endl << "use --help for usage" << endl;

		return STATUS_INCORRECT_ARGS;
	}

	if (argc == 2 && argv[1] == "--help"s) {
		help();

		return STATUS_OK;
	}

	string path;

	int printStack = 0;
	bool printText = false;
	bool printLexemes = false;
	bool printIdentifiers = false;
	bool printRPN = false;
	bool printTime = false;
	bool execute = false;

	for (int i = 1; i < argc; i++) {
		if (optPrintText == argv[i])
			printText = true;
		else if (optPrintLexemes == argv[i])
			printLexemes = true;
		else if (optPrintIdentifiers == argv[i])
			printIdentifiers = true;
		else if (optPrintRpn == argv[i])
			printRPN = true;
		else if (optPrintStack == argv[i])
			printStack = 1;
		else if (optPrintStackStepByStep == argv[i])
			printStack = 2;
		else if (optPrintTime == argv[i])
			printTime = true;
		else if (optPrintAll == argv[i]) {
			printText = printLexemes = printIdentifiers = printRPN = true;
			printStack = 1;
		}
		else if (optExecute == argv[i])
			execute = true;
		else if (path.empty())
			path = argv[i];
		else {
			cout << BOLDRED << "incorrect argument: " << argv[i] << RESET << endl;
			cout << "use --help for usage" << endl;

			return STATUS_INCORRECT_ARGS;
		}
	}

	if (printStack && !execute) {
		cout << BOLDYELLOW << "Warning: printing of the stack is possible only at the execution of the program." << RESET << endl;
		cout << "Use -e option to execute interpreted program or --help for more info." << endl;

		printStack = false;
	}

	try {
		clock_t startClock = clock();

		ifstream sourceFile(path);
		LexicalAnalyzer lexicalAnalyzer = LexicalAnalyzer(sourceFile);

		vector<Lexeme>& lexemes = lexicalAnalyzer.analyze();

		if (lexemes.size() < 2) {
			cout << "No lexemes founded" << endl;
			return STATUS_NO_LEXEMES;
		}

		if (printText)
			lexicalAnalyzer.printText();

		if (printLexemes)
			lexicalAnalyzer.printLexemes();

		if (printIdentifiers)
			lexicalAnalyzer.printIdentifiers();

		clock_t lexicalClock = clock();

		// Syntax and semantic analyze
		SyntaxAnalyzer syntaxAnalyzer(lexicalAnalyzer.getTextLines(), lexemes, lexicalAnalyzer.getIdentifiersTable());

		if (!syntaxAnalyzer.analyze())
			return STATUS_SYNTAX_ERRORS;

		if (printIdentifiers)
			syntaxAnalyzer.printIdentifiers();

		if (printRPN)
			syntaxAnalyzer.printReversePolishNotation();

		clock_t syntaxClock = clock();
		
		if (execute) {
			Executor executor(syntaxAnalyzer.getIdentifiersTable(), syntaxAnalyzer.getReversePolishNotation());

			executor.execute(printStack);
		}

		clock_t executeClock = clock();

		if (printTime) {
			double lexicalTime = double (lexicalClock - startClock) / CLOCKS_PER_SEC;
			double syntaxTime = double (syntaxClock - lexicalClock) / CLOCKS_PER_SEC;
			double executeTime = double (executeClock - syntaxClock) / CLOCKS_PER_SEC;

			cout << endl << BOLDBLUE << "Lexical analyzer time: " << RESET << (lexicalTime * 1000000) << " us" << endl;
			cout << BOLDBLUE << "Syntax analyzer time: " << RESET  << (syntaxTime * 1000000) << " us" << endl;

			if (execute)
				cout << BOLDBLUE << "Executor time: " << RESET  << (executeTime * 1000) << " ms" << endl;

			cout << BOLDBLUE << "Total perform time: " << RESET  << (lexicalTime + syntaxTime + executeTime) * 1000 << " ms" << endl;
		}
	}
	catch (string s) {
		cout << BOLDRED << s << RESET << endl;
	}

	return 0;
}