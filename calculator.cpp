#include<bits/stdc++.h>
using namespace std;

string INTEGER = "INTEGER", PLUS = "PLUS", MINUS = "MINUS",MUL = "MUL", DIV = "DIV", EOL = "EOL", LPAREN = "(", RPAREN = ")";

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
ostream& operator<<(ostream &strm, const Token &token) {
    return strm << "(" << token.type << ", "<<token.value<<")";
}

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

        Lexer(string text) //Constructors have to be public
        {
            this->text = text;
            this->pos = 0;
            this->current_char = text[pos];
        }

        void error()
        {
            cout<<"ERROR:: Error parsing the input\n";
            _Exit(10);
        }

        void advance()
        {
            pos++;
            if(pos > text.size()-1)
                current_char = '\0';
            else
                current_char = text[pos];
        }

        void skip_whitspace()
        {
            while(current_char != '\0' && current_char == ' ')
                advance();
        }

        string integer_val()
        {
            string result = "";
            while(current_char != '\0' && current_char >= 48 && current_char <= 57)
            {
                result += current_char;
                advance();
            }
            return result;
        }

    /* ******LEXICAL ANALYSER****** */
        Token get_next_token()
        {
            while(current_char == ' ')
                skip_whitspace();

            if(current_char == '\0')
                return Token(EOL, "\0");


            string current_char_str;
            current_char_str = current_char;

            //Means we have a digit
            if(current_char >= 48 && current_char <= 57)
            {
                Token token(INTEGER, integer_val());
                return token;
            }

            if(current_char == '+')
            {
                Token token(PLUS, current_char_str);
                advance();
                return token;
            }

            if(current_char == '-')
            {
                Token token(MINUS, current_char_str);
                advance();
                return token;
            }

            if(current_char == '*')
            {
                Token token(MUL, current_char_str);
                advance();
                return token;
            }

            if(current_char == '/')
            {
                Token token(DIV, current_char_str);
                advance();
                return token;
            }
            
            if(current_char == '(')
            {
                Token token(LPAREN, current_char_str);
                advance();
                return token;
            }

            if(current_char == ')')
            {
                Token token(RPAREN, current_char_str);
                advance();
                return token;
            }

            //If it isnt a digit or + or -, then some other char, hence show error
            error();
        }
};

class Interpreter
{
    Lexer lexer;
    Token current_token;

    void error()
    {
        cout<<"ERROR:: Invalid Syntax\n";
        _Exit(10);
    }

    void eat(string type)
    {
        if(current_token.type == type)
            current_token = this->lexer.get_next_token();
        else
            error();
    }

    string factor()
    {
        //factor: INTEGER
        Token token = current_token;
        if(token.type == INTEGER)
        {
            eat(INTEGER);
            return token.value;
        }

        else if(token.type == LPAREN){
            eat(LPAREN);
            int result = expr();
            eat(RPAREN);
            return to_string(result);
        }
        
    }

    int term()
    {
        //term: factor ((MUL | DIV) factor)*
        int result;

        result = stoi(factor());

        while(current_token.type == MUL || current_token.type == DIV)
        {
            if(current_token.type == MUL)
            {
                eat(MUL);
                result *= stoi(factor());
            }
            else
            {
                eat(DIV);
                result /= stoi(factor());
            }
        }

        return result;
    }

    public:
        Interpreter(Lexer lexer)
        {
            this->lexer = lexer;
            this->current_token = this->lexer.get_next_token();
        }

        int expr()
        {
            int result;

            result = term();

            while(current_token.type == PLUS || current_token.type == MINUS)
            {
                if(current_token.type == PLUS)
                {
                    eat(PLUS);
                    result += term();
                }
                else
                {
                    eat(MINUS);
                    result -= term();
                }
            }

            return result;
        }
};

int main()
{
    string text;
    while(true)
    {
        cout<<"calc> ";
        getline(cin, text);
        if(text.size() == 0)
            continue; //meaning that sometimes we tend to press enter but we still want to give input

        string temp = text;
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower); //to check all possible permutations of QUIT
        if(temp == "quit")
            break;

        //call interpreter
        Lexer lexer(text);
        Interpreter interpreter(lexer);
        cout<<(interpreter.expr())<<"\n";
    }
}