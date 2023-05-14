#include "lib.hpp"
#include <map>

enum SymbolType {
    Terminal,
    Nonterminal,
    Undef
};

class LR0Symbol {
public:
    SymbolType type;
    string name;

    LR0Symbol(SymbolType type, string name) : type(type), name(name) {}
    
    LR0Symbol() : type(Undef) {}

    bool operator==(const LR0Symbol& s) const {
        return (name == s.name && type == s.type);
    }

    bool operator!=(const LR0Symbol& s) const {
        return !(name == s.name && type == s.type);
    }

    bool operator<(const LR0Symbol& other) const {
        return tie(type, name) < tie(other.type, other.name);
    }
};

class Production {
public:
    LR0Symbol left;
    vector<LR0Symbol> right;

    Production(LR0Symbol left, vector<LR0Symbol> right) : left(left), right(right) {}

    Production() {}

    bool operator==(const Production& other) const {
        return ((left == other.left) && (right == other.right));
    }

    bool operator<(const Production& other) const {
        return tie(left, right) < tie(other.left, other.right);
    }
};

class LR0State {
    public:
        int id;
        set<pair<Production, int>> items;
        map<LR0Symbol, LR0State> transitions;

        LR0State(int id, set<pair<Production, int>> items) : id(id), items(items) {}

        LR0State() : id(-1) {}
};

class Grammar {
public:
    vector<LR0Symbol> terminals;
    vector<LR0Symbol> nonterminals;
    LR0Symbol startSym;
    vector<Production> productions;
    Production startProd;

    Grammar(vector<LR0Symbol> terminals, vector<LR0Symbol> nonterminals, LR0Symbol startSym,
        vector<Production> productions) : terminals(terminals), nonterminals(nonterminals),
        startSym(startSym), productions(productions) {
            vector<LR0Symbol> right = {startSym};
            LR0Symbol left(Nonterminal, startSym.name + "'");
            Production augProd(left, right);
            nonterminals.push_back(left);
            productions.insert(productions.begin(), augProd);
            startProd = augProd;

    }

    bool isNonterminal(LR0Symbol symbol) const {
        return symbol.type == Nonterminal;
    }

    vector<Production> getRules(LR0Symbol symbol) const {
        vector<Production> rules;
        for (Production production : productions) {
            if (production.left == symbol) {
                rules.push_back(production);
            }
        }
        return rules;
    }

    vector<LR0Symbol> getSymbols() const {
        vector<LR0Symbol> symbols;
        symbols.reserve(terminals.size() + nonterminals.size());
        symbols.insert(symbols.end(), terminals.begin(), terminals.end());
        symbols.insert(symbols.end(), nonterminals.begin(), nonterminals.end());
        return symbols;
    }

    Production getStart() const {
        return startProd;
    }

};

set<pair<Production, int>> closure(set<pair<Production, int>> items, Grammar grammar) {
    set<pair<Production, int>> closure = items;
    bool changed = true;
    while (changed) {
        changed = false;
        for (pair<Production, int> item : closure) {
            Production production = item.first;
            int position = item.second;
            if (position < production.right.size() && grammar.isNonterminal(production.right[position])) {
                LR0Symbol symbol = production.right[position];
                for (Production rule : grammar.getRules(symbol)) {
                    pair<Production, int> newItem = make_pair(rule, 0);
                    if (closure.find(newItem) == closure.end()) {
                        closure.insert(newItem);
                        changed = true;
                    }
                }
            }
        }
    }
    return closure;
}

set<pair<Production, int>> gotoSet(set<pair<Production, int>> items, LR0Symbol symbol, Grammar grammar) {
    set<pair<Production, int>> gotoItems;
    for (auto item : items) {
        Production production = item.first;
        int position = item.second;
        if (position < production.right.size() && production.right[position] == symbol) {
            gotoItems.insert(make_pair(production, position + 1));
        }
    }
    return closure(gotoItems, grammar);
}

vector<LR0State> generateLR0Automaton(Grammar grammar) {
    vector<LR0State> states;
    set<pair<Production, int>> startItems;

    startItems.insert(make_pair(grammar.getStart(), 0));
    LR0State startState = {0, closure(startItems, grammar)};
    states.push_back(startState);
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < states.size(); i++) {
            LR0State state = states[i];
            for (LR0Symbol symbol: grammar.getSymbols()) {
                set<pair<Production, int>> gotoItems = gotoSet(state.items, symbol, grammar);
                if (gotoItems.size() > 0) {
                    int j = -1;
                    for (int k = 0; k < states.size(); k++) {
                        if (states[k].items == gotoItems) {
                            j = k;
                            break;
                        }
                    }
                    if (j == -1) {
                        LR0State newState = {(int)states.size(), gotoItems};
                        states.push_back(newState);
                        j = states.size() - 1;
                        changed = true;
                    }
                    LR0State selectedState = states[j];
                    state.transitions[symbol] = selectedState;
                }
            }
            states[i] = state;
        }
    }
    return states;
}

void printLR0Automaton(vector<LR0State> states) {
    for (auto state : states) {
        cout << "State " << state.id << ":" << endl;
        for (pair<Production, int> item : state.items) {
            Production production = item.first;
            int position = item.second;
            cout << "  " << production.left.name << " ->";
            for (int i = 0; i < production.right.size(); i++) {
                if (i == position) {
                    cout << " .";
                }
                cout << " " << production.right[i].name;
            }
            if (position == production.right.size()) {
                cout << " .";
            }
            cout << endl;
        }
        cout << endl;
        for (pair<LR0Symbol, LR0State> transition : state.transitions) {
            LR0Symbol symbol = transition.first;
            cout << "  " << symbol.name << " -> State " << transition.second.id << endl;
        }
        cout << endl;
    }
}