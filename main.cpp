#include "libraries/parser.hpp"
#include "libraries/lexer.hpp"


int main(int argc, char *argv[]) {
    string parser_filename = "";
    string lexer_filename = "";
    if (argc > 2) {
        parser_filename = argv[1];
        lexer_filename = argv[2];
    } else {
        cout << "Please enter the filenames for the yapar and yalex files to build the scanner." << endl;
        return -1;
    }
    cout << "Building parser..." << endl;
    parser(parser_filename);
    cout << "Building lexer... (Takes some time)" << endl;
    lexer(lexer_filename);
    cout << "Executed successfully" << endl;
    return 0;
}