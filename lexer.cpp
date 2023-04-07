#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>

#include "libraries/lib.hpp"
#include "libraries/thompson.hpp"
#include "libraries/shunting_yard.hpp"
#include "libraries/subset_construction.hpp"
#include "libraries/direct_construction.hpp"
#include "libraries/minimization.hpp"

string handleRegDef(string regDef) {
    return "";
}

// main
int main(int argc, char *argv[]){
    string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        cout << "Por favor ingrese el nombre del archivo .yal" << endl;
        return -1;
    }
    // Store the contents of file in str variable
    ifstream t(filename);
    if (!t.good()) {
        cout << "El archivo '" << filename << "' no se pudo abrir correctamente. :(" << endl;
        cout << "Por favor revise que el archivo exista y tenga los permisos correctos" << endl;
        return -1;
    }
    string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
    t.close();
    // end file handling

    // begin declaration of useful regular expressions
    string letters = "(A|a";
    for (char i = 1; i < 26; i++) {
        letters += '|';
        letters += (i+65);
        letters += '|';
        letters += (i+97);
    }
    letters += ")+";

    TreeNode *comment_tree = postfixToTree(shuntingYard("\\(\\*.*\\*\\)"));
    TreeNode *ws_tree = postfixToTree(shuntingYard("( |\n|\t)*"));
    TreeNode *let_tree = postfixToTree(shuntingYard("let "));
    TreeNode *name_tree = postfixToTree(shuntingYard(letters));
    TreeNode *range_tree = postfixToTree(shuntingYard("'.'-'.'"));
    TreeNode *single_char_tree = postfixToTree(shuntingYard("'.'"));
    TreeNode *string_tree = postfixToTree(shuntingYard("\".+\""));
    TreeNode *capture_tree = postfixToTree(shuntingYard("[.+]"));
    TreeNode *anti_capture_tree = postfixToTree(shuntingYard("[^.+]"));
    TreeNode *rule_tree = postfixToTree(shuntingYard("rule"));

    DFA comment = directConstruction(comment_tree);
    DFA ws = directConstruction(ws_tree);
    DFA let_A = directConstruction(let_tree);
    DFA name = directConstruction(name_tree);
    DFA range = directConstruction(range_tree);
    DFA single_char = directConstruction(single_char_tree);
    DFA string_A = directConstruction(string_tree);
    DFA capture = directConstruction(capture_tree);
    DFA anti_capture = directConstruction(anti_capture_tree);
    DFA rule = directConstruction(rule_tree);

    /**
     * W -> \(\*.*\*\)| |\n|\t|()
     * F -> WLWRW
     * L -> letWNW=WEWL|()
     * R
     */

    //file reading
    int current_char = 0;
    bool in_let = false;
    while (!in_let) {
        pair<bool, int> ws_status = ws.simulate(str, current_char);
        if (ws_status.first) current_char = ws_status.second + 1;
        pair<bool, int> comment_status = comment.simulate(str,current_char);
        if (comment_status.first) current_char = comment_status.second + 1;
        if (!(ws_status.first || comment_status.first)) in_let = true;
    }

    while (in_let) {
        pair<bool, int> let_status = let_A.simulate(str, current_char);
        if (!let_status.first) {
            in_let = false;
            break;
        }
        current_char = let_status.second + 1;
        pair<bool, int> status = ws.simulate(str, current_char);
        if (status.first)
        current_char = status.second + 1; // move after let
        int name_last_char = name.simulate(str, current_char).second + 1;
        string let_name = str.substr(current_char, name_last_char - current_char);
        current_char = name_last_char;
        cout << "Def_name: " << let_name << endl;
        current_char = ws.simulate(str, current_char).second + 1; // move after name
        if (str[current_char] != '=') {
            cout << "error en lectura" << endl;
            break;
        }
        current_char++; // move after =

        current_char = ws.simulate(str, current_char).second + 1; // move after =

        string reg_def = "";       
        while(!(let_A.simulate(str, ws.simulate(str, current_char).second+1).first || rule.simulate(str, ws.simulate(str, current_char).second+1).first)) {
            char c = str[current_char];
            reg_def += c;
            current_char++;
        }
        cout << " Reg_def: " << reg_def << endl << endl;
        string fixed_reg_def = handleRegDef(reg_def);
        current_char = ws.simulate(str, current_char).second + 1; // move to next let or rule

    }
    //cout << str[current_char] << str[current_char + 1] << str[current_char + 2] << str[current_char + 3] << str[current_char + 4] << endl;
    // while (current_char < str.length()) {

    // }


    
    return 0;
}