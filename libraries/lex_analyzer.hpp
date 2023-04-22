#include "lib.hpp"
#include "shunting_yard.hpp"
#include "thompson.hpp"
#include "subset_construction.hpp"
#include "minimization.hpp"


/*
    Note to self: If you want to make more efficient the lex analyzer you must change the use
    of DFA for a new class with a better transition table
*/

DFA MakeLexAnalyzer(vector<string> regexes) {
    int startState = 0, current_accepting_pattern = 0;
    State initial(startState);
    startState++;
    NFA nautomata;
    nautomata.initial_state=initial;
    for (string regex: regexes) {
        string postfix = shuntingYard(regex);
        TreeNode *tree = postfixToTree(postfix);
        NFA sub_automata = Thompson(tree, startState);
        startState = sub_automata.max_state() + 1;

        nautomata.merge(sub_automata, true);
        State accepting = sub_automata.thompsonFinal;
        accepting.accepting_pattern = current_accepting_pattern;
        nautomata.final_states.insert(accepting);
        nautomata.transitions.push_back(Transition(initial, sub_automata.initial_state, epsilon));
        current_accepting_pattern++;
    }
    DFA dautomata = subsetConstruction(nautomata);
    cout << "before minimization" << endl;
    DFA min_dautomata = minimize(dautomata);
    cout << "after minimization" << endl;
    return min_dautomata;
}
