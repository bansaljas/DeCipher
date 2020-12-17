#include "symbol.h"
#include <stack>

/* ###############################
   #       INTERPRETER           #
   ###############################
*/

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
    unordered_map<string, typevar> members;
    unordered_map<string, string> declarations;

    ActivationRecord()
    {
        this->nestingLevel = 0;
    }

    ActivationRecord(string name, string type, int nestingLevel) 
    {
        this->name = name;
        this->type = type;
        this->nestingLevel = nestingLevel;
    }

    void setItem(string key, typevar value)
    {
        members[key] = value;
    }

    typevar getItem(string key)
    {
        return members[key];
    }
};

class CallStack 
{

public:
    stack<ActivationRecord*> records;

    void push(ActivationRecord* ar)
    {
        this->records.push(ar);
    }

    ActivationRecord* pop()
    {
        ActivationRecord* element = records.top();
        records.pop();
        return element;
    }

    ActivationRecord* peek()
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

    typevar visit(boostvar node)
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
        else if (node.which() == 18)
            return visit_Read(boost::get<Read*>(node));
        else if (node.which() == 19)
            return visit_Condition(boost::get<Condition*>(node));
        else if (node.which() == 20)
            return visit_Loop(boost::get<Loop*>(node));
        else if (node.which() == 21)
            return visit_Message(boost::get<Message*>(node));
        else
            error();
    }

    typevar visit_BinOp(BinOp* node)
    {
        typevar left = visit(node->left);
        typevar right = visit(node->right);

        if (node->op.type == PLUS)
        {
            if (left.which() == 1 || right.which() == 1) //means one of the is float
                return boost::get<float>(left) + boost::get<float>(right);
            else
                return boost::get<int>(left) + boost::get<int>(right);
        }
        else if (node->op.type == MINUS)
        {
            if (left.which() == 1 || right.which() == 1) //means one of the is float
                return boost::get<float>(left) - boost::get<float>(right);
            else
                return boost::get<int>(left) - boost::get<int>(right);
        }
        else if (node->op.type == MUL)
        {
            if (left.which() == 1 || right.which() == 1) //means one of the is float
                return boost::get<float>(left) * boost::get<float>(right);
            else
                return boost::get<int>(left) * boost::get<int>(right);
        }
        else if (node->op.type == INTEGER_DIV)
        {
            if (left.which() == 1 || right.which() == 1) //means one of the is float
                return int(boost::get<float>(left) / boost::get<float>(right));
            else
                return boost::get<int>(left) / boost::get<int>(right);
        }
        else if (node->op.type == FLOAT_DIV)
        {
            if (left.which() == 1 || right.which() == 1) //means one of the is float
                return boost::get<float>(left) / boost::get<float>(right);
            else
                return boost::get<int>(left) / boost::get<int>(right);
        }
        else if (node->op.type == POW)
        {
            if (left.which() == 1 || right.which() == 1) //means one of the is float
                return int(pow(boost::get<float>(left), boost::get<float>(right)) + 0.5);
            else
                return int(pow(boost::get<int>(left), boost::get<int>(right)) + 0.5);
        }
    }

    typevar visit_Num(Num* node)
    {
        if (node->token.type == INTEGER_CONST)
            return stoi(node->value);
        else if(node->token.type == REAL_CONST)
            return stof(node->value);
        else
        {
            cout << "ERROR:: Invalid Input" << endl;
            _Exit(10);
        }
    }

    typevar visit_UnaryOp(UnaryOp* node)
    {
        typevar val = visit(node->expr);
        if (node->op.type == PLUS)
        {
            return val;
        }
        else if (node->op.type == MINUS)
        {
            if (val.which() == 0) return -1 * boost::get<int>(val);
            else return -1 * boost::get<float>(val);
        }
    }

    typevar visit_Compound(Compound* node)
    {
        for (auto child : node->children)
            visit(child);
        return 0;
    }

    typevar visit_NoOp(NoOp* node)
    {
        return 0;
    }

    typevar visit_Assign(Assign* node)
    {
        string var_name = boost::get<Var*>(node->left)->value;
        ActivationRecord *ar = this->call_stack.peek();
        typevar val = visit(node->right);
        
        if (ar->declarations[var_name] == "INTEGER" && val.which() == 1)
        {
            cout << "ERROR:: Incompatible type: variable '" << var_name << "'" << endl;
            _Exit(10);
        }

        ar->members[var_name] = val;
        return 0;
    }

    typevar visit_Message(Message* node)
    {
        return node->msg;
    }

    typevar visit_Read(Read* node)
    {
        Var* variable = boost::get<Var*>(node->var);
        cout << "Enter the value of " << variable->value <<": ";
        string input_value; cin >> input_value;

        Num* number;
        if(input_value.find('.') != string::npos)
            number = new Num(Token("REAL_CONST", input_value));
        else
            number = new Num(Token("INTEGER_CONST", input_value));
        
        visit_Assign(new Assign(variable, Token(ASSIGN, ":="), number));
        return 0;
    }

    typevar visit_Print(Print* node)
    {
        for (auto message : node->messages)
        {
            typevar output = visit(message);
            if (output.which() == 0) cout << boost::get<int>(output) << " ";
            else if (output.which() == 1) cout << boost::get<float>(output) << " ";
            else cout << boost::get<string>(output);
        }
        cout << endl;
        
        return 0;
    }

    typevar visit_Condition(Condition* node)
    {
        typevar condition_value = visit(node->condition_node);
        if ((condition_value.which() == 1 && boost::get<float>(condition_value) != 0) || 
            (condition_value.which() == 0 && boost::get<int>(condition_value) != 0))
        {
            for (auto statement : node->if_statements)
                visit(statement);
        }
        else
        {
            for (auto statement : node->else_statements)
                visit(statement);
        }
        return 0;
    }

    typevar visit_Loop(Loop* node)
    {
        typevar condition_value = visit(node->condition_node);
        while ((condition_value.which() == 1 && boost::get<float>(condition_value) != 0) ||
            (condition_value.which() == 0 && boost::get<int>(condition_value) != 0))
        {
            for (auto statement : node->statements)
                visit(statement);
            condition_value = visit(node->condition_node);
        }
        return 0;
    }

    typevar visit_Var(Var* node)
    {
        string var_name = node->value;
        ActivationRecord* ar = this->call_stack.peek();
        if (ar->members.find(var_name) == ar->members.end())
        {
            cout << "ERROR:: Variable "<< var_name <<" not defined.\n";
            _Exit(10);
        }
        return ar->members[var_name];
    }

    int visit_Type(Type* node)
    {
        return 0;
    }

    int visit_VarDecl(VarDecl* node)
    {
        ActivationRecord* ar = this->call_stack.peek();
        Var* var = boost::get<Var*>(node->var_node);
        Type* type_val = boost::get<Type*>(node->type_node);
        ar->declarations[var->value] = type_val->value;
        return 0;
    }

    int visit_ProcedureDecl(ProcedureDecl* node) {
        return 0;
    }

    int visit_ProcedureCall(ProcedureCall* node)
    {
        string proc_name = node->proc_name;
        ProcedureSymbol* proc_symbol = node->proc_symbol;

        ActivationRecord* parent = this->call_stack.peek();

        ARType ar_type;
        ActivationRecord* ar = new ActivationRecord(proc_name, ar_type.PROCEDURE, proc_symbol->scope_level+1);
        
        vector<boostvar> formal_params = proc_symbol->params;
        vector<boostvar> actual_params = node->actual_params;

        if (formal_params.size() != actual_params.size())
        {
            if (formal_params.size() > actual_params.size())
                cout << "ERROR:: Too few arguments given in the function call." << endl;
            else
                cout << "ERROR:: Too many arguments given in the function call." << endl;
            _Exit(10);
        }

        for (int i = 0; i < formal_params.size(); i++)
        {
            VarSymbol* var = boost::get<VarSymbol*>(formal_params[i]);
            string var_name = var->name;
            typevar val = visit(actual_params[i]);
            BuiltinTypeSymbol* type_val = boost::get<BuiltinTypeSymbol*>(var->type);
            if (type_val->name == INTEGER && val.which() == 1)
            {
                cout << "ERROR:: Incompatible type: variable '" << var_name << "'" << endl;
                _Exit(10);
            }

            ar->members[var_name] = val;
        }

        for (auto i : parent->members)
            ar->members[i.first] = i.second;

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
        ActivationRecord* ar = new ActivationRecord(program_name, ar_object.PROGRAM, 1);
        this->call_stack.push(ar);
        visit(node->block);
        this->call_stack.pop();
        return 0;
    }

    typevar interpret()
    {
        return visit(tree);
    }
};
