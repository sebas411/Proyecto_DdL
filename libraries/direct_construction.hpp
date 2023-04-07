#include "lib.hpp"

TreeNode* fixTree(TreeNode *tree) {
    if (tree->left == NULL && tree->right == NULL) {
        return new TreeNode((tree->value));
    } else if (tree->value == '?') {
        TreeNode *left = fixTree(tree->left);
        TreeNode *right = new TreeNode((char)(-1));
        TreeNode *newTree = new TreeNode('|', left, right);
        return newTree;
    } else if (tree->value == '+') {
        TreeNode *left = fixTree(tree->left);
        TreeNode *subright = fixTree(tree->left);
        TreeNode *right = new TreeNode('*', subright);
        TreeNode *newTree = new TreeNode('~', left, right);
        return newTree;
    } else if (tree->value == '*') {
        TreeNode *unique = fixTree(tree->left);
        TreeNode *newTree = new TreeNode('*', unique);
        return newTree;
    }
    //only ~ and | remaining
    TreeNode *left = fixTree(tree->left);
    TreeNode *right = fixTree(tree->right);
    TreeNode *newTree = new TreeNode((tree->value), left, right);
    return newTree;
}

void setFollowPos(TreeNode *tree, set<int> *followPos, Symbol *syms) {
    if (tree) {
        setFollowPos(tree->left, followPos, syms);
        setFollowPos(tree->right, followPos, syms);
        if (tree->left == NULL && tree->right == NULL) {
            syms[tree->position-1] = Symbol(tree->value);
        } else if (tree->value == '~') {
            for (int i: tree->left->lastPos) {
                followPos[i-1] = Union(followPos[i-1], tree->right->firstPos);
            }
        } else if (tree->value == '*') {
            for (int i: tree->lastPos) {
                followPos[i-1] = Union(followPos[i-1], tree->firstPos);
            }
        }
    }
}

DFA directConstruction(TreeNode *tree) {
    // fix trees that use + and ? and then augment it
    TreeNode *fixedTree = fixTree(tree);
    TreeNode *rightNode = new TreeNode('#');
    TreeNode *augmented = new TreeNode('~', fixedTree, rightNode);
    // set positions in tree (also firstpos, lastpos and nullable)
    int max_position = augmented->setPositions();

    // create states for each position
    State pos_states[max_position];
    for (int i = 0; i < max_position; i++) {
        pos_states[i] = State(i+1);
    }

    // create symbols and followpos
    Symbol pos_symbols[max_position];
    set<int> follow_positions[max_position];
    setFollowPos(augmented, follow_positions, pos_symbols);


    DFA Dautomaton = DFA();
    // initialize Dstates to contain only the unmarked state firstpos(n0),
    // where n0 is the root of syntax tree T for (r)#;
    stack<State> Dstates;
    State ini(0);
    for (int i: augmented->firstPos) {
        ini.NFA_States.insert(pos_states[i-1]);
    }
    Dautomaton.initial_state = ini;
    Dautomaton.states.insert(ini);
    // check if initial state is accepted
    if (ini.NFA_States.find(State(max_position)) != ini.NFA_States.end()) {
        Dautomaton.final_states.insert(ini);
    }
    // set DFA symbols to positions' symbols
    for (int i = 0; i < max_position - 1; i++) {
        Dautomaton.symbols.insert(pos_symbols[i]);
    }
    Dstates.push(ini);
    int counter = 1;
    // while ( there is an unmarked state S in Dstates ) f
    while (!Dstates.empty()) {
        // mark S;
        State S = Dstates.top();
        Dstates.pop();
        // for ( each input symbol a ) f
        for (Symbol a: Dautomaton.symbols) {
            // let U be the union of followpos(p) for all p
            // in S that correspond to a;
            set<State> U;
            for (State p: S.NFA_States) {
                // check if p symbol == a
                if (pos_symbols[p.getid()-1] != a) continue;
                // insert all following positions from p states
                for (int i: follow_positions[p.getid()-1]) {
                    U.insert(pos_states[i-1]);
                }
            }

            bool inDstates = false;
            // check if U is in Dstates
            for (State t: Dautomaton.states) {
                if (t.NFA_States == U) {
                    // Dtran[S, a] = U;
                    Dautomaton.transitions.push_back(Transition(S, t, a));
                    inDstates = true;
                    break;
                }
            }
            // if ( U is not in Dstates )
            if (!inDstates) {
                // add U as an unmarked state to Dstates;
                State new_state = State(counter, U);
                counter += 1;
                Dautomaton.states.insert(new_state);
                // check if it is accepted
                if (new_state.NFA_States.find(State(max_position)) != new_state.NFA_States.end()) {
                    Dautomaton.final_states.insert(new_state);
                }
                Dstates.push(new_state);
                // Dtran[S, a] = U;
                Dautomaton.transitions.push_back(Transition(S, new_state, a));
            }
        }
    }


    delete augmented;
    return Dautomaton;
}