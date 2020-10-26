#include <iostream>
#include <string>
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

#define boostvar boost::variant<BinOp*, Num*, UnaryOp*, Compound*, Assign*, Var*, NoOp*>

/* ###############################
   #        LEXER                #
   ###############################
*/

string INTEGER = "INTEGER", PLUS = "PLUS", MINUS = "MINUS", MUL = "MUL", DIV = "DIV", EOL = "EOL", LPAREN = "(", RPAREN = ")", POW = "POW", BEGIN = "BEGIN", END = "END", DOT = "DOT", ID = "ID", ASSIGN = "ASSIGN", SEMI = "SEMI";

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

        if (RESERVED_KEYWORDS.find(result) != RESERVED_KEYWORDS.end())
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

    string integer_val()
    {
        string result = "";
        while (current_char != '\0' && current_char >= 48 && current_char <= 57)
        {
            result += current_char;
            advance();
        }
        return result;
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

        //Means we have a digit
        if (current_char >= 48 && current_char <= 57)
        {
            Token token(INTEGER, integer_val());
            return token;
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
        //atom: (PLUS | MINUS) atom | INTEGER | LPAREN expr RPAREN
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
        boostvar node = Var(current_token);
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
        boostvar node = Assign(left, token, right);
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
        {
            error();
        }

        return results;

    }

    boostvar compound_statement()
    {
        //compound_statement: BEGIN statement_list END
        eat(BEGIN);
        vector<boostvar> nodes = statement_list();
        eat(END);

        Compound* root;
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
        return expr();
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

    int interpret()
    {
        boostvar tree = parser.parse();
        return visit(tree);
    }
};

int main()
{
    string text;
    while (true)
    {
        cout << "calc> ";
        getline(cin, text);
        if (text.size() == 0)
            continue; //meaning that sometimes we tend to press enter but we still want to give input

        string temp = text;
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower); //to check all possible permutations of QUIT
        if (temp == "quit")
            break;

        //call interpreter
        Lexer lexer(text);
        Parser parser(lexer);
        Interpreter interpreter(parser);
        int result = interpreter.interpret();
        //if (parser.current_token.type != EOL)
        //{
        //    cout << "ERROR:: Invalid Syntax";
        //    break;
        //}
        //else
        cout << result << "\n";
    }
}
