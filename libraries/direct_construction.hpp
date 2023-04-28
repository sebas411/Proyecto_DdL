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

void setFollowPos(TreeNode *tree, set<int> *followPos, Symbol *syms, set<State> *accepting_positions) {
    if (tree) {
        setFollowPos(tree->left, followPos, syms, accepting_positions);
        setFollowPos(tree->right, followPos, syms, accepting_positions);
        if (tree->left == NULL && tree->right == NULL) {
            syms[tree->position-1] = Symbol(tree->value);
            if (tree->accepting_pat >= 0) {
                State s(tree->position);
                s.accepting_pattern = tree->accepting_pat;
                accepting_positions->insert(s);
            }
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


DFA directConstruction(vector<TreeNode*> trees) {
    int current_accepting = 0;
    TreeNode *initial = trees[0];
    // fix trees that use + and ? and then augment it
    TreeNode *ini_fixedTree = fixTree(initial);
    TreeNode *ini_rightNode = new TreeNode('#');
    ini_rightNode->accepting_pat = current_accepting;
    TreeNode *final_tree = new TreeNode('~', ini_fixedTree, ini_rightNode);

    for (int i = 1; i < trees.size(); i++) {
        current_accepting++;
        TreeNode *currTree = trees[i];
        TreeNode *fixedTree = fixTree(currTree);
        TreeNode *rightNode = new TreeNode('#');
        rightNode->accepting_pat = current_accepting;
        TreeNode *augmented = new TreeNode('~', fixedTree, rightNode);

        final_tree = new TreeNode('|', final_tree, augmented);
    }
    // set positions in tree (also firstpos, lastpos and nullable)
    int max_position = final_tree->setPositions();

    // create states for each position
    State pos_states[max_position];
    for (int i = 0; i < max_position; i++) {
        pos_states[i] = State(i+1);
    }

    // create symbols and followpos
    Symbol pos_symbols[max_position];
    set<int> follow_positions[max_position];
    set<State> accepting_positions;
    setFollowPos(final_tree, follow_positions, pos_symbols, &accepting_positions);
    int accept_pattern;

    DFA Dautomaton = DFA();
    // initialize Dstates to contain only the unmarked state firstpos(n0),
    // where n0 is the root of syntax tree T for (r)#;
    stack<State> Dstates;
    State ini(0);
    for (int i: final_tree->firstPos) {
        ini.NFA_States.insert(pos_states[i-1]);
    }
    Dautomaton.initial_state = ini;
    Dautomaton.states.insert(ini);
    // check if initial state is accepted
    
    if (isFinalCheck(ini, accepting_positions, &accept_pattern)) {
        ini.accepting_pattern = accept_pattern;
        Dautomaton.final_states.insert(ini);
    }
    // set DFA symbols to positions' symbols
    for (int i = 0; i < max_position; i++) {
        if (accepting_positions.find(State(i+1)) == accepting_positions.end())
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
                if (isFinalCheck(new_state, accepting_positions, &accept_pattern)) {
                    new_state.accepting_pattern = accept_pattern;
                    Dautomaton.final_states.insert(new_state);
                }
                Dstates.push(new_state);
                // Dtran[S, a] = U;
                Dautomaton.transitions.push_back(Transition(S, new_state, a));
            }
        }
    }

    delete final_tree;
    return Dautomaton;
}

DFA directConstruction(TreeNode* tree) {
    vector<TreeNode*> trees;
    trees.push_back(tree);
    return directConstruction(trees);
}