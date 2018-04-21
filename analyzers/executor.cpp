#include "executor.h"

using namespace std;

Executor::Executor(IdentifiersTable& identifiersTable, vector<Lexeme>& rpn) {
	this->identifiersTable = identifiersTable;
	this->rpn = rpn;
}

string Executor::realToString(LongDouble v) const {
	return v.toString();
}

string Executor::intToString(LongInt v) const {
	return v.toString();
}

LongDouble Executor::stringToReal(const string& v) const {
	return LongDouble(v);
}

LongInt Executor::stringToInt(const string& v) const {
	return LongInt(v);
}

LongInt Executor::lexemeToInt(Lexeme& lexeme) {
	LexemeT lexT = lexeme.getType();

	if (lexT == LexemeT::constant_int || lexT == LexemeT::rpn_label)
		return stringToInt(lexeme.getValue());
	
	throw string("Expected int lexeme");
}

LongDouble Executor::lexemeToReal(Lexeme& lexeme) {
	LexemeT lexT = lexeme.getType();

	if (lexT == LexemeT::constant_real || lexT == LexemeT::constant_int)
		return stringToReal(lexeme.getValue());
	
	throw string("Expected real lexeme");
}

bool Executor::lexemeToBool(Lexeme& lexeme) {
	if (lexeme.getType()== LexemeT::constant_boolean)
		return lexeme.getValue() == LEX_TRUE;

	throw string("Expected boolean lexeme");
}

string Executor::lexemeToString(Lexeme& lexeme) {
	if (lexeme.getType() == LexemeT::constant_string)
		return lexeme.getValue();

	throw string("Expected string lexeme");
}

IdentifierT Executor::getLexemeType(Lexeme& lexeme) {
	LexemeT lexT = lexeme.getType();

	switch (lexT) {
		case LexemeT::constant_int:
			return IdentifierT::identifier_int;

		case LexemeT::constant_real:
			return IdentifierT::identifier_real;

		case LexemeT::constant_string:
			return IdentifierT::identifier_string;

		case LexemeT::constant_boolean:
			return IdentifierT::identifier_boolean;

		case LexemeT::identifier:
		case LexemeT::rpn_address:
			return identifiersTable.getByName(lexeme.getValue())->getType();

		default:
			return IdentifierT::identifier_unknown;
	}
}

void Executor::executeIdentifiers(string& name) {
	Identifier* ident = identifiersTable.getByName(name);

	if (ident != nullptr)
		stack.push(Lexeme(ident));
	else
		throw string("Undefined identifier");
}

void Executor::executeUnaryMinus() {
	string& v = stack.top().getValue();

	if (v.find(LEX_MINUS) == string::npos)
		v = LEX_MINUS + v;
	else
		v.erase(0, 1);

	stack.top().setValue(v);
}

void Executor::executeIncDec(int value) {
	Identifier* ident = identifiersTable.getByName(stack.top().getValue());
	stack.pop();

	LongInt v = stringToInt(ident->getValue());

	ident->setValue(intToString(v + value));
}

void Executor::executeShortOp(string& lexV) {
	Lexeme arg = stack.top();
	stack.pop();

	Identifier* ident = identifiersTable.getByName(stack.top().getValue());
	IdentifierT type = ident->getType();
	stack.pop();

	string result;

	if (type == IdentifierT::identifier_string) {
		result = ident->getValue();
		result += arg.getValue();
	}
	else if (type == IdentifierT::identifier_int) {
		LongInt v1 = lexemeToInt(arg);
		LongInt v2 = stringToInt(ident->getValue());
		LongInt res;

		if (lexV == LEX_PLUS)
			res = v2 + v1;
		else if (lexV == LEX_MINUS)
			res = v2 - v1;
		else if (lexV == LEX_MULT)
			res = v2 * v1;
		else if (v1 != 0)
			res = (lexV == LEX_DIV) ? (v2 / v1) : (v2 % v1);
		else
			throw string("Division by zero");

		result = intToString(res);
	} else if (type == IdentifierT::identifier_real) {
		LongDouble v1 = lexemeToReal(arg);
		LongDouble v2 = stringToReal(ident->getValue());
		LongDouble res;

		if (lexV == LEX_PLUS)
			res = v2 + v1;
		else if (lexV == LEX_MINUS)
			res = v2 - v1;
		else if (lexV == LEX_MULT)
			res = v2 * v1;
		else if (v1 != 0)
			res = v2 / v1;
		else
			throw string("Division by zero");

		result = realToString(res);
	}
	else
		throw "Unknown identifier type in short operation" + lexV;

	ident->setValue(result);
}

void Executor::executeNot() {
	Lexeme& arg = stack.top();

	arg.setValue(lexemeToBool(arg) ? LEX_FALSE : LEX_TRUE);
}

void Executor::executeOr() {
	Lexeme arg1 = stack.top();
	stack.pop();

	Lexeme arg2 = stack.top();
	stack.pop();

	stack.push(Lexeme(LexemeT::constant_boolean, (lexemeToBool(arg1) || lexemeToBool(arg2)) ? LEX_TRUE : LEX_FALSE));
}

