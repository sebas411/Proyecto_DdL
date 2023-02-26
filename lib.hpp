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
            if (id == -1) {
                ep = true;
            } else {
                ep = false;
            }
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
            } else if (c_id == '"') {
                return "\\\"";
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
        MyFile << "    q" << t.origin_state.getid() << " -> q" << t.destiny_state.getid() << "   [label=\"" << t.symbol.printable() << "\"];" << endl;
    }
    MyFile << "}" << endl;
    MyFile.close();

    execlp("dot", "dot", "graph.dot", "-Tpng", "-o", "graph.png", NULL);
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
#endif