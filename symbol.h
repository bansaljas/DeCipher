#pragma once
#include "parser.h"

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
        this->name = name;
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
        this->name = name;
        this->type = type;
	}
};

ostream& operator<<(ostream& strm, const VarSymbol& varSymbol) {
    Type* type = boost::get<Type *>(varSymbol.type);
	return strm << "<{" << varSymbol.name << " : " << type->value << "}>";
}

class ProcedureSymbol : public Symbol
{
public:
    string name;
    vector<boostvar> params;
    ProcedureSymbol(string name, vector<boostvar>params) 
    {
        Symbol symbol(name);
        this->params = params;
    }
};

class ScopedSymbolTable
{

public:
    unordered_map<string, boostvar> symbols;
    string scope_name;
    int scope_level;

    //ScopedSymbolTable() {}

    ScopedSymbolTable(string scope_name, int scope_level) {
        this->scope_name = scope_name;
        this->scope_level = scope_level;
        _init_builtins();
    }

    void _init_builtins()
    {
        insert(new BuiltinTypeSymbol("INTEGER"));
        insert(new BuiltinTypeSymbol("REAL"));
    }

    void error(string name)
    {
        cout << name << " does not exist.";
        _Exit(10);
    }

    void insert(boostvar symbol)
    {
        if (symbol.which() == 11)
            symbols[boost::get<BuiltinTypeSymbol*>(symbol)->name] = symbol;
        else if (symbol.which() == 12)
            symbols[boost::get<VarSymbol*>(symbol)->name] = symbol;
        else if (symbol.which() == 15)
            symbols[boost::get<ProcedureSymbol*>(symbol)->name] = symbol;
    }

    boostvar lookup(string name, bool current_scope_only = false);
}; 

ostream& operator<<(ostream& strm, const ScopedSymbolTable& symbolTable) {
	string information;
	for (auto value : symbolTable.symbols)
		information += " " + value.first;
	return strm << "Symbols: {" + information;
    //can add printing of scoped symbol table
}

stack<ScopedSymbolTable> enclosed_scopes;

boostvar ScopedSymbolTable :: lookup(string name, bool current_scope_only)
{
    cout << "Lookup: " << name;

    if (symbols.find(name) != symbols.end())
    {
        boostvar symbol = symbols[name];
        return symbol;
    }

    if (!current_scope_only && enclosed_scopes.size() > 1) {
        ScopedSymbolTable enclosed_scope = enclosed_scopes.top();
        enclosed_scopes.pop();
        boostvar symbol = enclosed_scope.lookup(name);
        enclosed_scopes.push(enclosed_scope);
        return symbol;
    }

    error(name);
}

class SemanticAnalyzer {

public:
    ScopedSymbolTable current_scope = ScopedSymbolTable("", 0);

    void error()
    {
        cout << "ERROR:: No such symbol method exists\n";
        _Exit(10);
    }

    void visit(boostvar node)
    {
        //need to add ProcedureDecl-13 condition too
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

    void visit_Assign(Assign* node) {
        visit(node->left);
        visit(node->right);
    }

    void visit_Var(Var* node) {
        string var_name = node->value;
        boostvar var_symbol = current_scope.lookup(var_name);
    }

    void visit_ProcedureDecl(ProcedureDecl* node) {
        string proc_name = node->proc_name;
        ProcedureSymbol* proc_symbol = new ProcedureSymbol(proc_name, vector<boostvar>());
        this->current_scope.insert(proc_symbol);
        enclosed_scopes.push(current_scope);
        cout << "Enter scope: " << proc_name << endl;

        ScopedSymbolTable procedure_scope = ScopedSymbolTable(proc_name, this->current_scope.scope_level + 1);
        this->current_scope = procedure_scope;

        for (auto param : node->params) {
            Param* param_node = boost::get<Param*>(param);
            Type* type_node = boost::get<Type*>(param_node->type_node);
            boostvar param_type = this->current_scope.lookup(type_node->value);
            Var* var_node = boost::get<Var*>(param_node->var_node);
            string param_name = var_node->value;
            VarSymbol* var_symbol = new VarSymbol(param_name, param_type);
            this->current_scope.insert(var_symbol);//needs to be sorted
            proc_symbol->params.push_back(var_symbol);
        }
        visit(node->block_node);
        cout << "procedure_scope" << endl;
        this->current_scope = enclosed_scopes.top();
        enclosed_scopes.pop();
        cout << "Leave scope : " << proc_name << endl;
    }

    void visit_VarDecl(VarDecl* node)
    {
        Type* type_node = boost::get<Type*>(node->type_node);
        string type_name = type_node->value;
        boostvar type_symbol = current_scope.lookup(type_name);
        Var* var_node = boost::get<Var*>(node->var_node);
        string var_name = var_node->value;
        if (current_scope.symbols.find(var_name) != current_scope.symbols.end()) {
            cout << "Duplicate identifier found " << var_name;
            _Exit(10);
        }
        boostvar var_symbol = new VarSymbol(var_name, type_symbol);
        current_scope.insert(var_symbol);
    }

    void visit_Block(Block* node)
    {
        for (auto declaration : node->declarations)
            visit(declaration);
        visit(node->compound_statement);
    }

    void visit_Program(Program* node)
    {
        cout << "Enter Scope : GLOBAL" << endl;
        ScopedSymbolTable global_scope = ScopedSymbolTable("GLOBAL", 1);
        enclosed_scopes.push(current_scope);
        this->current_scope = global_scope;
        visit(node->block);
        cout << "global_scope" << endl;
        this->current_scope = enclosed_scopes.top();
        enclosed_scopes.pop();
        cout << "Leave scope : GLOBAL" << endl;
    }
};
