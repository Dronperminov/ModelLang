#include "identifiersTable.h"

size_t IdentifiersTable::size() const {
	return identifiers.size();
}

Identifier* IdentifiersTable::getByName(const std::string& identName) {
	auto result = find_if(identifiers.begin(), identifiers.end(), [&identName] (Identifier& ident) { return ident.getName() == identName; });

	if (result != identifiers.end())
		return &(*result);
	else
		return nullptr;
}

void IdentifiersTable::addByName(const std::string& identName) {
	if (getByName(identName) == nullptr)
		identifiers.push_back(Identifier(IdentifierT::identifier_unknown, identName));
}

std::ostream &operator<<(std::ostream &stream, const IdentifiersTable &identifiersTable) {
	stream << ".====================================================================================================." << std::endl;
	stream << "|                                         " << BOLDGREEN << "Identifiers table" << RESET << "                                          |" << std::endl;
	stream << "|====================================================================================================|" << std::endl;
	stream << "|              Name              |    Type    | Declared |                   Value                   |" << std::endl;
	stream << "|--------------------------------+------------+----------+-------------------------------------------|" << std::endl;

	for (auto i = identifiersTable.identifiers.begin(); i < identifiersTable.identifiers.end(); i++) {
		Identifier ident = *i;

		stream << "|";
		printCenter(stream, ident.getName(), 32);
		stream << "|";
		printCenter(stream, ident.printType(), 12);
		stream << "|";
		printCenter(stream, ident.getDeclared() ? "yes" : "no", 10);
		stream << "|";
		printCenter(stream, ident.getValue(), 43);
		stream << "|" << std::endl;
	}

	stream << "'===================================================================================================='" << std::endl;

	return stream;
}