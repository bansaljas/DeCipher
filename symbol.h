#pragma once
#include "parser.h"
#include <boost/functional/hash.hpp>

class Symbol
{

public:
	string name;
	boostvar type;

	Symbol() {}

	Symbol(string name)
	{
		this->name = name;
	}

	Symbol(string name, boostvar type)
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
	VarSymbol(string name, boostvar type)
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

class SymbolTableBuilder {

public:
    SymbolTable symtab;

    void error()
    {
        cout << "ERROR:: No such parsing method present\n";
        _Exit(10);
    }

    void visit(boostvar node)
    {
        if (node.which() == 0)
            visit_BinOp(boost::get<BinOp*>(node));
        else if (node.which() == 1)
            visit_Num(boost::get<Num*>(node));
        else if (node.which() == 2)
            visit_UnaryOp(boost::get<UnaryOp*>(node));
        else if (node.which() == 3)
            visit_Compound(boost::get<Compound*>(node));
        else if (node.which() == 4)
            visit_Assign(boost::get<Assign*>(node));
        else if (node.which() == 5)
            visit_Var(boost::get<Var*>(node));
        else if (node.which() == 6)
            visit_NoOp(boost::get<NoOp*>(node));
        else if (node.which() == 7)
            visit_Program(boost::get<Program*>(node));
        else if (node.which() == 8)
            visit_Block(boost::get<Block*>(node));
        else if (node.which() == 9)
            visit_VarDecl(boost::get<VarDecl*>(node));
        else
            error();
    }

    void visit_BinOp(BinOp* node)
    {
        visit(node->left);
        visit(node->right);
    }

    void visit_Num(Num* node)
    {
        return;
    }

    void visit_UnaryOp(UnaryOp* node)
    {
        visit(node->expr);
    }

    void visit_Compound(Compound* node)
    {
        for (auto child : node->children)
            visit(child);
    }

    void visit_NoOp(NoOp* node)
    {
        return;
    }

    void visit_VarDecl(VarDecl* node)
    {
        Type* type_node = boost::get<Type*>(node->type_node);
        string type_name = type_node->value;
        boostvar type_symbol = symtab.lookup(type_name);
        Var* var_node = boost::get<Var*>(node->var_node);
        string var_name = var_node->value;
        boostvar var_symbol = new VarSymbol(var_name, type_symbol);
        symtab.define(var_symbol);
    }

    void visit_Block(Block* node)
    {
        for (auto declaration : node->declarations)
            visit(declaration);
        visit(node->compound_statement);
    }

    void visit_Program(Program* node)
    {
        visit(node->block);
    }
};
