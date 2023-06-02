#include "lib.hpp"
#include <map>

enum SymbolType {
    Terminal,
    Nonterminal,
    Epsilon,
    Accepting,
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
        bool isAccepting;
        set<pair<Production, int>> items;
        map<LR0Symbol, LR0State> transitions;

        LR0State(int id, set<pair<Production, int>> items) : id(id), items(items), isAccepting(false) {}

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
            LR0Symbol left(Nonterminal, startSym.name + "''");
            Production augProd(left, right);
            nonterminals.push_back(left);
            //productions.insert(productions.begin(), augProd);
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

enum ActionType {
    NoAction,
    Reduce,
    Shift,
    Accept
};

class Action {
public:
    ActionType actionType;
    int to;
    Action(ActionType actionType, int to) : actionType(actionType), to(to) {}
    Action(): actionType(NoAction), to(-1) {}
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

Grammar eliminateLeftRecursion(Grammar originalGrammar) {
    vector<Production> newProductions;
    vector<LR0Symbol> newNonTerms = originalGrammar.nonterminals;

    for (LR0Symbol nonterminal: originalGrammar.nonterminals) {
        vector<vector<LR0Symbol>> alphas;
        vector<vector<LR0Symbol>> betas;
        LR0Symbol newnonterminal(Nonterminal, nonterminal.name + '\'');
        bool hasLeft = false;

        for (Production production: originalGrammar.getRules(nonterminal)) {
            vector<LR0Symbol> prod_right = production.right;
            if (prod_right[0] == nonterminal) {
                // E -> E + T | T

                // E -> T E'
                // E' -> + T E' | e
                vector<LR0Symbol> alpha(prod_right.begin() + 1, prod_right.end());
                alpha.push_back(newnonterminal);
                alphas.push_back(alpha);
                hasLeft = true;
            } else {
                prod_right.push_back(newnonterminal);
                betas.push_back(prod_right);
            }
        }
        if (alphas.size() > 0) {
            for (vector<LR0Symbol> alpha: alphas) {
                Production production;
                production.left = newnonterminal;
                production.right = alpha;
                newProductions.push_back(production);
            }
            for (vector<LR0Symbol> beta: betas) {
                Production production;
                production.left = nonterminal;
                production.right = beta;
                newProductions.push_back(production);
            }
            newProductions.push_back(Production(LR0Symbol(Nonterminal, nonterminal.name + '\''), {LR0Symbol(Epsilon, "ε")}));
            newNonTerms.push_back(newnonterminal);
        } else {
            for (Production production: originalGrammar.getRules(nonterminal)) {
                newProductions.push_back(production);
            }
        }
    }

    Grammar new_grammar(originalGrammar.terminals, newNonTerms, originalGrammar.startSym, newProductions);
    return new_grammar;
    
}

bool isLeftRecursive(Grammar grammar, LR0Symbol X) {
    for (Production production: grammar.getRules(X)) {
        if (production.right[0] == X)
            return true;
    }
    return false;
}

set<LR0Symbol> computeFirst(Grammar grammar, LR0Symbol X) {
    set<LR0Symbol> FIRST_X;
    LR0Symbol EPSILON(Epsilon, "ε");

    if (grammar.isNonterminal(X)) {
        for (Production production : grammar.getRules(X)) {
            LR0Symbol Y1 = production.right[0];
            
            if (grammar.isNonterminal(Y1)) {
                set<LR0Symbol> FIRST_Y1 = computeFirst(grammar, Y1);
                FIRST_X.insert(FIRST_Y1.begin(), FIRST_Y1.end());
                
                int i = 1;
                while (i < production.right.size() && FIRST_Y1.count(EPSILON) > 0) {
                    LR0Symbol Yi = production.right[i];
                    set<LR0Symbol> FIRST_Yi = computeFirst(grammar, Yi);
                    FIRST_X.insert(FIRST_Yi.begin(), FIRST_Yi.end());
                    FIRST_Y1.erase(EPSILON);
                    FIRST_X.erase(EPSILON);
                    FIRST_X.insert(FIRST_Yi.begin(), FIRST_Yi.end());
                    i++;
                }
                
                if (i == production.right.size() && FIRST_Y1.count(EPSILON) > 0) {
                    FIRST_X.insert(EPSILON);
                }
            } else {
                FIRST_X.insert(Y1);
            }
        }
    } else {
        FIRST_X.insert(X);
    }

    return FIRST_X;
}

set<LR0Symbol> computeFollow(Grammar grammar, LR0Symbol X) {
    set<LR0Symbol> FOLLOW_X;
    LR0Symbol endMarker(Accepting, "$");
    LR0Symbol EPSILON(Epsilon, "ε");

    if (X == grammar.startSym) {
        FOLLOW_X.insert(endMarker);  // Insert your representation of the end-of-input marker here
    }

    for (Production production : grammar.productions) {
        vector<LR0Symbol> symbols = production.right;

        for (int i = 0; i < symbols.size(); ++i) {
            LR0Symbol symbol = symbols[i];

            if (symbol == X) {
                if (i < symbols.size() - 1) {
                    LR0Symbol nextSymbol = symbols[i + 1];

                    if (grammar.isNonterminal(nextSymbol)) {
                        set<LR0Symbol> FIRST_next = computeFirst(grammar, nextSymbol);

                        for (LR0Symbol FIRST_Symbol: FIRST_next) {
                            if (FIRST_Symbol != EPSILON) {
                                FOLLOW_X.insert(FIRST_Symbol);
                            }
                        }

                        if (FIRST_next.count(EPSILON) > 0 && production.left != X) {
                            set<LR0Symbol> FOLLOW_A = computeFollow(grammar, production.left);
                            FOLLOW_X.insert(FOLLOW_A.begin(), FOLLOW_A.end());
                        }
                    } else {
                        FOLLOW_X.insert(nextSymbol);
                    }
                } else {
                    if (production.left != X) {
                        set<LR0Symbol> FOLLOW_A = computeFollow(grammar, production.left);
                        FOLLOW_X.insert(FOLLOW_A.begin(), FOLLOW_A.end());
                    }
                }
            }
        }
    }

    return FOLLOW_X;
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

    for (auto it = states.begin(); it != states.end(); it++) {
        LR0State & state = *it;
        for (auto item: state.items) {
            Production production = item.first;
            int position = item.second;
            if (production == grammar.getStart() && position == 1) {
                state.isAccepting = true;
            }
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
            cout << production.left.name << " ->";
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
        if (state.transitions.size() > 0)
            cout << "  Transitions:" << endl;
        for (pair<LR0Symbol, LR0State> transition : state.transitions) {
            LR0Symbol symbol = transition.first;
            cout << "  " << symbol.name << " -> State " << transition.second.id << endl;
        }
        if (state.isAccepting)
            cout << "  $ -> accept :)" << endl;
        cout << endl;
    }
}

void fillTables(vector<LR0State> states, Grammar grammar, map<LR0Symbol, int> GOTO[], map<LR0Symbol, Action> ACTION[]) {
    int nStates = states.size();
    Grammar nonLeftRecursiveGrammar = eliminateLeftRecursion(grammar);

    // Fill with default values
    for (int i = 0; i < nStates; i++) {
        for (LR0Symbol terminal: grammar.terminals) {
            ACTION[i][terminal] = Action(NoAction, -1);
        }
        for (LR0Symbol nonterminal: grammar.nonterminals) {
            GOTO[i][nonterminal] = -1;
        }
    }

    for (int i = 0; i < nStates; i++) {
        LR0State state = states[i];

        // Fill GOTO and shift ACTIONs
        for (pair<LR0Symbol, LR0State> transition: state.transitions) {
            if (transition.first.type == Terminal) {
                ACTION[i][transition.first] = Action(Shift, transition.second.id);
            } else if (transition.first.type == Nonterminal) {
                GOTO[i][transition.first] = transition.second.id;
            }
        }

        // Add acceptance
        if (state.isAccepting) {
            ACTION[i][LR0Symbol(Accepting, "$")] = Action(Accept, 0);
        }

        // Fill reduce ACTIONs

        for (pair<Production, int> item: state.items) {
            Production production = item.first;
            int position = item.second;
            if (position == production.right.size()) {
                int reduce_to = 0;
                for (int j = 0; j < grammar.productions.size(); j++) {
                    if (production == grammar.productions[j]) {
                        reduce_to = j + 1;
                        break;
                    }
                }
                set<LR0Symbol> FOLLOW_A = computeFollow(nonLeftRecursiveGrammar, production.left);
                if (isLeftRecursive(grammar, production.left)) {
                    LR0Symbol leftPrime(Nonterminal, production.left.name + '\'');
                    for (Production production2: nonLeftRecursiveGrammar.getRules(leftPrime)) {
                        if (production2.right[0] != LR0Symbol(Epsilon, "ε")) {
                            FOLLOW_A.insert(production2.right[0]);
                        }
                    }
                }
                for (LR0Symbol symbol: FOLLOW_A) {
                    ACTION[i][symbol] = Action(Reduce, reduce_to);
                }
            }

        }
    }

}