void Executor::executeAnd() {
	Lexeme arg1 = stack.top();
	stack.pop();

	Lexeme arg2 = stack.top();
	stack.pop();

	stack.push(Lexeme(LexemeT::constant_boolean, (lexemeToBool(arg1) && lexemeToBool(arg2)) ? LEX_TRUE : LEX_FALSE));
}

void Executor::executeAriphmetics(string& lexV) {
	Lexeme arg1 = stack.top();
	stack.pop();

	Lexeme arg2 = stack.top();
	stack.pop();

	IdentifierT t1 = getLexemeType(arg1);
	IdentifierT t2 = getLexemeType(arg2);	

	if (t2 == IdentifierT::identifier_string) {
		string& res = arg2.getValue();
		res += arg1.getValue();

		stack.push(Lexeme(LexemeT::constant_string, res));
	}
	else if (t1 == IdentifierT::identifier_real || t2 == IdentifierT::identifier_real) {
		LongDouble v1 = lexemeToReal(arg1);
		LongDouble v2 = lexemeToReal(arg2);
		LongDouble res;

		if (lexV == LEX_PLUS)
			res = v2 + v1;
		else if (lexV == LEX_MINUS)
			res = v2 - v1;
		else if (lexV == LEX_MULT)
			res = v2 * v1;
		else if (v1 != 0)
			res = v2 / v1;
		else
			throw string("Division by zero");

		stack.push(Lexeme(LexemeT::constant_real, realToString(res)));
	}
	else if (t2 == IdentifierT::identifier_int) {
		LongInt v1 = lexemeToInt(arg1);
		LongInt v2 = lexemeToInt(arg2);
		LongInt res;

		if (lexV == LEX_PLUS) {
			res = v2 + v1;
		}
		else if (lexV == LEX_MINUS) {
			res = v2 - v1;
		}
		else if (lexV == LEX_MULT) {
			res = v2 * v1;
		}
		else if (v1 != 0) {
			res = (lexV == LEX_DIV) ? (v2 / v1) : (v2 % v1);
		}
		else
			throw string("Division by zero");

		stack.push(Lexeme(LexemeT::constant_int, intToString(res)));
	}
	else
		throw string("Unknown lexeme at ariphmetics process");
}

void Executor::executeEqualities(string& lexV) {
	Lexeme arg1 = stack.top();
	stack.pop();

	Lexeme arg2 = stack.top();
	stack.pop();

	IdentifierT t1 = getLexemeType(arg1);
	IdentifierT t2 = getLexemeType(arg2);

	bool res;

	if ((t1 == IdentifierT::identifier_real || t1 == IdentifierT::identifier_int) &&
		(t2 == IdentifierT::identifier_real || t2 == IdentifierT::identifier_int)) {
		LongDouble v1 = lexemeToReal(arg1), v2 = lexemeToReal(arg2);

		res = (lexV == LEX_EQUAL) ? (v1 == v2) : (v1 != v2);
	}
	else
		res = (lexV == LEX_EQUAL) ? (arg1.getValue() == arg2.getValue()) : (arg1.getValue() != arg2.getValue());

	stack.push(Lexeme(LexemeT::constant_boolean, res ? LEX_TRUE : LEX_FALSE));
}

void Executor::executeComparsions(string& lexV) {
	Lexeme arg1 = stack.top();
	stack.pop();

	Lexeme arg2 = stack.top();
	stack.pop();

	bool res = false;

	IdentifierT t1 = getLexemeType(arg1);
	IdentifierT t2 = getLexemeType(arg2);	

	if (t1 == IdentifierT::identifier_real || t2 == IdentifierT::identifier_real) {
		LongDouble v1 = lexemeToReal(arg1);
		LongDouble v2 = lexemeToReal(arg2);

		if (lexV == LEX_LESS)
			res = v2 < v1;
		else if (lexV == LEX_LESS_EQUAL)
			res = v2 <= v1;
		else if (lexV == LEX_GREATER)
			res = v2 > v1;
		else
			res = v2 >= v1;		
	}
	else if (t1 == IdentifierT::identifier_int) {
		LongInt v1 = lexemeToInt(arg1);
		LongInt v2 = lexemeToInt(arg2);

		if (lexV == LEX_LESS)
			res = v2 < v1;
		else if (lexV == LEX_LESS_EQUAL)
			res = v2 <= v1;
		else if (lexV == LEX_GREATER)
			res = v2 > v1;
		else
			res = v2 >= v1;
	}
	else if (t1 == IdentifierT::identifier_string) {
		if (lexV == LEX_LESS)
			res = lexemeToString(arg2) < lexemeToString(arg1);
		else
			res = lexemeToString(arg2) > lexemeToString(arg1);
	}

	stack.push(Lexeme(LexemeT::constant_boolean, res ? LEX_TRUE : LEX_FALSE));
}

void Executor::executeGo(unsigned long& rpnIndex) {
	rpnIndex = stoi(stack.top().getValue()) - 1;
	stack.pop();
}

