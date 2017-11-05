#include "lexicalAnalyzer.h"

using namespace std;

LexicalAnalyzer::LexicalAnalyzer(ifstream& sourceFile) {
	if (!sourceFile.good())
		throw string("Error: incorrect file");

	string sourceText {
		istreambuf_iterator<char>(sourceFile), istreambuf_iterator<char>()
	};

	checkSource(sourceText);

	this->progText = sourceText;
	this->charPosition = 0;
	this->lineNumber = 0;
}

LexicalAnalyzer::LexicalAnalyzer(string &sourceText) {
	checkSource(sourceText);

	this->progText = sourceText;
	this->charPosition = 0;
	this->lineNumber = 0;
}

void LexicalAnalyzer::checkSource(string& source) {
	unsigned long line = 1;
	string s;

	size_t i = 0;

	while (i < source.length()) {
		if (source[i] == '\n') {
			lines.push_back(s);
			s.clear();

			line++;
		}
		else if (source[i] == '\t')
			s += "    ";
		else {
			s += source[i];

			if (!source[i]) {
				source.erase(i--, 1);
				cout << BOLDYELLOW << "Warning: surpressed zero byte in text at line " << line << " (was deleted for interpreting)" << RESET << endl;
			}
		}

		i++;
	}

	lines.push_back(s);
}

string LexicalAnalyzer::currChar() {
	char c = progText[charPosition];

	string s;
	s += c;

	if (c == -48 || c == -47)
		s += progText[charPosition + 1];

	return s;
}

string LexicalAnalyzer::nextChar(bool shift) {
	size_t len = currChar().length();
	charPosition += len;

	string s = currChar();

	if (!shift)
		charPosition -= len;
	else if (s == "\n")
		lineNumber++;

	return s;
}

