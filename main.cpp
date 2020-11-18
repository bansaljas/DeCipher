#include "interpreter.h"

int main()
{
    string text;
    string line;
    ifstream MyReadfile("sample.txt");
    while (getline(MyReadfile, line))
        text = text + line + "\n";
    text = text.substr(0, text.size() - 1);

    string temp = text;


    Lexer lexer(text);
    Parser parser(lexer);
    boostvar tree = parser.parse();
    SemanticAnalyzer semantic_analyser;
    semantic_analyser.visit(tree);
    Interpreter interpreter(tree);
    int result = interpreter.interpret();

    //for (auto i : GLOBAL_SCOPE)
        //cout << i.first << " : " << i.second << "\n";
}
