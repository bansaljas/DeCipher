#include "interpreter.h"

int main()
{
    string text;
    string line;
    ifstream MyReadfile("sample.txt");
    while (getline(MyReadfile, line))
        text = text + line + " ";
    text = text.substr(0, text.size() - 1);

    string temp = text;

    cout << text<<endl;


    Lexer lexer(text);
    Parser parser(lexer);
    Interpreter interpreter(parser);
    int result = interpreter.interpret();
    for (auto i : GLOBAL_SCOPE)
        cout << i.first << " : " << i.second << "\n";
}
