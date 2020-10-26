#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <boost/variant.hpp>
using namespace std;

class BinOp;
class Num;
class UnaryOp;
class Compound;
class Assign;
class Var;
class NoOp;
class Program;
class Block;
class VarDecl;
class Type;

#define boostvar boost::variant<BinOp*, Num*, UnaryOp*, Compound*, Assign*, Var*, NoOp*, Program*, Block*, VarDecl*, Type*>

unordered_map<string, int> GLOBAL_SCOPE;

/* ###############################
   #        LEXER                #
   ###############################
*/

string INTEGER = "INTEGER", PLUS = "PLUS", MINUS = "MINUS", MUL = "MUL", DIV = "DIV", EOL = "EOL", LPAREN = "(", RPAREN = ")", POW = "POW", BEGIN = "BEGIN", END = "END", DOT = "DOT",
ID = "ID", ASSIGN = "ASSIGN", SEMI = "SEMI", PROGRAM = "PROGRAM", VAR = "VAR", COLON = "COLON",
COMMA = "COMMA", REAL = "REAL", INTEGER_CONST = "INTEGER_CONST", REAL_CONST = "REAL_CONST",
INTEGER_DIV = "INTEGER_DIV", FLOAT_DIV = "FLOAT_DIV";

class Token
{
public:

    string value; //will store the value in string
    string type; //will store the word

    Token()
    {
        this->value = "";
        this->type = "";
    }

    Token(string type, string value)
    {
        this->value = value;
        this->type = type;
    }

};

//To print the token in the correct format
ostream& operator<<(ostream& strm, const Token& token) {
    return strm << "(" << token.type << ", " << token.value << ")";
}

unordered_map<string, class Token> RESERVED_KEYWORDS;


class Lexer
{
    string text;
    int pos;
    char current_char;

public:
    Lexer()
    {
        this->pos = 0;
        this->current_char = '\0';
    }

    Lexer(string text)
    {
        this->text = text;
        this->pos = 0;
        this->current_char = text[pos];
        add_keywords();
    }

    void add_keywords()
    {
        RESERVED_KEYWORDS[PROGRAM] = Token("PROGRAM", PROGRAM);
        RESERVED_KEYWORDS[VAR] = Token("VAR", VAR);
        RESERVED_KEYWORDS[DIV] = Token("INTEGER_DIV", DIV);
        RESERVED_KEYWORDS[INTEGER] = Token("INTEGER", INTEGER);
        RESERVED_KEYWORDS[REAL] = Token("REAL", REAL);
        RESERVED_KEYWORDS[BEGIN] = Token("BEGIN", BEGIN);
        RESERVED_KEYWORDS[END] = Token("END", END);
    }

    void error()
    {
        cout << "ERROR:: Error parsing the input\n";
        _Exit(10);
    }

    Token id()
    {
        string result;
        while (current_char != '\0' && isalnum(current_char))
        {
            result += current_char;
            advance();
        }

        if (RESERVED_KEYWORDS.find(result) == RESERVED_KEYWORDS.end())
            return RESERVED_KEYWORDS[result] = Token(ID, result);
        else
            return RESERVED_KEYWORDS[result];
    }

    char peek()
    {
        int peek_pos = pos + 1;
        if (peek_pos > text.size() - 1)
            return '\0';
        else
            return text[peek_pos];
    }

    void advance()
    {
        pos++;
        if (pos > text.size() - 1)
            current_char = '\0';
        else
            current_char = text[pos];
    }

    void skip_whitspace()
    {
        while (current_char != '\0' && current_char == ' ')
            advance();
    }

    void skip_comment()
    {
        while (current_char != '}')
            advance();
        advance(); //for the closing bracket
    }

    Token number()
    {
        Token token;
         string result = "";
         while (current_char != '\0' && current_char >= 48 && current_char <= 57)
         {
             result += current_char;
             advance();
         }

         if (current_char == '.')
         {
             result += current_char;
             advance();

             while (current_char != '\0' && current_char >= 48 && current_char <= 57)
             {
                 result += current_char;
                 advance();
             }

             token = Token("REAL_CONST", result);
         }
         else
             token = Token("INTEGER_CONST", result);

         return token;
    }

