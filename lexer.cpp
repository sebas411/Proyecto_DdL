#include "libraries/lib.hpp"
#include "libraries/thompson.hpp"
#include "libraries/shunting_yard.hpp"
#include "libraries/subset_construction.hpp"
#include "libraries/direct_construction.hpp"
#include "libraries/minimization.hpp"

// main
int main(int argc, char *argv[]){
    string expression;
    string input;
    
    if (argc == 2) {
        expression = argv[1];
    } else if (argc < 2) {
        cout << "Por favor ingrese una expresión regular y opcionalmente una cadena para revisión." << endl;
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
    DFA Dautomaton2 = directConstruction(expression_tree);
    DFA Dautomaton3 = minimize(Dautomaton);
    DFA Dautomaton4 = minimize(Dautomaton2);
    if (argc == 3) {
        cout << "Resultado input (AFN): " << getResult(Nautomaton.simulate(input)) << endl;
        cout << "Resultado input (AFD subconjuntos): " << getResult(Dautomaton.simulate(input)) << endl;
        cout << "Resultado input (AFD directa): " << getResult(Dautomaton2.simulate(input)) << endl;
        cout << "Resultado input (AFD subconjuntos min): " << getResult(Dautomaton3.simulate(input)) << endl;
        cout << "Resultado input (AFD directa min): " << getResult(Dautomaton4.simulate(input)) << endl;
    }
    createGraph(Nautomaton, nfa_graph);
    createGraph(Dautomaton, dfa_sc_graph);
    createGraph(Dautomaton2, dfa_dc_graph);
    createGraph(Dautomaton3, dfa_min_sc_graph);
    createGraph(Dautomaton4, dfa_min_dc_graph);
    delete expression_tree;
    return 0;
}
