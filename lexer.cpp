#include "libraries/lib.hpp"
#include "libraries/thompson.hpp"
#include "libraries/shunting_yard.hpp"
#include "libraries/subset_construction.hpp"
#include "libraries/direct_construction.hpp"

// main
int main(int argc, char *argv[]){
    string expression;
    string input;
    
    if (argc == 2) {
        expression = argv[1];
    } else if (argc < 2) {
        cout << "Por favor ingrese una expresión regular." << endl;
        cout << "Uso: ./lexer \"regex\" [\"input\"]" << endl;
        return -1;
    } else if (argc == 3) {
        expression = argv[1];
        input = argv[2];
    } else {
        cout << "Ha ingresado demasiados argumentos." << endl;
        cout << "Uso: ./lexer \"regex\" [\"input\"]" << endl;
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
    if (argc == 3) {
        cout << "Resultado input (NFA): " << Nautomaton.simulate(input) << endl;
        cout << "Resultado input (DFA): " << Dautomaton.simulate(input) << endl;
    }
    DFA Dautomaton2 = directConstruction(expression_tree);
    createGraph(Nautomaton, nfa_graph);
    createGraph(Dautomaton, dfa_sc_graph);
    createGraph(Dautomaton2, dfa_dc_graph);
    delete expression_tree;
    return 0;
}
