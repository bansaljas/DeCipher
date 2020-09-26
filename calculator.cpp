#include<bits/stdc++.h>
using namespace std;

string INTEGER = "INTEGER", PLUS = "PLUS", MINUS = "MINUS", EOL = "EOL";

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

class Interpreter
{
    string text;
    int pos;
    Token current_token;
    char current_char;

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
        while(current_char != '\0')
        {
            if(current_char == ' ')
            {
                skip_whitspace();
                continue;
            }

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

            //If it isnt a digit or + or -, then some other char, hence show error
            error();
        }
        return Token(EOL, "\0");
    }

    void eat(string type)
    {
        if(current_token.type == type)
            current_token = get_next_token();
        else
            error();
    }

    public:
        Interpreter(string text) //Constructors have to be public
        {
            this->text = text;
            this->pos = 0;
            this->current_char = text[pos];
        }

        int eval()
        {
            current_token = get_next_token();

            int value1 = stoi(current_token.value);
            eat(INTEGER);

            string operator_type = current_token.type;

            if(operator_type == "PLUS")
                eat(PLUS);
            else
                eat(MINUS);

            int value2 = stoi(current_token.value);
            eat(INTEGER);

            if(operator_type == "PLUS")
                return (value1 + value2);
            else
                return (value1 - value2);
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
        Interpreter interpreter(text);
        cout<<(interpreter.eval())<<"\n";
    }
}
