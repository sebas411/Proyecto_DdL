#include "lib.hpp"
#include "shunting_yard.hpp"
#include "direct_construction.hpp"
#include "minimization.hpp"


/*
    Note to self: If you want to make more efficient the lex analyzer you must change the use
    of DFA for a new class with a better transition table
*/

DFA MakeLexAnalyzer(vector<string> regexes) {
    int startState = 0, current_accepting_pattern = 0;
    vector<TreeNode*> trees;
    for (string regex: regexes) {
        string postfix = shuntingYard(regex);
        TreeNode *tree = postfixToTree(postfix);
        trees.push_back(tree);
    }
    DFA dautomata = directConstruction(trees);
    DFA min_dautomata = minimize(dautomata);
    return min_dautomata;
}
