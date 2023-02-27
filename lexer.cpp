#include "lib.hpp"
#include "thompson.hpp"
#include "shunting_yard.hpp"
#include "subset_construction.hpp"

// main
int main(int argc, char *argv[]){
    string expression;
    
    if (argc == 2) {
        expression = argv[1];
    } else if (argc < 2) {
        cout << "Por favor ingrese una expresión regular." << endl;
        cout << "Uso: ./lexer \"[expression]\"" << endl;
        return -1;
    } else {
        cout << "Ha ingresado demasiados argumentos." << endl;
        cout << "Uso: ./lexer \"[expression]\"" << endl;
        return -1;
    }
    expression = subepsilon(expression);
    bool valid = isExpValid(expression);
    if (!valid) return -1;
    string postfix = shuntingYard(expression);
    cout << "Resultado postfix: ";
    coutEpsilonSafe(postfix);
    TreeNode *expression_tree = postfixToTree(postfix);
    NFA Nautomaton = Thompson(expression_tree);
    DFA Dautomaton = subsetConstruction(Nautomaton);
    //createGraph(Nautomaton);
    createGraph(Dautomaton);
    return 0;
}
