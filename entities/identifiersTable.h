#ifndef IDENTIFIERS_TABLE_H
#define IDENTIFIERS_TABLE_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "functions.h"
#include "identifier.h"

class IdentifiersTable {
public:
	std::vector<Identifier> identifiers;

	size_t size() const;

	void addByName(const std::string& identName);
	Identifier* getByName(const std::string& identName);

	friend std::ostream &operator<<(std::ostream &stream, const IdentifiersTable &identifiersTable);
};

#endif