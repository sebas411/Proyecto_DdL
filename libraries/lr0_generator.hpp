#include "lib.hpp"
#include <map>

enum SymbolType {
    Terminal,
    Nonterminal
};

class LR0Symbol {
public:
    SymbolType type;
    string name;

    LR0Symbol(SymbolType type, string name) : type(type), name(name) {}

    bool operator==(const LR0Symbol& s) const {
        return (name == s.name && type == s.type);
    }
};

class Production {
public:
    LR0Symbol left;
    vector<LR0Symbol> right;

    Production(LR0Symbol left, vector<LR0Symbol> right) : left(left), right(right) {}
};

class LR0State {
    public:
        set<pair<Production, int>> items;
        map<LR0Symbol, set<pair<Production, int>>> transitions;
};

class Grammar {
public:
    vector<LR0Symbol> terminals;
    vector<LR0Symbol> nonterminals;
    LR0Symbol start_symbol;
    vector<Production> productions;

    Grammar(vector<LR0Symbol> terminals, vector<LR0Symbol> nonterminals, LR0Symbol start_symbol,
        vector<Production> productions) : terminals(terminals), nonterminals(nonterminals),
        start_symbol(start_symbol), productions(productions) {}

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