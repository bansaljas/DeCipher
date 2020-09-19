#include<bits/stdc++.h>
using namespace std;

string INTEGER = "INTEGER", PLUS = "PLUS", EOL = "EOL";

class Token
{
    public:

        int value; //will store the value in ASCII
        string type; //will store the word

        Token()
        {
            this->value = 0;
            this->type = "";
        }

        Token(string type, int value)
        {
            this->value = value;
            this->type = type;
        }
};

//To print the token in the correct format
ostream& operator<<(ostream &strm, const Token &token) {
    return strm << "(" << token.type << ", "<<char(token.value)<<")";
}

class Interpreter
{
    string text;
    int pos;
    Token current_token;

    void error()
    {
        cout<<"ERROR:: Error parsing the input\n";
        _Exit(10);
    }

/* ******LEXICAL ANALYSER****** */
    Token get_next_token()
    {
        if(pos > text.size()-1)
            return Token(EOL, '\0');

        char current_char = text[pos];

        //Means we have a digit
        if(current_char >= 48 && current_char <= 57)
        {
            Token token(INTEGER, current_char);
            pos++;
            return token;
        }

        if(current_char == '+')
        {
            Token token(PLUS, current_char);
            pos++;
            return token;
        }

        //If it isnt a digit or +, then some other char, hence show error
        error();
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
        }

        int eval()
        {
            current_token = get_next_token();

            int value1 = (current_token.value)-48;
            eat(INTEGER);

            eat(PLUS);

            int value2 = (current_token.value)-48;
            eat(INTEGER);

            return (value1 + value2);
        }
};

int main()
{
    string text;
    while(true)
    {
        cout<<"calc> ";
        cin>>text;
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
