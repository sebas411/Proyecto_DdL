#ifndef _LEXLIB_

#define _LEXLIB_

#include <iostream>
#include <fstream>
#include <stack>
#include <set>
#include <list>
#include <unordered_map>
#include <unistd.h>

using namespace std;
// Declaración de clases
class TreeNode {
    public:
        char value;
        // parametros
        TreeNode *left;
        TreeNode *right;

        //constructores
        TreeNode(char x) {
            value = x;
            left = right = NULL;
        }
        TreeNode(char x, TreeNode *lnode, TreeNode *rnode) {
            value = x;
            left = lnode;
            right = rnode;
        }
        TreeNode(char x, TreeNode *unique) {
            value = x;
            left = unique;
            right = NULL;
        }
};

class State {
    private:
        int id;
    public:
        set<State*> NFA_States;
        State(int i) {
            id = i;
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
            ep = false;
        }
        Symbol(bool b) {
            c_id = '\0';
            id = 0;
            ep = b;
        }
        Symbol() {
            c_id = '\0';
            id = 0;
            ep = false;
        }
        string printable() {
            if (ep) {
                return "\u03b5";
            }
            return string(1, c_id);
        }
        bool operator<(const Symbol& s) const {
            return id < s.id;  
        }
};
Symbol epsilon = Symbol(true);

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

class Automata {
    public:
        set<State> states;
        set<State> final_states;
        set<Symbol> symbols;
        State initial_state;
        list<Transition> transitions;
        Automata(set<State> states_p, set<State> final_states_p) {
            symbols.insert(epsilon);
            states = states_p;
            final_states = final_states_p;
        }
        Automata() {
            symbols.insert(epsilon);
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
        void merge(Automata a) {
            for (State s : a.states) {
                states.insert(s);
            }
            for (Symbol s : a.symbols) {
                symbols.insert(s);
            }
        }

};

class NFA : public Automata {
    public:
        State thompsonFinal;

};

// funciones

bool in(string s, char c) {
    for (char ch:s) {
        if (ch == c) {
            return true;
        }
    }
    return false;
}


void printTree(TreeNode *node) {
    // prints items in depth first order
    if (node) {
        printTree(node->left);

        printTree(node->right);
        cout << node->value << " ";
    }
}


void createGraph(Automata automata) {
    ofstream MyFile("graph.dot");
    MyFile << "digraph automata {" << endl;
    MyFile << "   rankdir=LR;" << endl;
    MyFile << "    node [shape = point ]; qi;" << endl;
    MyFile << "    node [shape = doublecircle];";
    for (State finalstate:automata.final_states) {
        MyFile << " q" << finalstate.getid();
    }
	MyFile << ";" << endl << "    node [shape = circle];" << endl;
    MyFile << "    qi -> q" << automata.initial_state.getid() << endl;
    for (Transition t: automata.transitions) {
        MyFile << "    q" << t.origin_state.getid() << " -> q" << t.destiny_state.getid() << "   [label=" << t.symbol.printable() << "];" << endl;
    }
    MyFile << "}" << endl;
    MyFile.close();

    execlp("dot", "dot", "graph.dot", "-Tpng", "-o", "graph.png", NULL);
}

bool isValidRegex(const std::string& regexStr) {
    bool escaped = false;
    bool inCharacterClass = false;
    int numParentheses = 0;

    for (char c : regexStr) {
        switch (c) {
            case '\\':
                escaped = !escaped;
                break;
            case '[':
                if (!escaped) {
                    inCharacterClass = true;
                }
                escaped = false;
                break;
            case ']':
                if (inCharacterClass) {
                    inCharacterClass = false;
                }
                escaped = false;
                break;
            case '(':
                if (!inCharacterClass) {
                    numParentheses++;
                }
                escaped = false;
                break;
            case ')':
                if (!inCharacterClass) {
                    numParentheses--;
                    if (numParentheses < 0) {
                        return false; // unbalanced parentheses
                    }
                }
                escaped = false;
                break;
            case '*':
            case '+':
            case '?':
            case '|':
                if (!escaped && !inCharacterClass) {
                    return false; // invalid use of regex metacharacter
                }
                escaped = false;
                break;
            default:
                escaped = false;
                break;
        }
    }

    return numParentheses == 0; // parentheses must be balanced
}

#endif