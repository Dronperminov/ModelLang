#include "functions.h"

int realLength(std::string s) {
	size_t index = 0;
	int len = 0;

	while (index < s.length()) {
		if (s[0] != -48 && s[0] != -47)
			len++;

		index++;
	}

	return len;
}

void printCenter(std::string msg, int count, const std::string& color) {
	int len = realLength(msg);

	for (int j = 0; j < (count - len) / 2; j++)
		std::cout << ' ';

	std::cout << color << msg << RESET;

	for (int j = (count - len) / 2 + len; j < count; j++)
		std::cout << ' ';
}

void printCenter(std::ostream &stream, std::string msg, int count, const std::string& color) {
	int len = realLength(msg);

	for (int j = 0; j < (count - len) / 2; j++)
		stream << ' ';

	stream << color << msg << RESET;

	for (int j = (count - len) / 2 + len; j < count; j++)
		stream << ' ';
}

void printCenterCell(std::string msg, int count, const char* color) {
	std::cout << "|";
	printCenter(msg, count, color);
	std::cout << "|" << std::endl;
}