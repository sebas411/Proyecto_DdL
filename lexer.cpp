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
#include "libraries/scanner_generation.hpp"

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
    cout << "Generating automatons for yalex recognition..." << endl;
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
    TreeNode *single_char_tree = postfixToTree(shuntingYard("'\\\\?.'"));
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

    // some regular expressions needed to be minimized because otherwise they took to much time
    cout << "Minimizing some automatons for rule reading..." << endl;
    single_char = minimize(single_char);
    comment = minimize(comment);
    string_A = minimize(string_A);

    //file reading
    cout << "Reading let definitions..." << endl;
    string first_execution = "";
    bool is_first_execution_set = false;
    int current_char = 0;
    bool in_let = false;
    while (!in_let) {
        pair<bool, int> ws_status = ws.simulate(str, current_char);
        if (ws_status.first) current_char = ws_status.second + 1;
        pair<bool, int> comment_status = comment.simulate(str,current_char);
        if (!is_first_execution_set && comment_status.first) {
            int distance = comment_status.second - current_char;
            string code_test = str.substr(current_char + 2, distance - 3);
            first_execution = code_test;
        }
        if (comment_status.first) current_char = comment_status.second + 1;
        if (!(ws_status.first || comment_status.first)) in_let = true;
    }
    string ops = "?|()*+";

    vector<pair<DFA,string>> reg_defs;
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
        int reg_def_current_char = 0;
        string fixed_reg_def = "(";
        while (reg_def_current_char < reg_def.length()) {
            status = capture.simulate(reg_def, reg_def_current_char);
            if (status.first) {
                string capture_str = "(";
                int capture_last_char = status.second;
                set<char> captures;
                reg_def_current_char ++;
                while (reg_def_current_char < capture_last_char) {
                    status = range.simulate(reg_def, reg_def_current_char);
                    if (status.first) {
                        char start = reg_def[reg_def_current_char + 1];
                        char end = reg_def[reg_def_current_char + 5];
                        for (char c = start; c <= end; c++) {
                            captures.insert(c);
                        }
                        reg_def_current_char = status.second + 1;
                        continue;
                    }
                    status = single_char.simulate(reg_def, reg_def_current_char);
                    if (status.first) {
                        char c = reg_def[reg_def_current_char + 1];
                        if (c == '\\') {
                            c = reg_def[reg_def_current_char + 2];
                            if (c == 'n') c = '\n';
                            if (c == 't') c = '\t';
                        }
                        captures.insert(c);
                        reg_def_current_char = status.second + 1;
                        continue;
                    }
                    status = string_A.simulate(reg_def, reg_def_current_char);
                    if (status.first) {
                        int start = reg_def_current_char + 1;
                        for (int string_current_char = start; string_current_char < status.second; string_current_char++) {
                            char c = reg_def[string_current_char];
                            if (c == '\\') {
                                c = reg_def[string_current_char + 1];
                                if (c == 'n') c = '\n';
                                if (c == 't') c = '\t';
                                if (c == 's') c = ' ';
                                string_current_char++;
                            }
                            captures.insert(c);
                        }
                        reg_def_current_char = status.second + 1;
                        continue;
                    }
                    reg_def_current_char++;
                }
                bool first_capture = true;
                for (char c: captures) {
                    if (first_capture) first_capture = false;
                    else capture_str += '|';
                    if (in(ops, c)) capture_str += '\\';
                    capture_str += c;
                }
                capture_str += ')';
                fixed_reg_def += capture_str;
                reg_def_current_char = capture_last_char + 1;
                continue;
            }
            if (in(ops, reg_def[reg_def_current_char])) {
                fixed_reg_def += reg_def[reg_def_current_char];
                reg_def_current_char++;
                continue;
            }
            status = single_char.simulate(reg_def, reg_def_current_char);
            if (status.first) {
                char c = reg_def[reg_def_current_char + 1];
                if (c == '\\') {
                    c = reg_def[reg_def_current_char + 2];
                    if (c == 'n') c = '\n';
                    if (c == 't') c = '\t';
                }
                if (c == '.') fixed_reg_def += '\\';
                fixed_reg_def += c;
                reg_def_current_char = status.second + 1;
                continue;
            }
            status = string_A.simulate(reg_def, reg_def_current_char);
            if (status.first) {
                int distance = status.second - reg_def_current_char;
                fixed_reg_def += reg_def.substr(reg_def_current_char + 1, distance - 1);
                reg_def_current_char = status.second + 1;
                continue;
            }
            status = name.simulate(reg_def, reg_def_current_char);
            if (status.first) {
                int name_last_char = status.second;
                int matching_name = -1;
                int matching_length = 0;
                for (int i = 0; i < reg_defs.size(); i++) {
                    pair<DFA, string> c_reg_def = reg_defs[i];
                    status = c_reg_def.first.simulate(reg_def, reg_def_current_char);
                    if (status.first) {
                        if (status.second > matching_length) {
                            matching_name = i;
                            matching_length = status.second;
                        }
                    }
                }
                if (matching_name != -1) {
                    pair<DFA, string> matching_reg_def = reg_defs[matching_name];
                    fixed_reg_def += matching_reg_def.second;
                    reg_def_current_char = name_last_char + 1;
                    continue;
                }
            }
            reg_def_current_char++;
        }
        fixed_reg_def += ')';
        TreeNode *reg_def_tree = postfixToTree(shuntingYard(let_name));

        pair<DFA, string> reg_def_pair;
        DFA reg_def_dfa = directConstruction(reg_def_tree);
        reg_def_pair.first = reg_def_dfa;
        reg_def_pair.second = fixed_reg_def;
        reg_defs.push_back(reg_def_pair);

        current_char = ws.simulate(str, current_char).second + 1; // move to next let or rule

    }

    // handle rules
    current_char = rule.simulate(str, current_char).second + 1;
    current_char = ws.simulate(str, current_char).second + 1;
    current_char = name.simulate(str, current_char).second + 1;
    current_char = ws.simulate(str, current_char).second + 1;

    if (str[current_char] != '=') {
        cout << "error en lectura" << endl;
    }
    current_char++; // move after =

    current_char = ws.simulate(str, current_char).second + 1; // move after =


    string temp_definition = "";
    pair<bool, int> status;
    vector<string> executions;
    vector<string> regexps;
    string current_execution = "";
    string last_execution = "";

    cout << "Reading rules..." << endl;
    // rules
    while (current_char < str.length()) {
        status = name.simulate(str, current_char);
        if (status.first) {
            int name_last_char = status.second;
            int matching_name = -1;
            int matching_length = 0;
            for (int i = 0; i < reg_defs.size(); i++) {
                pair<DFA, string> c_reg_def = reg_defs[i];
                status = c_reg_def.first.simulate(str, current_char);
                if (status.first) {
                    if (status.second > matching_length) {
                        matching_name = i;
                        matching_length = status.second;
                    }
                }
            }
            if (matching_name != -1) {
                pair<DFA, string> matching_reg_def = reg_defs[matching_name];
                temp_definition += matching_reg_def.second;
            }
            current_char = status.second + 1;
            continue;
        }
        status = single_char.simulate(str, current_char);
        if (status.first) {
            char c = str[current_char + 1];
            if (c == '\\') {
                c = str[current_char + 2];
                if (c == 'n') c = '\n';
                if (c == 't') c = '\t';
            }
            if (in(ops, c) || c == '.') temp_definition += '\\';
            temp_definition += c;
            current_char = status.second + 1;
            continue;
        }
        status = string_A.simulate(str, current_char);
        if (status.first) {
            int distance = status.second - current_char;
            temp_definition += str.substr(current_char + 1, distance - 1);
            current_char = status.second + 1;
            continue;
        }
        
        if (str[current_char] == '{') {
            string code_extract = "";
            current_char++;
            int layer = 1;
            while (true) {
                if (str[current_char] == '}') {
                    layer--;
                    if (layer == 0) break;
                }
                if (str[current_char] == '{') layer++;
                code_extract += str[current_char];
                current_char++;
            }
            current_execution = code_extract;
            continue;
        }
        status = comment.simulate(str, current_char);
        if (status.first) {
            int distance = status.second - current_char;
            string code_test = str.substr(current_char + 2, distance - 3);
            current_char = status.second + 1;
            last_execution = code_test;
            continue;
        }
        if (str[current_char] == '*' || str[current_char] == '+') {
            temp_definition += str[current_char];
        }
        if (str[current_char] == '|') {
            regexps.push_back(fix_escapes(temp_definition));
            executions.push_back(current_execution);
            temp_definition = "";
            current_execution = "";
            last_execution = "";
        }
        current_char++;
    }
    regexps.push_back(fix_escapes(temp_definition));
    executions.push_back(current_execution);
    // cout << "First execution: " << first_execution << endl;
    // cout << "Regexps: " << regexps.size() << ", Executions: " << executions.size() << endl;
    // for (int i = 0; i < regexps.size(); i++) {
    //     string reg = regexps[i];
    //     string exec = executions[i];
    //     cout << "    Regexp: ";
    //     coutSafe(reg);
    //     cout << ", Execution: " << exec << endl;
    // }
    // cout << "Final execution: " << last_execution << endl;
    cout << "Generating scanner..." << endl;
    generate_scanner(regexps, executions, first_execution, last_execution);
    cout << "Done :)" << endl;
    
    return 0;
}