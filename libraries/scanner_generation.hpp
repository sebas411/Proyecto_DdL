#ifndef _SCANGEN_
#define _SCANGEN_

#include "lib.hpp"
#include "lr0_generator.hpp"

void generate_scanner(vector<string> regexes, vector<string> executions, string preexecution, string postexecution, map<LR0Symbol, Action> ACTION[], map<LR0Symbol, int> GOTO[], pair<LR0Symbol, int> magnitudes[], int nStates, int nProds, vector<LR0Symbol> terminals, vector<LR0Symbol> nonterminals) {
    ofstream MyFile("scanner.cpp");
// start header
    string header = 
"#include \"libraries/lib.hpp\"\n\
#include \"libraries/lex_analyzer.hpp\"\n\
#include \"libraries/lr0_generator.hpp\"\n\
int main(int argc, char *argv[]) {\
    if (argc <= 1) {\
        cout << \"Por favor ingrese el archivo a leer\" << endl;\
        return -1;\
    }\
    string filename = argv[1];\
    ifstream t(filename);\
    if (!t.good()) {\
        cout << \"El archivo '\" << filename << \"' no se pudo abrir correctamente. :(\" << endl;\
        cout << \"Por favor revise que el archivo exista y tenga los permisos correctos\" << endl;\
        return -1;\
    }\
    string infile((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());\
    t.close();";
// end header
    MyFile << header << endl;
    MyFile << preexecution << endl;

    MyFile << "vector<string> regexes;" << endl;
    for (string regex: regexes) {
        MyFile << "regexes.push_back(\"" << regex << "\");" << endl;
    }
    MyFile << "DFA lexy = MakeLexAnalyzer(regexes);" << endl;
    MyFile <<
"bool found;\
int patt_found, curr_char = 0, next_char = 0;\
while(true) {\
    next_char = lexy.lexical_simulate(infile, &found, &patt_found, curr_char);\
    if (!found) {\
        if (!(infile[curr_char] == '\\0')) cout << \"Error en el caracter \" << curr_char << \" (\" << infile[curr_char] << \") Token no reconocido\" << endl;\
        break;\
    }"<< endl;
    int i = 0;
    for (string execution: executions) {
        MyFile << "if (patt_found == " << i << ") {" <<
            execution <<
        "} else ";
        i++;
    }
    MyFile << "{}";
MyFile << 
"    curr_char = next_char + 1;\
}";
    MyFile << postexecution << endl;
    MyFile << "map<LR0Symbol, int> GOTO[" << nStates << "];" << endl;
    MyFile << "map<LR0Symbol, Action> ACTION[" << nStates << "];" << endl;
    for (int i = 0; i < nStates; i++) {
        for (LR0Symbol terminal: terminals) {
            MyFile << "ACTION[" << i << "][LR0Symbol(Terminal,\"" << terminal.name << "\")] = Action((ActionType)" << ACTION[i][terminal].actionType << ',' << ACTION[i][terminal].to << ");" << endl;
        }
        MyFile << "ACTION[" << i << "][LR0Symbol(Accepting,\"$\")] = Action((ActionType)" << ACTION[i][LR0Symbol(Accepting, "$")].actionType << ',' << ACTION[i][LR0Symbol(Accepting, "$")].to << ");" << endl;
        for (LR0Symbol nonterminal: nonterminals) {
            MyFile << "GOTO[" << i << "][LR0Symbol(Nonterminal,\"" << nonterminal.name << "\")] = " << GOTO[i][nonterminal] << ';' << endl;
        }
    }
    MyFile << "pair<LR0Symbol, int> magnitudes[" << nProds << "];" << endl;
    for (int i = 0; i < nProds; i++) {
        MyFile << "magnitudes[" << i << "] = {LR0Symbol(Nonterminal, \"" << magnitudes[i].first.name << "\")," << magnitudes[i].second << "};" << endl;
    }
    
    MyFile << "bool result = simulateTable(ACTION, GOTO, tokens, magnitudes);\
    if (result) {\
        cout << \"SI\" << endl;\
    } else {\
        cout << \"NO\" << endl;\
    }" << endl;
    MyFile << "return 0;" << endl;
    MyFile << "}" << endl;
    MyFile.close();
}
#endif