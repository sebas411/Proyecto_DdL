#include "lib.hpp"

bool isFinalCheck(State dState, set<State> nFinalStates, int *accept_pattern) {
    bool flag = false;
    int matching_accept_pattern = -1;
    for (State s : dState.NFA_States) {
        for (State n_s : nFinalStates) {
            if (s == n_s) {
                flag = true;
                if (matching_accept_pattern == -1 || n_s.accepting_pattern < matching_accept_pattern) {
                    matching_accept_pattern = n_s.accepting_pattern;
                }
                break;
            }
        }
    }
    if (flag) *accept_pattern = matching_accept_pattern;
    return flag;
}

DFA subsetConstruction(NFA Nautomaton) {
    DFA Dautomaton = DFA();
    // initially, e-closure(s0) is the only state in Dstates, and it is unmarked;
    State ini = State(0, Nautomaton.e_closure(Nautomaton.initial_state));
    Dautomaton.initial_state = ini;
    Dautomaton.symbols = Nautomaton.symbols;
    Dautomaton.states.insert(ini);
    int accept_pattern;
    if (isFinalCheck(ini,Nautomaton.final_states, &accept_pattern)) {
        ini.accepting_pattern = accept_pattern;
        Dautomaton.final_states.insert(ini);
    }
    stack<State> state_stack;
    state_stack.push(ini);
    int counter = 1;
    // while ( there is an unmarked state T in Dstates )
    while (!state_stack.empty()) {
        // mark T
        State subset_state = state_stack.top();
        state_stack.pop();
        
        // for ( each input symbol a )
        for (Symbol s: Dautomaton.symbols) {
            // U = e-closure(move(T,a));
            set<State> U = Nautomaton.e_closure(Nautomaton.move(subset_state.NFA_States, s));

            bool inDstates = false;
            // check if U is in Dstates
            for (State t: Dautomaton.states) {
                if (t.NFA_States == U) {
                    // Dtran[T,a] = U
                    Dautomaton.transitions.push_back(Transition(subset_state, t, s));
                    inDstates = true;
                    break;
                }
            }

            // if ( U is not in Dstates )
            if (!inDstates) {
                // add U as an unmarked state to Dstates
                State new_state = State(counter, U);
                counter += 1;
                Dautomaton.states.insert(new_state);
                
                if (isFinalCheck(new_state, Nautomaton.final_states, &accept_pattern)) {
                    new_state.accepting_pattern = accept_pattern;
                    Dautomaton.final_states.insert(new_state);
                }
                state_stack.push(new_state);
                // Dtran[T,a] = U;
                Dautomaton.transitions.push_back(Transition(subset_state, new_state, s));
            }
        }
    }
    return Dautomaton;
}