#pragma once
#include "lexer.h"


/* ###############################
   #        PARSER               #
   ###############################
*/

class AST
{};

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
    Lexer lexer;
    Token current_token;

private:
    void error()
    {
        cout << "ERROR:: Invalid Syntax\n";
        _Exit(10);
    }

    void eat(string type)
    {
        if (current_token.type == type)
            current_token = this->lexer.get_next_token();
        else
            error();
    }

    boostvar atom()
    {
        //atom: (PLUS | MINUS) atom | INTEGER | LPAREN expr RPAREN | variable 
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
        else if (token.type == INTEGER)
        {
            eat(INTEGER);
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
        error();
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
        //term: factor ((MUL | DIV) factor)*

        boostvar node = factor();

        while (current_token.type == MUL || current_token.type == DIV)
        {
            Token token = current_token;
            if (current_token.type == MUL)
                eat(MUL);
            else
                eat(DIV);

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

    boostvar statement()
    {
        //statement: compound_statement | assignment_statement | empty
        boostvar node;

        if (current_token.type == BEGIN)
            node = compound_statement();
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
            error();

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

    boostvar program()
    {
        //program : compound_statement DOT
        boostvar node = compound_statement();
        eat(DOT);
        return node;
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
            error();
        return node;
    }
};
