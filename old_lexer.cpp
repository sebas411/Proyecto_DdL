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
    //expression = "(((\t|\n| ))+)|(((A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z))(((A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z))|((0|1|2|3|4|5|6|7|8|9)))*)|\\+|\\*|\\(|\\)";
    expression = subepsilon(expression);
    bool valid = isExpValid(expression);
    if (!valid) return -1;
    string postfix = shuntingYard(expression);
    cout << "Resultado postfix: ";
    coutSafe(postfix);
    TreeNode *expression_tree = postfixToTree(postfix);
     printTree(expression_tree);
    DFA direct_DFA = directConstruction(expression_tree);
    DFA min_DFA = minimize(direct_DFA);
    NFA nauto = Thompson(expression_tree);
    if (argc == 3) {
        cout << "Resultado input (AFD directa min): " << getResult(min_DFA.simulate(input).second == input.length()-1) << endl;
        cout << "Resultado input (NFA): " << getResult(nauto.simulate(input)) << endl;
    }
    createGraph(min_DFA, dfa_min_dc_graph);
    delete expression_tree;
    return 0;
}
