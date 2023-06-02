#include "libraries/lib.hpp"
#include "libraries/lex_analyzer.hpp"
int main(int argc, char *argv[]) {
    if (argc <= 1) {
        cout << "Por favor ingrese el archivo a leer" << endl;
        return -1;
    }
    string filename = argv[1];
    ifstream t(filename);
    if (!t.good()) {
        cout << "El archivo '" << filename << "' no se pudo abrir correctamente. :(" << endl;
        cout << "Por favor revise que el archivo exista y tenga los permisos correctos" << endl;
        return -1;
    }
    string infile((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
    t.close();

    vector<string> regexes;
    regexes.push_back("(((	|\n| ))+)");
    regexes.push_back("((((0|1|2|3|4|5|6|7|8|9))+)(\\.(((0|1|2|3|4|5|6|7|8|9))+))?(E(\\+|-)?(((0|1|2|3|4|5|6|7|8|9))+))?)");
    regexes.push_back("\\+");
    regexes.push_back("-");
    regexes.push_back("\\*");
    regexes.push_back("/");
    regexes.push_back("\\(");
    regexes.push_back("\\)");
    DFA lexy = MakeLexAnalyzer(regexes);
    bool found;
    int patt_found, curr_char = 0, next_char = 0;
    while(true) {
        next_char = lexy.lexical_simulate(infile, &found, &patt_found, curr_char);
        if (!found) {
            if (!(infile[curr_char] == '\0'))
                cout << "Error en el caracter " << curr_char << " (" << infile[curr_char] << ") Token no reconocido" << endl;
            break;
        }
        if (patt_found == 0) {
            cout << "ws";
        } else if (patt_found == 1) {
            cout << "NUMBER";
        } else if (patt_found == 2) {
            cout << "+";
        } else if (patt_found == 3) {
            cout << "-";
        } else if (patt_found == 4) {
            cout << "*";
        } else if (patt_found == 5) {
            cout << "/";
        } else if (patt_found == 6) {
            cout << "(";
        } else if (patt_found == 7) {
            cout << ")";
        } else {}   
        curr_char = next_char + 1;
    }
    return 0;
}