void Executor::executeFgo(unsigned long& rpnIndex) {
	LongInt i = lexemeToInt(stack.top());
	stack.pop();

	Lexeme arg2 = stack.top();
	stack.pop();

	if (!lexemeToBool(arg2))
		rpnIndex = stoi(i.toString()) - 1;
}

void Executor::executeRead() {
	Identifier* ident = identifiersTable.getByName(stack.top().getValue());
	IdentifierT identT = ident->getType();
	stack.pop();

	if (identT == IdentifierT::identifier_real) {
		long double x;
		cin >> x;
		LongDouble r(x);
		cin.ignore(); // skip '\n' character

		ident->setValue(realToString(r));
	}
	else if (identT == IdentifierT::identifier_int) {
		LongInt k;
		cin >> k;
		cin.ignore(); // skip '\n' character

		ident->setValue(intToString(k));
	}
	else if (identT == IdentifierT::identifier_string) {
		string s;
		getline(cin, s);

		ident->setValue(s);
	}
	else {
		string s;
		getline(cin, s);

		transform(s.begin(), s.end(), s.begin(), ::tolower);

		ident->setValue(s == LEX_TRUE ? LEX_TRUE : LEX_FALSE);
	}
}

void Executor::executeWrite() {
	cout << stack.top().getValue();

	stack.pop();
}

void Executor::executeAssign() {
	Lexeme arg1 = stack.top();
	stack.pop();	

	Identifier* ident = identifiersTable.getByName(stack.top().getValue());
	IdentifierT identT = ident->getType();
	stack.pop();

	if (identT == IdentifierT::identifier_int && getLexemeType(arg1) == IdentifierT::identifier_real) {
		ident->setValue(lexemeToReal(arg1).intToString());
	}
	else
		ident->setValue(arg1.getValue());
}

void Executor::printStackDump(unsigned long rpnIndex, const string& lexV) {
	std::stack<Lexeme> dump = stack;

	cout << endl << endl << ".====================================================================================================." << endl;

	printCenterCell(string("Stack (index: ") + to_string((signed long) rpnIndex) + ", current lexeme: " + lexV + ")", 100, BOLDGREEN);

	cout << "|----------------------------------------------------------------------------------------------------|" << endl;

	if (dump.empty())
		printCenterCell("stack is empty", 100, BOLDRED);
	else
		while (!dump.empty()) {
			printCenterCell(dump.top().printForRPN(false), 100, BOLDYELLOW);

			dump.pop();
		}

	cout << "'===================================================================================================='" << endl;
}

void Executor::execute(int printStack) {
	try {
		unsigned long rpnIndex = 0;
		unsigned long size = rpn.size();

		while (rpnIndex < size) {
			Lexeme& lex = rpn[rpnIndex];
			string& lexV = lex.getValue();

			switch (lex.getType()) {
				case LexemeT::constant_int:
				case LexemeT::constant_real:
				case LexemeT::constant_string:
				case LexemeT::constant_boolean:
				case LexemeT::rpn_address:
				case LexemeT::rpn_label:
					stack.push(lex);
					break;

				case LexemeT::identifier:
					executeIdentifiers(lexV);
					break;

				case LexemeT::rpn_minus:
					executeUnaryMinus();
					break;

				case LexemeT::rpn_short_op:
					executeShortOp(lexV);
					break;

				case LexemeT::rpn_increment:
					executeIncDec(1);
					break;

				case LexemeT::rpn_decrement:
					executeIncDec(-1);
					break;

				case LexemeT::keyword:
					if (lexV == LEX_NOT)
						executeNot();
					else if (lexV == LEX_OR)
						executeOr();
					else if (lexV == LEX_AND)
						executeAnd();
					else if (lexV == LEX_WRITE)
						executeWrite();
					else if (lexV == LEX_READ)
						executeRead();
					break;

				case LexemeT::delimeter:
					if (lex.isAriphmeticOp())
						executeAriphmetics(lexV);
					else if (lexV == LEX_EQUAL || lexV == LEX_NEQUAL)
						executeEqualities(lexV);
					else if (lexV == LEX_LESS || lexV == LEX_GREATER || lexV == LEX_LESS_EQUAL || lexV == LEX_GREATER_EQUAL)
						executeComparsions(lexV);
					else if (lexV == LEX_EQUALS)
						executeAssign();					
					break;

				case LexemeT::rpn_fgo:
					executeFgo(rpnIndex);					
					break;

				case LexemeT::rpn_go:
					executeGo(rpnIndex);
					break;

				default:
					break;
			}

			if (printStack) {
				printStackDump(rpnIndex, lex.printForRPN(false));
				cout << endl << identifiersTable << endl;

				if (printStack > 1) {
					cout << BOLDCYAN << "Press enter for next step" << RESET;
					getchar();
				}
			}

			rpnIndex++;
		}
	}
	catch (string e) {
		cout << BOLDRED << "Execution error: " << e << RESET << endl;
	}
}