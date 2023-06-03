#include "libraries/parser.hpp"
#include "libraries/lexer.hpp"
#include "libraries/scanner_generation.hpp"
#include "libraries/lex_analyzer.hpp"

int main(int argc, char *argv[]) {
    string parser_filename = "";
    string lexer_filename = "";
    //string test_filename = "";
    if (argc > 2) {
        parser_filename = argv[1];
        lexer_filename = argv[2];
        //test_filename = argv[3];
    } else {
        cout << "Please enter the filenames for the yapar and yalex files to build the scanner." << endl;
        return -1;
    }
    cout << "Building parser..." << endl;
    pair<vector<LR0State>, Grammar> parser_out = parser(parser_filename);
    vector<LR0State> lr0automaton = parser_out.first;
    Grammar grammar = parser_out.second;
    map<LR0Symbol, int> GOTO[lr0automaton.size()];
    map<LR0Symbol, Action> ACTION[lr0automaton.size()];

    bool success = fillTables(lr0automaton, grammar, GOTO, ACTION);
    if (!success) return -1;
    printLR0Automaton(lr0automaton);
    cout << endl << endl;
    printGOTOTable(ACTION, GOTO, grammar, lr0automaton.size());
    pair<LR0Symbol, int> magnitudes[grammar.productions.size()];
    for (int i = 0; i < grammar.productions.size(); i++) {
        magnitudes[i] = {grammar.productions[i].left, grammar.productions[i].right.size()};
    }


    cout << "Building lexer... (Takes some time)" << endl;
    tuple<vector<string>, vector<string>, string, string> lexer_out = lexer(lexer_filename);
    cout << "Generating scanner..." << endl;
    generate_scanner(get<0>(lexer_out), get<1>(lexer_out), get<2>(lexer_out), get<3>(lexer_out), ACTION, GOTO, magnitudes, lr0automaton.size(), grammar.productions.size(), grammar.terminals, grammar.nonterminals);
    
    return 0;
}