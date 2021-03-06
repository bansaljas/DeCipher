#pragma once
#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_LIST_SIZE 30
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <boost/variant.hpp>
#include <stack>
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
class BuiltinTypeSymbol;
class VarSymbol;
class ProcedureDecl;
class Param;
class ProcedureSymbol;
class Read;
class Print;
class ProcedureCall;
class Condition;
class Loop;
class Message;

#define boostvar boost::variant<BinOp*, Num*, UnaryOp*, Compound*, Assign*, Var*, NoOp*, Program*, Block*, VarDecl*, Type*, BuiltinTypeSymbol*, VarSymbol*, ProcedureDecl*, Param*, ProcedureSymbol*, Print*, ProcedureCall*, Read*, Condition*, Loop*, Message*>
#define typevar boost::variant<int, float, string>

unordered_map<string, float> GLOBAL_SCOPE;

/* ###############################
   #        LEXER                #
   ###############################
*/

string INTEGER = "INTEGER", PLUS = "PLUS", MINUS = "MINUS", MUL = "MUL", DIV = "DIV", EOL = "EOL", LPAREN = "(", RPAREN = ")", POW = "POW", BEGIN = "BEGIN", END = "END", DOT = "DOT",
ID = "ID", ASSIGN = "ASSIGN", SEMI = "SEMI", PROGRAM = "PROGRAM", VAR = "VAR", COLON = "COLON", WHILE = "WHILE", ENDWHILE = "ENDWHILE",
COMMA = "COMMA", REAL = "REAL", INTEGER_CONST = "INTEGER_CONST", REAL_CONST = "REAL_CONST", IF = "IF", ELSE = "ELSE", ENDIF = "ENDIF",
INTEGER_DIV = "INTEGER_DIV", FLOAT_DIV = "FLOAT_DIV", PROCEDURE = "PROCEDURE", PRINT = "PRINT", READ = "READ", QUOTE = "QUOTE", SEP = "SEP";

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
    
public:
    int lineno, column;
    char current_char;

    Lexer()
    {
        this->pos = 0;
        this->current_char = '\0';
        this->lineno = 1;
        this->column = 1;
    }

    Lexer(string text)
    {
        this->text = text;
        this->pos = 0;
        this->current_char = text[pos];
        this->lineno = 1;
        this->column = 1;
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
        RESERVED_KEYWORDS[PROCEDURE] = Token("PROCEDURE", PROCEDURE);
        RESERVED_KEYWORDS[PRINT] = Token("PRINT", PRINT);
        RESERVED_KEYWORDS[READ] = Token("READ", READ);
        RESERVED_KEYWORDS[IF] = Token("IF", IF);
        RESERVED_KEYWORDS[ELSE] = Token("ELSE", ELSE);
        RESERVED_KEYWORDS[ENDIF] = Token("ENDIF", ENDIF);
        RESERVED_KEYWORDS[WHILE] = Token("WHILE", WHILE);
        RESERVED_KEYWORDS[ENDWHILE] = Token("ENDWHILE", ENDWHILE);
    }

    void error()
    {
        cout << "ERROR:: Lexer error on '"<<current_char<< "' line:: "<<lineno<<":"<<column<<endl;
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
        if (current_char == '\n')
        {
            this->lineno++;
            this->column = 0;
        }

        pos++;
        if (pos > text.size() - 1)
            current_char = '\0';
        else
        {
            current_char = text[pos];
            this->column++;
        }
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

        while (current_char == '\n' || current_char == ' ')
        {
            if (current_char == '\n')
                advance();

            if (current_char == ' ')
                skip_whitspace();
        }
        
        if (current_char == '\0')
            return Token(EOL, "\0");

        if (current_char == '{')
        {
            advance();
            skip_comment();
            return get_next_token();
        }

        string current_char_str;
        current_char_str = current_char;
                

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

        if (current_char == '<' && peek() == '<')
        {
            Token token(SEP, "<<");
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

        if (current_char == '"')
        {
            Token token(QUOTE, "\"");
            advance();
            return token;
        }

        //If it isnt a digit or + or -, then some other char, hence show error
        error();
    }
};