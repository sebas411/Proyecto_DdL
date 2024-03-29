#ifndef _PARSER_
#define _PARSER_

#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>

#include "lib.hpp"
#include "thompson.hpp"
#include "shunting_yard.hpp"
#include "direct_construction.hpp"
#include "minimization.hpp"
#include "lr0_generator.hpp"

// main
pair<vector<LR0State>, Grammar> parser(string filename) {
    // Store the contents of file in str variable
    ifstream t(filename);
    if (!t.good()) {
        cout << "El archivo '" << filename << "' no se pudo abrir correctamente. :(" << endl;
        cout << "Por favor revise que el archivo exista y tenga los permisos correctos" << endl;
        return {{}, Grammar({}, {}, LR0Symbol(), {})};
    }
    string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
    t.close();
    // end file handling

    // begin declaration of useful regular expressions
    string Upper = "(A";
    string Lower = "(a";
    for (char i = 1; i < 26; i++) {
        Upper += '|';
        Upper += (i+65);
        Lower += '|';
        Lower += (i+97);
    }
    Upper += ")+";
    Lower += ")+";
    TreeNode *ws_tree = postfixToTree(shuntingYard("( |\n|\t)*"));
    TreeNode *terminal_tree = postfixToTree(shuntingYard(Upper));
    TreeNode *nonterminal_tree = postfixToTree(shuntingYard(Lower));
    TreeNode *token_tree = postfixToTree(shuntingYard("%token"));
    TreeNode *ignore_tree = postfixToTree(shuntingYard("IGNORE"));
    
    DFA ws = directConstruction(ws_tree);
    DFA terminal = directConstruction(terminal_tree);
    DFA nonterminal = directConstruction(nonterminal_tree);
    DFA token = directConstruction(token_tree);
    DFA ignore = directConstruction(ignore_tree);
    terminal = minimize(terminal);
    nonterminal = minimize(nonterminal);

    int current_char = 0;
    bool in_tok = false;
    pair<bool, int> status;
    while (!in_tok) {
        status = token.simulate(str, current_char);
        if (status.first) {
            in_tok = true;
            break;
        }
        current_char++;
    }
    
    bool isStartSet = false;
    vector<LR0Symbol> terminals;
    vector<LR0Symbol> nonterminals;
    vector<LR0Symbol> ignoredtok;
    vector<Production> productions;
    LR0Symbol startSym;


    while (in_tok) {
        current_char = ws.simulate(str, token.simulate(str, current_char).second + 1).second + 1;
        while (true) {
            status = terminal.simulate(str, current_char);
            if (!status.first) break;
            string term = str.substr(current_char, status.second - current_char + 1);
            if (term == "IGNORE") break;
            terminals.push_back(LR0Symbol(Terminal, term));
            current_char = ws.simulate(str, status.second + 1).second + 1;
        }
        if (nonterminal.simulate(str, current_char).first || ignore.simulate(str, current_char).first)
            in_tok = false;
    }
    while (true) {
        status = ignore.simulate(str, current_char);
        if (status.first) {
            current_char = ws.simulate(str, status.second + 1).second + 1;
            status = terminal.simulate(str, current_char);
            string term = str.substr(current_char, status.second - current_char + 1);
            ignoredtok.push_back(LR0Symbol(Terminal, term));
            terminals.erase(remove(terminals.begin(), terminals.end(), LR0Symbol(Terminal, term)), terminals.end());
            current_char = ws.simulate(str, status.second + 1).second + 1;
        } else {
            break;
        }
    }
    bool in_prods = true;
    while (in_prods) {
        status = ws.simulate(str, current_char);
        if (status.first)
            current_char = status.second + 1;

        status = nonterminal.simulate(str, current_char);
        string leftStr = str.substr(current_char, status.second - current_char + 1);
        LR0Symbol left = LR0Symbol(Nonterminal, leftStr);
        nonterminals.push_back(left);
        if (!isStartSet) {
            startSym = left;
            isStartSet = true;
        }

        vector<LR0Symbol> right;

        current_char = status.second + 2;
        while (true) {
            status = ws.simulate(str, current_char);
            if (status.first)
                current_char = status.second + 1;
            status = nonterminal.simulate(str, current_char);
            if (status.first) {
                string symb = str.substr(current_char, status.second - current_char + 1);
                right.push_back(LR0Symbol(Nonterminal, symb));
                current_char = status.second + 1;
                continue;
            }

            status = terminal.simulate(str, current_char);
            if (status.first) {
                string symb = str.substr(current_char, status.second - current_char + 1);
                right.push_back(LR0Symbol(Terminal, symb));
                current_char = status.second + 1;
                continue;
            }

            if (str[current_char] == '|') {
                Production prod(left, right);
                productions.push_back(prod);
                right = {};
                current_char++;
                continue;
            }
            if (str[current_char] == ';') {
                Production prod(left, right);
                productions.push_back(prod);
                right = {};
                current_char++;
                break;
            }
        }

        status = nonterminal.simulate(str, ws.simulate(str, current_char).second + 1);
        if (!status.first) {
            break;
        }
    }
    Grammar grammar(terminals, nonterminals, startSym, productions);
    vector<LR0State> aut_states = generateLR0Automaton(grammar);
    //printLR0Automaton(aut_states);
    return {aut_states, grammar};
}
#endif