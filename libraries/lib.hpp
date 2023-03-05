#ifndef _LEXLIB_

#define _LEXLIB_

#include <iostream>
#include <fstream>
#include <stack>
#include <set>
#include <vector>
#include <unordered_map>
#include <unistd.h>

using namespace std;

// set methods
template <typename T>
set<T> Union(set<T> set1, set<T> set2) {
    set<T> *resultptr = new set<T>;
    set<T> result = *resultptr;
    for (T item: set1) {
        result.insert(item);
    }
    for(T item: set2) {
        result.insert(item);
    }
    return result;
}

template <typename T>
set<T> Substraction(set<T> set1, set<T> set2) {
    set<T> *resultptr = new set<T>;
    set<T> result = *resultptr;
    for (T item: set1) {
        if (set2.find(item) == set2.end()) {
            result.insert(item);
        }
    }
    return result;
}

template <typename T>
set<T> Intersection(set<T> set1, set<T> set2) {
    set<T> *resultptr = new set<T>;
    set<T> result = *resultptr;
    for (T item: set1) {
        if (set2.find(item) != set2.end()) {
            result.insert(item);
        }
    }
    return result;
}

// classes declaration
class TreeNode {
    public:
        char value;
        TreeNode *left;
        TreeNode *right;
        int position;
        set<int> firstPos;
        set<int> lastPos;
        bool nullable;

        TreeNode(char x) {
            value = x;
            left = right = NULL;
            position = -1;
            nullable = false;
        }
        TreeNode(char x, TreeNode *lnode, TreeNode *rnode) {
            value = x;
            left = lnode;
            right = rnode;
            position = -1;
            nullable = false;
        }
        TreeNode(char x, TreeNode *unique) {
            value = x;
            left = unique;
            right = NULL;
            position = -1;
            nullable = false;
        }

        int setPositions(int counter = 0) {
            if (left == NULL && right == NULL) {
                if (value == -1) {
                    nullable = true;
                    return counter;
                }
                nullable = false;
                position = counter + 1;
                firstPos.insert(position);
                lastPos.insert(position);
                return position;
            } else {
                if (value == '*') {
                    int rcounter = left->setPositions(counter);
                    nullable = true;
                    firstPos = left->firstPos;
                    lastPos = left->lastPos;
                    return rcounter;
                } else if (value == '|') {
                    int rcounter = left->setPositions(counter);
                    rcounter = right->setPositions(rcounter);
                    nullable = left->nullable || right->nullable;
                    firstPos = Union(left->firstPos, right->firstPos);
                    lastPos = Union(left->lastPos, right->lastPos);
                    return rcounter;
                } else if (value == '~') {
                    int rcounter = left->setPositions(counter);
                    rcounter = right->setPositions(rcounter);
                    nullable = left->nullable && right->nullable;
                    if (left->nullable) {
                        firstPos = Union(left->firstPos, right->firstPos);
                    } else {
                        firstPos = left->firstPos;
                    }
                    if (right->nullable) {
                        lastPos = Union(left->lastPos, right->lastPos);
                    } else {
                        lastPos = right->lastPos;
                    }
                    return rcounter;
                }
            }
            return -1;
        }

};

class State {
    private:
        int id;
    public:
        set<State> NFA_States;
        State(int i) {
            id = i;
        }
        State (int i, set<State> states) {
            id = i;
            NFA_States = states;
        }
        State() {
            id = 0;
        }
        int getid() {
            return id;
        }
        bool operator<(const State& s) const {
            return id < s.id;  
        }
        bool operator==(const State& s) const {
            return id == s.id;
        }
        bool operator!=(const State& s) const {
            return id != s.id;
        }
};

class Symbol {
    private:
        int id;
        char c_id;
    public:
        bool ep;
        Symbol(char c) {
            c_id = c;
            id = int(c);
            if (id == -1) {
                ep = true;
            } else {
                ep = false;
            }
        }
        Symbol() {
            c_id = '\0';
            id = 0;
            ep = false;
        }
        string printable() {
            if (ep) {
                return "\u03b5";
            } else if (c_id == '"') {
                return "\\\"";
            }
            return string(1, c_id);
        }
        bool operator<(const Symbol& s) const {
            return id < s.id;  
        }
        bool operator==(const Symbol& s) const {
            return id == s.id;  
        }
        bool operator!=(const Symbol& s) const {
            return id != s.id;  
        }
};
Symbol epsilon((char)(-1));

class Transition {
    public:
        State origin_state;
        State destiny_state;
        Symbol symbol;
        Transition(State i, State f, Symbol s) {
            origin_state = i;
            destiny_state = f;
            symbol = s;
        }
        Transition() {
            origin_state = destiny_state = State();
            symbol = Symbol();
        }
        void setDestiny(State s) {
            destiny_state = s;
        }
};

