#pragma once
#include "lexer.h"

/* ###############################
   #        PARSER               #
   ###############################
*/

class AST
{};

class Program : public AST
{

public:
    string name;
    boostvar block;

    Program(string name, boostvar block)
    {
        this->name = name;
        this->block = block;
    }
};

class Block : public AST
{

public:
    vector<boostvar> declarations;
    boostvar compound_statement;

    Block(vector<boostvar> declarations, boostvar compound_statement)
    {
        this->declarations = declarations;
        this->compound_statement = compound_statement;
    }
};

class Param : public AST
{

public:
    boostvar var_node;
    boostvar type_node;

    Param() {}

    Param(boostvar var_node, boostvar type_node)
    {
        this->var_node = var_node;
        this->type_node = type_node;
    }
};

class ProcedureDecl : public AST
{

public:

    string proc_name;
    boostvar block_node;
    vector<boostvar> params;

    ProcedureDecl(string proc_name, boostvar block_node) {
        this->proc_name = proc_name;
        this->block_node = block_node;
    }
};

class Print : public AST
{

public:
    boostvar output_variable;

    Print(boostvar output_variable)
    {
        this->output_variable = output_variable;
    }
};

class VarDecl : public AST
{

public:
    boostvar var_node;
    boostvar type_node;

    VarDecl(boostvar var_node, boostvar type_node)
    {
        this->var_node = var_node;
        this->type_node = type_node;
    }
};


class Type : public AST
{

public:
    Token token;
    string value;

    Type() {}

    Type(Token token)
    {
        this->token = token;
        this->value = token.value;
    }
};

class Compound : public AST
{

public:
    vector<boostvar> children;
};

class Assign : public AST
{

public:
    boostvar left;
    Token token, op;
    boostvar right;

    Assign(boostvar left, Token op, boostvar right)
    {
        this->left = left;
        this->token = op;
        this->op = op;
        this->right = right;
    }
};

class Var : public AST
{

public:
    Token token;
    string value;

    Var(Token token)
    {
        this->token = token;
        this->value = token.value;
    }
};

class NoOp : public AST
{

};

class BinOp : public AST
{

public:
    Token token, op;
    boostvar left, right;

    BinOp(boostvar left, Token op, boostvar right)
    {
        this->left = left;
        this->token = op;
        this->op = op;
        this->right = right;
    }
};

class Num : public AST
{

public:
    Token token;
    string value;

    Num(Token token)
    {
        this->token = token;
        this->value = token.value;
    }
};

class UnaryOp : public AST
{

public:
    Token token, op;
    boostvar expr;

    UnaryOp(Token op, boostvar expr)
    {
        this->token = op;
        this->op = op;
        this->expr = expr;
    }
};

class Parser
{

public: 
    Lexer lexer;
    Token current_token;

private:
    void error(string error_code, Token token, string error_message = "")
    {
        cout << "ERROR:: "<<error_code << "->" << token <<" line:: "<< lexer.lineno <<":"<<lexer.column << error_message << endl;
        _Exit(10);
    }

    void eat(string type)
    {
        if (current_token.type == type)
            current_token = this->lexer.get_next_token();
        else
        {
            string error_message = "\nExpected type: " + type;
            error("UNEXPECTED_TOKEN", current_token, error_message);
        }
    }

    boostvar atom()
    {
        //atom: (PLUS | MINUS) atom | INTEGER_CONST | REAL_CONST | LPAREN expr RPAREN | variable 
        Token token = current_token;
        if (token.type == PLUS)
        {
            eat(PLUS);
            return new UnaryOp(token, atom());
        }
        else if (token.type == MINUS)
        {
            eat(MINUS);
            return new UnaryOp(token, atom());
        }
        else if (token.type == INTEGER_CONST)
        {
            eat(INTEGER_CONST);
            return new Num(token);
        }
        else if (token.type == REAL_CONST)
        {
            eat(REAL_CONST);
            return new Num(token);
        }
        else if (token.type == LPAREN) {
            eat(LPAREN);
            boostvar node = expr();
            eat(RPAREN);
            return node;
        }
        else if (token.type == ID)
        {
            boostvar node = variable();
            return node;
        }
        error("UNEXPECTED_TOKEN", current_token);
    }

    boostvar factor()
    {
        //factor: atom (POW factor)*
        boostvar node = atom();

        while (current_token.type == POW)
        {
            Token token = current_token;
            eat(POW);
            node = new BinOp(node, token, factor());
        }

        return node;
    }

    boostvar term()
    {
        //term: factor factor ((MUL | INTEGER_DIV | FLOAT_DIV) factor)

        boostvar node = factor();

        while (current_token.type == MUL || current_token.type == INTEGER_DIV || current_token.type == FLOAT_DIV)
        {
            Token token = current_token;
            if (current_token.type == MUL)
                eat(MUL);
            else if (current_token.type == INTEGER_DIV)
                eat(INTEGER_DIV);
            else
                eat(FLOAT_DIV);

            node = new BinOp(node, token, factor());
        }

        return node;
    }

