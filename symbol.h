#pragma once
#include "parser.h"
#include <boost/functional/hash.hpp>

class Symbol
{

public:
	string name;
	Type type;

	Symbol() {}

	Symbol(string name)
	{
		this->name = name;
	}

	Symbol(string name, Type type)
	{
		this->name = name;
		this->type = type;
	}
};

class BuiltinTypeSymbol : public Symbol
{

public:
	BuiltinTypeSymbol(string name)
	{
		Symbol symbol(name);
	}
};

//To print the token in the correct format
ostream& operator<<(ostream& strm, const BuiltinTypeSymbol& builtInSymbol) {
	return strm << builtInSymbol.name;
}


class VarSymbol : public Symbol
{

public:
	VarSymbol(string name, Type type)
	{
		Symbol symbol(name, type);
	}
};

ostream& operator<<(ostream& strm, const VarSymbol& varSymbol) {
	return strm << "<{" << varSymbol.name << " : " << varSymbol.type.value << "}>";
}

class SymbolTable
{
	
public:
	unordered_map<string, boostvar> symbols;

	void error(string name)
	{
		cout << name << " does not exist.";
		_Exit(10);
	}

	void define(boostvar symbol)
	{
		cout << "Define: " << symbol;
		if (symbol.which() == 11)
			symbols[boost::get<BuiltinTypeSymbol*>(symbol)->name] = symbol;
		else if (symbol.which() == 12)
			symbols[boost::get<VarSymbol*>(symbol)->name] = symbol;
	}

	boostvar lookup(string name)
	{
		cout << "Lookup: " << name;
		if (symbols.find(name) != symbols.end())
		{
			boostvar symbol = symbols[name];
			return symbol;
		}
		error(name);
	}
};

ostream& operator<<(ostream& strm, const SymbolTable& symbolTable) {
	string information;
	for (auto value : symbolTable.symbols)
		information += " " + value.first;
	return strm << "Symbols: {" + information;
}