class Automaton {
    public:
        set<State> states;
        set<State> final_states;
        set<Symbol> symbols;
        State initial_state;
        vector<Transition> transitions;
        Automaton(set<State> states_p, set<State> final_states_p) {
            //symbols.insert(epsilon);
            states = states_p;
            final_states = final_states_p;
        }
        Automaton() {
            //symbols.insert(epsilon);
        }
        int max_state() {
            int max = 0;
            for (State s : states) {
                if (s.getid() > max) {
                    max = s.getid();
                }
            }
            return max;
        }
        void merge(Automaton a) {
            for (State s : a.states) {
                states.insert(s);
            }
            for (Symbol s : a.symbols) {
                symbols.insert(s);
            }
        }
        bool simulate(string input);

};

class NFA : public Automaton {
    public:
        State thompsonFinal;

        set<State> e_closure(set<State> T) {
            stack<State> state_stack;
            for (State s : T) {
                state_stack.push(s);
            }
            set<State> rstates = T;

            while (!state_stack.empty()) {
                State t = state_stack.top();
                state_stack.pop();
                for (Transition tran : transitions) {
                    if (tran.origin_state != t || tran.symbol != epsilon) continue;
                    State u = tran.destiny_state;
                    if (rstates.emplace(u).second) {
                        state_stack.push(u);
                    }
                }
            }

            return rstates;
        }

        set<State> e_closure(State s) {
            set<State> T;
            T.insert(s);
            return e_closure(T);
        }

        set<State> move(set<State> T, Symbol sym) {
            set<State> rstates;
            for (State s: T) {
                for (Transition tran : transitions) {
                    if (tran.origin_state != s || tran.symbol != sym) continue;
                    rstates.insert(tran.destiny_state);
                }
            }

            return rstates;
        }

        bool simulate(string input) {
            set<State> S = e_closure(initial_state);
            for (char c: input) {
                Symbol sym(c);
                S = e_closure(move(S,sym));
            }

            for (State s: S) {
                for (State f: final_states) {
                    if (s == f) {
                        return true;
                    }
                }
            }
            return false;
        }
};

class DFA : public Automaton {
    public:
        State move(State s, Symbol sym) {
            for (Transition t: transitions) {
                if (t.origin_state == s && t.symbol == sym) {
                    return t.destiny_state;
                }
            }
            return -1;
        }
        bool simulate(string input) {
            State s = initial_state;
            for (char c: input) {
                Symbol sym(c);
                s = move(s, sym);
            }
            if (final_states.find(s) != final_states.end()) {
                return true;
            }
            return false;
        }
};

class GroupTransition {
    public:
        Symbol symbol;
        int destGroup;
        GroupTransition(Symbol sym, int dest) {
            symbol = sym;
            destGroup = dest;
        }
        bool operator<(const GroupTransition& gt) const {
            if (symbol != gt.symbol) {
                return symbol < gt.symbol;
            }
            return destGroup < gt.destGroup;
        }
        bool operator==(const GroupTransition& gt) const {
            return (symbol == gt.symbol) && (destGroup == gt.destGroup);
        }
};

class Group {
    public:
        set<State> states;
        set<GroupTransition> transitions;
        Group(set<State> i_states) {
            states = i_states;
        }
        Group(set<GroupTransition> new_transitions, State s) {
            transitions = new_transitions;
            states.insert(s);
        }

        void addTrans(Symbol a, int dest) {
            GroupTransition trans = GroupTransition(a, dest);
            transitions.insert(trans);
        }

        int getDest(Symbol a) {
            for (GroupTransition trans: transitions) {
                if (trans.symbol == a) {
                    return trans.destGroup;
                }
            }
            return -1;
        }

};

// functions

bool in(string s, char c) {
    for (char ch:s) {
        if (ch == c) {
            return true;
        }
    }
    return false;
}


void printTree(TreeNode *node, int indent = 0) {
    // prints items in depth first order
    if (node) {
        if (indent == 0) cout << string(50, '-') << endl;
        // print right tree
        printTree(node->right, indent + 1);
        // begin print value
        cout << string(indent * 4, ' ');
        if (node->value == -1){
            cout << "ε";
        } else if (node->value == '~' && node->left != NULL) {
            cout << "◯";
        } else {
            cout << node->value;
        }
        if (node->position != -1) cout << '(' << node->position << ')';
        cout << endl;
        // end print value
        // print left tree
        printTree(node->left, indent + 1);
        if (indent == 0) cout << string(50, '-') << endl;
    }
}

enum graphType {
    nfa_graph,
    dfa_sc_graph,
    dfa_dc_graph,
    dfa_min_sc_graph,
    dfa_min_dc_graph,
    def_graph
};