    boostvar expr()
    {
        //expr: term ((PLUS | MINUS) term)*
        boostvar node = term();

        while (current_token.type == PLUS || current_token.type == MINUS)
        {
            Token token = current_token;
            if (current_token.type == PLUS)
                eat(PLUS);
            else
                eat(MINUS);

            node = new BinOp(node, token, term());
        }

        return node;
    }

    boostvar empty() {
        //An empty production
        return new NoOp();
    }

    boostvar variable() {
        //variable: ID
        boostvar node = new Var(current_token);
        eat(ID);
        return node;
    }

    boostvar assignment_statement()
    {
        //assignment_statement: variable ASSIGN expr
        boostvar left = variable();
        Token token = current_token;
        eat(ASSIGN);
        boostvar right = expr();
        boostvar node = new Assign(left, token, right);
        return node;
    }

    boostvar print_statement()
    {
        //print_statement: PRINT expr
        Token token = current_token;
        eat(PRINT);
        boostvar output_variable = expr();
        boostvar node = new Print(output_variable);
        return node;
    }

    boostvar statement()
    {
        //statement: compound_statement | assignment_statement | print_statement | empty
        boostvar node;

        if (current_token.type == BEGIN)
            node = compound_statement();
        else if (current_token.type == PRINT)
            node = print_statement();
        else if (current_token.type == ID)
            node = assignment_statement();
        else
            node = empty();

        return node;
    }

    vector<boostvar> statement_list()
    {
        //statement_list: statement | statement SEMI statement_list

        boostvar node = statement();

        vector<boostvar> results;
        results.push_back(node);

        while (current_token.type == SEMI)
        {
            eat(SEMI);
            results.push_back(statement());
        }

        if (current_token.type == ID)
            error("UNEXPECTED_TOKEN", current_token);

        return results;
    }

    boostvar compound_statement()
    {
        //compound_statement: BEGIN statement_list END
        eat(BEGIN);
        vector<boostvar> nodes = statement_list();
        eat(END);

        Compound* root = new Compound();
        for (auto node : nodes)
            root->children.push_back(node);
        return root;
    }

    boostvar type_spec()
    {
        //type_spec : INTEGER | REAL
        Token token = current_token;
        if (current_token.type == INTEGER)
            eat(INTEGER);
        else
            eat(REAL);

        boostvar node = new Type(token);
        return node;
    }

    vector<boostvar> variable_declarations()
    {
        //variable_declaration : ID (COMMA ID)* COLON type_spec
        vector<boostvar> var_nodes;

        //storing the first ID
        var_nodes.push_back(new Var(current_token));
        eat(ID);

        while (current_token.type == COMMA)
        {
            eat(COMMA);
            var_nodes.push_back(new Var(current_token));
            eat(ID);
        }

        eat(COLON);

        boostvar type_node = type_spec();
        vector<boostvar> var_declarations;

        for (auto var_node : var_nodes)
            var_declarations.push_back(new VarDecl(var_node, type_node));

        return var_declarations;
    }

    vector<boostvar> declarations()
    {
    //declarations: (VAR(variable_declaration SEMI) + )*| (PROCEDURE ID(LPAREN formal_parameter_list RPAREN) ? SEMI block SEMI) *| empty
        vector<boostvar> declarations;
        if (current_token.type == VAR)
        {
            eat(VAR);
            while (current_token.type == ID)
            {
                vector<boostvar> var_decls = variable_declarations();
                for (auto var_decl : var_decls)
                    declarations.push_back(var_decl);
                eat(SEMI);
            }
        }

        while (current_token.type == PROCEDURE)
        {
            eat(PROCEDURE);
            string proc_name = current_token.value;
            eat(ID);
            eat(SEMI);
            boostvar block_node = block();
            boostvar proc_decl = new ProcedureDecl(proc_name, block_node);
            declarations.push_back(proc_decl);
            eat(SEMI);
            //need to define params
        }

        return declarations;
    }

    void formal_parameter_list()
    {
        //formal_parameter_list : formal_parameters| formal_parameters SEMI formal_parameter_list
    }

    void formal_parameter()
    {
        //formal_parameters : ID (COMMA ID)* COLON type_spec """
        vector<boostvar> param_nodes;
    }

    boostvar block()
    {
        //block : declarations compound_statement
        vector<boostvar> declaration_nodes = declarations();
        boostvar compound_statement_node = compound_statement();
        boostvar node = new Block(declaration_nodes, compound_statement_node);
        return node;
    }

    boostvar program()
    {
        //program : PROGRAM variable SEMI block DOT
        eat(PROGRAM);
        Var* var_node = boost::get<Var*>(variable());
        string program_name = var_node->value;
        eat(SEMI);
        boostvar block_node = block();
        boostvar program_node = new Program(program_name, block_node);
        eat(DOT);
        return program_node;
    }


public:
    Parser() {}

    Parser(Lexer lexer)
    {
        this->lexer = lexer;
        this->current_token = this->lexer.get_next_token();
    }

    boostvar parse()
    {
        boostvar node = program();
        if (current_token.type != EOL)
            error("UNEXPECTED_TOKEN", current_token);
        return node;
    }
};