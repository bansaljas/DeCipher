#include "symbol.h"
#include <stack>

/* ###############################
   #       INTERPRETER           #
   ###############################
*/

//enum used in original code
class ARType
{
public:
    string PROGRAM = "PROGRAM";
    string PROCEDURE = "PROCEDURE";
};

class ActivationRecord 
{
public:
    string name;
    string type;
    int nestingLevel;
    unordered_map<string, float> members;

    ActivationRecord(){}

    ActivationRecord(string name, string type, int nestingLevel) 
    {
        this->name = name;
        this->type = type;
        this->nestingLevel = nestingLevel;
    }

    void setItem(string key, float value)
    {
        members[key] = value;
    }

    float getItem(string key)
    {
        return members[key];
    }
    //can add get function which does not throw exception if key not found
    //can also add function for printing the activation record
};

class CallStack 
{

public:
    stack<ActivationRecord> records;

    //Name of function can be changed, to avoid recursion
    void push(ActivationRecord ar)
    {
        this->records.push(ar);
    }

    ActivationRecord pop()
    {
        ActivationRecord element = records.top();
        records.pop();
        return element;
    }

    ActivationRecord peek()
    {
        return records.top();
    } 
    //can add function for printing the stack
};


class Interpreter
{
    boostvar tree;
    CallStack call_stack;

public:
    Interpreter(boostvar tree)
    {
        this->tree = tree;
    }

    void error()
    {
        cout << "ERROR:: No such parsing method present\n";
        _Exit(10);
    }

    float visit(boostvar node)
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
        else if (node.which() == 7)
            return visit_Program(boost::get<Program*>(node));
        else if (node.which() == 8)
            return visit_Block(boost::get<Block*>(node));
        else if (node.which() == 9)
            return visit_VarDecl(boost::get<VarDecl*>(node));
        else if (node.which() == 10)
            return visit_Type(boost::get<Type*>(node));
        else if (node.which() == 13)
            return visit_ProcedureDecl(boost::get<ProcedureDecl*>(node));
        else if (node.which() == 16)
            return visit_Print(boost::get<Print*>(node));
        else if (node.which() == 17)
            return visit_ProcedureCall(boost::get<ProcedureCall*>(node));
        else
            error();
    }

    float visit_BinOp(BinOp* node)
    {
        if (node->op.type == PLUS)
            return visit(node->left) + visit(node->right);
        else if (node->op.type == MINUS)
            return visit(node->left) - visit(node->right);
        else if (node->op.type == MUL)
            return visit(node->left) * visit(node->right);
        else if (node->op.type == INTEGER_DIV)
            return int(visit(node->left) / visit(node->right));
        else if (node->op.type == FLOAT_DIV)
            return visit(node->left) / visit(node->right);
        else if (node->op.type == POW)
            return int(pow(visit(node->left), visit(node->right)) + 0.5);
    }

    float visit_Num(Num* node)
    {
        if (node->token.type == INTEGER_CONST)
            return stoi(node->value);
        else
            return stof(node->value);
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
        ActivationRecord ar = this->call_stack.peek();
        ar.members[var_name] = visit(node->right);
        return ar.members[var_name];
    }

    int visit_Print(Print* node)
    {
        Var* output = boost::get<Var*>(node->output_variable);
        cout << "\nValue of " << output->value << " is: ";
        cout << GLOBAL_SCOPE[output->value] << endl;
        return 0;
    }

    int visit_Var(Var* node)
    {
        string var_name = node->value;
        ActivationRecord ar = this->call_stack.peek();
        //Need to make changes here, i guess... for the function to access non-local variables
        if (ar.members.find(var_name) == ar.members.end())
        {
            cout << "ERROR:: Variable "<< var_name <<" not defined.\n";
            _Exit(10);
        }
        return ar.members[var_name];
    }

    int visit_Type(Type* node)
    {
        return 0;
    }

    int visit_VarDecl(VarDecl* node)
    {
        return 0;
    }

    int visit_ProcedureDecl(ProcedureDecl* node) {
        return 0;
    }

    int visit_ProcedureCall(ProcedureCall* node)
    {
        /*if (node->proc_symbol)
        {
            ProcedureSymbol* proc_symbol = node->proc_symbol;

            if (proc_symbol->params.size() != node->actual_params.size())
            {
                if (proc_symbol->params.size() > node->actual_params.size())
                    cout << "ERROR:: Too few arguments given in the function call." << endl;
                else
                    cout << "ERROR:: Too many arguments given in the function call." << endl;
                _Exit(10);
            }

            for (int i = 0; i < proc_symbol->params.size(); i++)
            {
                VarSymbol* var = boost::get<VarSymbol*>(proc_symbol->params[i]);
                string var_name = var->name;
                boostvar assign_var = new Var(Token(ID, var_name));
                visit_Assign(new Assign(assign_var, Token(ASSIGN, ":="), node->actual_params[i]));
            }

            visit(proc_symbol->block_node);
        }*/
        string proc_name = node->proc_name;
        ARType ar_type;
        ActivationRecord ar(proc_name, ar_type.PROCEDURE, 2);
        ProcedureSymbol* proc_symbol = node->proc_symbol;
        vector<boostvar> formal_params = proc_symbol->params;
        vector<boostvar> actual_params = node->actual_params;
        for (int i = 0; i < formal_params.size(); i++)
        {
            VarSymbol* var = boost::get<VarSymbol*>(formal_params[i]);
            string var_name = var->name;
            ar.members[var_name] = visit(actual_params[i]);
        }
        this->call_stack.push(ar);
        visit(proc_symbol->block_node);
        this->call_stack.pop();
        return 0;
    }

    int visit_Block(Block* node)
    {
        for (auto declaration : node->declarations)
            visit(declaration);
        visit(node->compound_statement);

        return 0;
    }

    int visit_Program(Program* node)
    {
        string program_name = node->name;
        ARType ar_object;
        ActivationRecord ar(program_name, ar_object.PROGRAM, 1);
        this->call_stack.push(ar);
        visit(node->block);
        this->call_stack.pop();
        return 0;
    }

    int interpret()
    {
        return visit(tree);
    }
    //Have not implemented the log function that prints the call stack
};