void createGraph(Automaton automaton, graphType graph_type=def_graph) {
    string filename;
    switch (graph_type) {
    case nfa_graph:
        filename = "nfa.dot";
        break;
    case dfa_dc_graph:
        filename = "dfa_dc.dot";
        break;
    case dfa_sc_graph:
        filename = "dfa_sc.dot";
        break;
    case dfa_min_dc_graph:
        filename = "dfa_min_dc.dot";
        break;
    case dfa_min_sc_graph:
        filename = "dfa_min_sc.dot";
        break;
    }
    ofstream MyFile(filename);
    MyFile << "digraph automaton {" << endl;
    MyFile << "   rankdir=LR;" << endl;
    MyFile << "    node [shape = point ]; qi;" << endl;
    MyFile << "    node [shape = doublecircle];";
    for (State finalstate:automaton.final_states) {
        MyFile << " q" << finalstate.getid();
    }
	MyFile << ";" << endl << "    node [shape = circle];" << endl;
    MyFile << "    qi -> q" << automaton.initial_state.getid() << endl;
    for (Transition t: automaton.transitions) {
        MyFile << "    q" << t.origin_state.getid() << " -> q" << t.destiny_state.getid() << "   [label=\"" << t.symbol.printable() << "\"];" << endl;
    }
    MyFile << "}" << endl;
    MyFile.close();

    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
    } else if (child_pid == 0) {
        // Execute the dot command from child to avoid blocking the process
        if (graph_type == nfa_graph) {
            execlp("dot", "dot", "nfa.dot", "-Tpng", "-o", "nfa.png", NULL);
        } else if (graph_type == dfa_sc_graph) {
            execlp("dot", "dot", "dfa_sc.dot", "-Tpng", "-o", "dfa_sc.png", NULL);
        } else if (graph_type == dfa_dc_graph) {
            execlp("dot", "dot", "dfa_dc.dot", "-Tpng", "-o", "dfa_dc.png", NULL);
        } else if (graph_type == dfa_min_sc_graph) {
            execlp("dot", "dot", "dfa_min_sc.dot", "-Tpng", "-o", "dfa_min_sc.png", NULL);
        } else if (graph_type == dfa_min_dc_graph) {
            execlp("dot", "dot", "dfa_min_dc.dot", "-Tpng", "-o", "dfa_min_dc.png", NULL);
        } else {
            execlp("dot", "dot", "graph.dot", "-Tpng", "-o", "graph.png", NULL);
        }
    }

}

string subepsilon(const string str) {
    string output = "";
    bool skip = false;
    for (int i = 0; i < str.length(); i++) {
        if (skip) {
            skip = false;
            continue;
        }
        char c = str[i];
        char next = str[i+1];
        if ((c & 0xFF) == 0xCE && (next & 0xFF) == 0xB5) {
            char subs = -1;
            output += subs;
            skip = true;
        } else {
            output += c;
        }
    }
    return output;
}

void coutEpsilonSafe(string expression) {
    for (char c : expression) {
        if (c == -1) {
            cout << "\u03b5";
        } else {
            cout << c;
        }
    }
    cout << endl;
}

bool isExpValid(string expression) {
    bool valid = true;
    bool escaped = false;
    char lastc = '\0';
    int numParentheses = 0;
    int firstParenthesis = 0;
    int wrong = 0;
    int errtype = 0;
    

    for (int i = 0; i < expression.length(); i++) {
        if (!valid) break;
        char c = expression[i];
        switch (c) {
            case '\\':
                if (i + 1 >= expression.length()) {
                    wrong = i;
                    errtype = 3;
                }
                escaped = !escaped;
                break;
            case '(':
                if (!escaped) {
                    numParentheses++;
                    if (numParentheses == 1) {
                        wrong = i;
                    }
                }
                escaped = false;
                break;
            case ')':
                if (!escaped) {
                    numParentheses--;
                    if (numParentheses < 0) {
                        wrong = i;
                        valid = false;
                        errtype = 1;
                    }
                }
                escaped = false;
                break;
            case '*':
                if (!escaped) {
                    if (lastc == '\0' || lastc == '(' || lastc == '|') {
                        wrong = i;
                        valid = false;
                        errtype = 2;
                    }
                }
                escaped = false;
                break;
            case '+':
                if (!escaped) {
                    if (lastc == '\0' || lastc == '(' || lastc == '|') {
                        wrong = i;
                        valid = false;
                        errtype = 2;
                    }
                }
                escaped = false;
                break;
            case '?':
                if (!escaped) {
                    if (lastc == '\0' || lastc == '(' || lastc == '|') {
                        wrong = i;
                        valid = false;
                        errtype = 2;
                    }
                }
                escaped = false;
                break;
            case '|':
                if (!escaped) {
                    if (lastc == '\0' || lastc == '(' || lastc == '|') {
                        wrong = i;
                        valid = false;
                        errtype = 2;
                    }
                }
                escaped = false;
                break;
            default:
                escaped = false;
                break;
        }
        lastc = c;
    }
    if (valid && numParentheses != 0) {
        valid = false;
        errtype = 1;
    }
    if (!valid) {
        coutEpsilonSafe(expression);
        for (int i = 0; i < wrong; i++) cout << " ";
        cout << '^' << endl;
        if (errtype == 1) {
            if (numParentheses > 0) {
                cout << "Missing closing parenthesis" << endl;
            } else if (numParentheses < 0) {
                cout << "Missing starting parenthesis" << endl;
            } 
        } else if (errtype == 2){
            cout << "Operator not used correctly" << endl;
        }
        else {
            cout << "Error in this character" << endl;
        }
        return false;
    }
    
    return true;
}

string getResult(bool b) {
    if (b) return "Sí";
    return "No";
}
#endif