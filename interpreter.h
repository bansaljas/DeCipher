#include "symbol.h"

/* ###############################
   #       INTERPRETER           #
   ###############################
*/


class Interpreter
{
    boostvar tree;

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
        GLOBAL_SCOPE[var_name] = visit(node->right);
        return GLOBAL_SCOPE[var_name];
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
        if (GLOBAL_SCOPE.find(var_name) == GLOBAL_SCOPE.end())
        {
            cout << "ERROR:: Variable "<< var_name <<" not defined.\n";
            _Exit(10);
        }
        return GLOBAL_SCOPE[var_name];
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
        if (node->proc_symbol)
        {
            ProcedureSymbol* proc_symbol = node->proc_symbol;
            for (int i = 0; i < proc_symbol->params.size(); i++)
            {
                VarSymbol* var = boost::get<VarSymbol*>(proc_symbol->params[i]);
                string var_name = var->name;
                boostvar assign_var = new Var(Token(ID, var_name));
                visit_Assign(new Assign(assign_var, Token(ASSIGN, ":="), node->actual_params[i]));
            }

            visit(proc_symbol->block_node);
        }
        
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
        visit(node->block);
        return 0;
    }

    int interpret()
    {
        return visit(tree);
    }
};