bool LexicalAnalyzer::isLetter(char symbol) const {
	return (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z') || symbol == '_' || (symbol == -47 || symbol == -48);
}

bool LexicalAnalyzer::isDigit(char symbol) const {
	return symbol >= '0' && symbol <= '9';
}

bool LexicalAnalyzer::isLetterOrDigit(char symbol) const {
	return isLetter(symbol) || isDigit(symbol);
}

bool LexicalAnalyzer::isAsDelimeter(char symbol) const {
	bool isSame = false;
	auto i = delimeters.begin();

	while (!isSame && i < delimeters.end()) {
		if ((*i)[0] == symbol)
			isSame = true;

		i++;
	}

	return isSame;
}

bool LexicalAnalyzer::isKeyword(string &word) const{
	return find(keywords.begin(), keywords.end(), word) < keywords.end();
}

bool LexicalAnalyzer::isDelimeter(string &word) const {
	return find(delimeters.begin(), delimeters.end(), word) < delimeters.end();
}

Lexeme LexicalAnalyzer::readFromLetter() {
	string c = currChar();
	string word;
	unsigned long line = lineNumber;

	while (c[0] && isLetterOrDigit(c[0])) {
		word += c;
		c = nextChar();
	}

	// downcase
	transform(word.begin(), word.end(), word.begin(), ::tolower);

	if (word == LEX_TRUE || word == LEX_FALSE)
		return Lexeme(LexemeT::constant_boolean, word, line);

	if (word == LEX_INT)
		return Lexeme(LexemeT::datatype_int, word, line);

	if (word == LEX_REAL)
		return Lexeme(LexemeT::datatype_real, word, line);

	if(word == LEX_STRING)
		return Lexeme(LexemeT::datatype_string, word, line);

	if (word == LEX_BOOLEAN)
		return Lexeme(LexemeT::datatype_boolean, word, line);

	if (isKeyword(word))
		return Lexeme(LexemeT::keyword, word, line);
	else {
		identifiersTable.addByName(word);

		return Lexeme(LexemeT::identifier, word, line);
	}
}

Lexeme LexicalAnalyzer::readFromDigit() {
	string c = currChar();
	string number;
	unsigned long line = lineNumber;

	while (c[0] && (isDigit(c[0]) || c == LEX_POINT)) {
		number += c;
		c = nextChar();
	}

	return Lexeme(number.find(LEX_POINT) == string::npos ? LexemeT::constant_int : LexemeT::constant_real, number, line);
}

Lexeme LexicalAnalyzer::readFromQuote() {
	string c = nextChar();
	string s;
	unsigned long line = lineNumber;

	while (c[0] && c != LEX_QUOTE) {
		s += c;

		if (c[0] == '\\') {
			c = nextChar();

			if (c[0])
				s += c;
		}

		c = nextChar();
	}

	if (!c[0])
		return Lexeme(LexemeT::unknown, "surprised end of file", line);

	nextChar();

	s.erase(remove(s.begin(), s.end(), '\n'), s.end());

	string s2;
	unsigned long i = 0, len = s.length();
	bool isOk = true;
	char c1;

	while (i < len && isOk) {
		c1 = s[i];

		if (c1 == '\\') {
			c1 = s[++i];

			switch (c1) {
				case 'n':
					s2 += '\n';
					break;

				case 't':
					s2 += '\t';
					break;

				case '\\':
				case '"':
					s2 += c1;
					break;

				default:
					isOk = false;
					break;
			}
		} else
			s2 += c1;

		i++;
	}

	return isOk ? Lexeme(LexemeT::constant_string, s2, line) : Lexeme(LexemeT::unknown, string("unknown screened character '\\") + c1 + "'", line);
	
}

Lexeme LexicalAnalyzer::readFromSlash() {
	string c = nextChar();
	unsigned long line = lineNumber;

	if (c == LEX_DIV || c == LEX_MULT) {
		string comment;

		if (c == LEX_DIV) {
			c = nextChar();

			while (c[0] && c[0] != '\n') {
				comment += c; 
				c = nextChar();
			}
		}
		else {
			c = nextChar();

			while (c[0] && c + nextChar(false) != LEX_ML_COMMENT_END) {
				comment += c;
				c = nextChar();
			}

			if (!c[0])
				return Lexeme(LexemeT::unknown, "surprised end of file", line);

			nextChar();
			nextChar();
		}

		return Lexeme(LexemeT::comment, comment, line);
	}
	else if (c == LEX_EQUALS) {
		nextChar();
		return Lexeme(LexemeT::delimeter, LEX_DIV_VALUE, line);
	}
	else
		return Lexeme(LexemeT::delimeter, LEX_DIV, line);
}

Lexeme LexicalAnalyzer::readDelimeter() {
	unsigned long line = lineNumber;
	string curr = currChar();
	string next = nextChar();

	string s = curr + next;
	string c = curr;

	if (isDelimeter(s)) {
		nextChar();
		
		return Lexeme(LexemeT::delimeter, s, line);	
	}
	
	if (isDelimeter(c))
		return Lexeme(LexemeT::delimeter, c, line);
	
	return Lexeme(LexemeT::unknown, string("unknown character '") + c + string("'"), line);
}

Lexeme LexicalAnalyzer::getLexem() {
	string c = currChar();

	while (c[0] && (c[0] == ' ' || c[0] == '\n' || c[0] == '\t'))
		c = nextChar();

	if (!c[0])
		return Lexeme(LexemeT::null, "null", lineNumber);

	if (isLetter(c[0]))
		return readFromLetter();

	if (isDigit(c[0]))
		return readFromDigit();

	if (c == LEX_QUOTE)
		return readFromQuote();

	if (c == LEX_DIV)
		return readFromSlash();

	if (isAsDelimeter(c[0]))
		return readDelimeter();

	unsigned long line = lineNumber;
	nextChar();

	return Lexeme(LexemeT::unknown, string("unknown character '") + c + '\'', line);	
}

vector<Lexeme>& LexicalAnalyzer::analyze() {
	Lexeme lexeme = Lexeme(LexemeT::null, "null", lineNumber);

	do {
		lexeme = getLexem();

		if (lexeme.getType() != LexemeT::comment)
			lexemes.push_back(lexeme);	

	} while (lexeme.getType() != LexemeT::null);

	return lexemes;
}

IdentifiersTable& LexicalAnalyzer::getIdentifiersTable() {
	return identifiersTable;
}

vector<string>& LexicalAnalyzer::getTextLines() {
	return lines;
}

void LexicalAnalyzer::printText() const {
	cout << BOLDMAGENTA << "Program text: " << RESET << endl;
	cout << BLUE;

	for (size_t i = 0; i < lines.size(); i++)
		cout << lines[i] << "\n";

	cout << RESET;
}

void LexicalAnalyzer::printLexemes() const {
	cout << endl;
	cout << BOLDMAGENTA << "Founded lexemes (" << (lexemes.size() - 1) << "):" << RESET << endl;

	cout << ".===============================================================." << endl;
	cout << "|             Value              |       Type       |   Line    |" << endl;
	cout << "|--------------------------------+------------------+-----------|" << endl;

	for (auto i = lexemes.begin(); i < lexemes.end(); i++)
		cout << *i << endl;

	cout << "'==============================================================='" << endl;
}

void LexicalAnalyzer::printIdentifiers() const {
	if (identifiersTable.size()) {
		cout << BOLDMAGENTA << endl << "Founded identifiers after lexical analyze (" << identifiersTable.size() << "):" << RESET << endl;

		cout << identifiersTable;
	}
	else
		cout << BOLDMAGENTA << endl << "Identifiers table after lexical analyze is empty" << RESET << endl;
}