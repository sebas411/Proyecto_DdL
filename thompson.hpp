#include "lib.hpp"

NFA Thompson(TreeNode *expression_tree, int start=0) {
    if (expression_tree) {
        // operands
        if (!expression_tree->left && !expression_tree->right) { 
            NFA result;
            State initial(start);
            State final(start+1);
            Symbol s(expression_tree->value);
            Transition t(initial, final, s);
            result.final_states.insert(final);
            result.thompsonFinal = final;
            result.states.insert(initial);
            result.states.insert(final);
            result.symbols.insert(s);
            result.initial_state = initial;
            result.transitions.push_back(t);
            return result;
        } else {
            // concat
            if (expression_tree->value == '~') {
                NFA left = Thompson(expression_tree->left, start);
                start = left.max_state() + 1;
                NFA right = Thompson(expression_tree->right,start);

                left.merge(right);
                State lfinal = left.thompsonFinal;
                State rinitial = right.initial_state;
                for (Transition t : right.transitions) {
                    if (t.origin_state == rinitial) {
                        left.transitions.push_back(Transition(lfinal,t.destiny_state,t.symbol));
                    } else {
                        left.transitions.push_back(t);
                    }
                }

                left.final_states = right.final_states;
                left.states.erase(left.thompsonFinal);
                left.thompsonFinal = right.thompsonFinal;
                return left;
            } else if (expression_tree->value == '|') {
            // or
                State ini = State(start);
                NFA one = Thompson(expression_tree->left, start + 1);
                start = one.max_state() + 1;
                NFA two = Thompson(expression_tree->right, start);
                start = two.max_state() + 1;

                one.merge(two);
                for (Transition t : two.transitions) {
                    one.transitions.push_back(t);
                }
                
                State fin = State(start);
                one.transitions.push_back(Transition(ini, one.initial_state, epsilon));
                one.transitions.push_back(Transition(ini, two.initial_state, epsilon));
                one.transitions.push_back(Transition(one.thompsonFinal, fin, epsilon));
                one.transitions.push_back(Transition(two.thompsonFinal, fin, epsilon));
                one.final_states.clear();
                one.final_states.insert(fin);
                one.states.insert(ini);
                one.states.insert(fin);
                one.initial_state = ini;
                one.thompsonFinal = fin;
                return one;
            } else if (expression_tree->value == '*') {
            // kleene
                State ini = State(start);
                NFA son = Thompson(expression_tree->left, start + 1);
                start = son.max_state() + 1;
                State fin = State(start);
                son.states.insert(ini);
                son.states.insert(fin);
                son.transitions.push_back(Transition(ini, son.initial_state, epsilon));
                son.transitions.push_back(Transition(son.thompsonFinal, fin, epsilon));
                son.transitions.push_back(Transition(ini, fin, epsilon));
                son.transitions.push_back(Transition(son.thompsonFinal, son.initial_state, epsilon));

                son.final_states.clear();
                son.final_states.insert(fin);
                son.initial_state = ini;
                son.thompsonFinal = fin;
                return son;
            } else if (expression_tree->value == '+') {
            // one or more
                State ini = State(start);
                NFA son = Thompson(expression_tree->left, start + 1);
                start = son.max_state() + 1;
                State fin = State(start);
                son.states.insert(ini);
                son.states.insert(fin);
                son.transitions.push_back(Transition(ini, son.initial_state, epsilon));
                son.transitions.push_back(Transition(son.thompsonFinal, fin, epsilon));
                son.transitions.push_back(Transition(son.thompsonFinal, son.initial_state, epsilon));

                son.final_states.clear();
                son.final_states.insert(fin);
                son.initial_state = ini;
                son.thompsonFinal = fin;
                return son;
            } else if (expression_tree->value == '?') {
            //optional
                State ini = State(start);
                NFA son = Thompson(expression_tree->left, start + 1);
                start = son.max_state() + 1;
                State fin = State(start);
                son.states.insert(ini);
                son.states.insert(fin);
                son.transitions.push_back(Transition(ini, son.initial_state, epsilon));
                son.transitions.push_back(Transition(son.thompsonFinal, fin, epsilon));
                son.transitions.push_back(Transition(son.initial_state, son.thompsonFinal, epsilon));

                son.final_states.clear();
                son.final_states.insert(fin);
                son.initial_state = ini;
                son.thompsonFinal = fin;
                return son;
            }
        }
    }
    return NFA();
}