    /* ******LEXICAL ANALYSER****** */
    Token get_next_token()
    {
        while (current_char == ' ')
            skip_whitspace();

        if (current_char == '\0')
            return Token(EOL, "\0");


        string current_char_str;
        current_char_str = current_char;

        if (current_char == '{')
        {
            advance();
            skip_comment();
            get_next_token();
        }
        
        //Means we have a digit
        if (current_char >= 48 && current_char <= 57)
        {
            return number();
        }

        if (current_char == '+')
        {
            Token token(PLUS, current_char_str);
            advance();
            return token;
        }

        if (current_char == '-')
        {
            Token token(MINUS, current_char_str);
            advance();
            return token;
        }

        if (current_char == '*')
        {
            Token token(MUL, current_char_str);
            advance();
            return token;
        }

        if (current_char == '/')
        {
            Token token(DIV, current_char_str);
            advance();
            return token;
        }

        if (current_char == '(')
        {
            Token token(LPAREN, current_char_str);
            advance();
            return token;
        }

        if (current_char == ')')
        {
            Token token(RPAREN, current_char_str);
            advance();
            return token;
        }

        if (current_char == '^')
        {
            Token token(POW, current_char_str);
            advance();
            return token;
        }

        if (isalpha(current_char))
        {
            return id();
        }

        if (current_char == ':' && peek() == '=')
        {
            Token token(ASSIGN, ":=");
            advance();
            advance();
            return token;
        }

        if (current_char == ';')
        {
            Token token(SEMI, ";");
            advance();
            return token;
        }

        if (current_char == '.')
        {
            Token token(DOT, ".");
            advance();
            return token;
        }

        if (current_char == ':')
        {
            Token token(COLON, ":");
            advance();
            return token;
        }

        if (current_char == ',')
        {
            Token token(COMMA, ",");
            advance();
            return token;
        }

        if (current_char == '/')
        {
            Token token(FLOAT_DIV, "/");
            advance();
            return token;
        }

        //If it isnt a digit or + or -, then some other char, hence show error
        error();
    }
};


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

    boostvar variable(){
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
        //declarations: VAR(variable_declaration SEMI) + | empty
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
        if (declarations.size() == 0)
            error();
        return declarations;
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
            error();
        return node;
    }
};


/* ###############################
   #       INTERPRETER           #
   ###############################
*/


class Interpreter
{
    Parser parser;

public:
    Interpreter(Parser parser)
    {
        this->parser = parser;
    }

    void error()
    {
        cout << "ERROR:: No such parsing method present\n";
        _Exit(10);
    }

    int visit(boostvar node)
    {
        if (node.which() == 0)
            return visit_BinOp(boost::get<BinOp*>(node));
        else if (node.which() == 1)
            return visit_Num(boost::get<Num*>(node));
        else if (node.which() == 2)
            return visit_UnaryOp(boost::get<UnaryOp*>(node));
        else if (node.which() == 3)
            return visit_Compound(boost::get<Compound*>(node));
        else if (node.which() == 4)
            return visit_Assign(boost::get<Assign*>(node));
        else if (node.which() == 5)
            return visit_Var(boost::get<Var*>(node));
        else if (node.which() == 6)
            return visit_NoOp(boost::get<NoOp*>(node));
        else
            error();
    }

    int visit_BinOp(BinOp* node)
    {
        if (node->op.type == PLUS)
            return visit(node->left) + visit(node->right);
        else if (node->op.type == MINUS)
            return visit(node->left) - visit(node->right);
        else if (node->op.type == MUL)
            return visit(node->left) * visit(node->right);
        else if (node->op.type == DIV)
            return visit(node->left) / visit(node->right);
        else if (node->op.type == POW)
            return int(pow(visit(node->left), visit(node->right)) + 0.5);
    }

    int visit_Num(Num* node)
    {
        //type check here
        return stoi(node->value);
    }

    int visit_UnaryOp(UnaryOp* node)
    {
        if (node->op.type == PLUS)
        {
            return +visit(node->expr);
        }
        else if (node->op.type == MINUS)
        {
            return -visit(node->expr);
        }
    }

    int visit_Compound(Compound* node)
    {
        for (auto child : node->children)
            visit(child);
        return 0;
    }

    int visit_NoOp(NoOp* node)
    {
        return 0;
    }

    int visit_Assign(Assign* node)
    {
        string var_name = boost::get<Var*>(node->left)->value;
        GLOBAL_SCOPE[var_name] = visit(node->right);
        return GLOBAL_SCOPE[var_name];
    }

    int visit_Var(Var* node)
    {
        string var_name = node->value;
        if (GLOBAL_SCOPE.find(var_name) == GLOBAL_SCOPE.end())
        {
            cout << "ERROR:: Variable not defined.\n";
            _Exit(10);
        }
        return GLOBAL_SCOPE[var_name];
    }

    int interpret()
    {
        boostvar tree = parser.parse();
        return visit(tree);
    }
};

int main()
{
    string text;
    string line;
    ifstream MyReadfile("sample.txt");
    while (getline(MyReadfile, line))
        text = text + line + " ";
    text = text.substr(0, text.size() - 1);
        
    string temp = text;
        
    cout << text;

       
    Lexer lexer(text);
    Parser parser(lexer);
    Interpreter interpreter(parser);
    int result = interpreter.interpret();
    for (auto i : GLOBAL_SCOPE)
        cout << i.first << " : " << i.second<<"\n";
}
