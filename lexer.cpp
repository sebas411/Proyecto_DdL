#include "lib.hpp"
#include "thompson.hpp"
#include "shunting_yard.hpp"

// main
int main(){
    string postfix = shuntingYard("(a)*");
    cout << "Resultado: " << postfix << endl;
    TreeNode *expression_tree = postfixToTree(postfix);
    NFA automata = Thompson(expression_tree);
    createGraph(automata);

    return 0;
}
