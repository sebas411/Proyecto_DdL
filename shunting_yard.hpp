#include "lib.hpp"


string shuntingYard(string infix) {
    string concatted = "";
    string postfix = "";
    unordered_map<char, int> precedence;
    precedence['*'] = 2;
    precedence['+'] = 2;
    precedence['?'] = 2;
    precedence['~'] = 1;
    precedence['|'] = 0;
    precedence['('] = -1;

    string ops = "|+?*()";
    string right_concat = "(";
    string left_concat = ")*?+";

    //concatted += infix[0];

    char prevc = '\0';
    for (int i = 0; i < infix.length(); i++) {
        char c = infix[i];
        if (prevc != '\0' && 
           (in(left_concat, prevc) || !in(ops, prevc)) && // left
           (in(right_concat, c)    || !in(ops, c)))       // right
        { 
            concatted += '~';
        }
        if (c == '~') {
            concatted += '\\';
        }
        if (c == ')' && prevc == '(') {
            concatted += (char)(-1);
        }
        concatted += c;
        prevc = c;
    }

    // cout << concatted << endl;

    stack<char> operators;
    
    bool escaped = false;
    for (int i=0; i < concatted.length(); i++) {
        if (escaped) {
            escaped = false;
            continue;
        }
        char token = concatted[i];
        if (token == '\\') {
            escaped = true;
            postfix += '\\';
            postfix += concatted[i+1];
        } else if (token == '(') {
            operators.push(token);
        } else if (token == ')') {
            while (!operators.empty() && operators.top() != '(') {
                postfix += operators.top();
                operators.pop();
            }
            if (!operators.empty() && operators.top() == '(') {
                operators.pop();
            }
        } else if (precedence.find(token) != precedence.end()) {
            while (!operators.empty() && precedence[operators.top()] >= precedence[token]) {
                postfix += operators.top();
                operators.pop();
            }
            operators.push(token);
        } else {
            postfix += token;
        }
    }

    while (!operators.empty()) {
        postfix += operators.top();
        operators.pop();
    }
    return postfix;
}


TreeNode* postfixToTree(string postfix) {
    stack<TreeNode*> tree_stack;
    string ops = "*|+?~\\";
    string unary_ops = "+*?";
    bool escaped = false;

    for (char c: postfix) {
        if (!in(ops, c) || escaped) {
            TreeNode *node = new TreeNode(c);
            tree_stack.push(node);
            escaped = false;
        } else {
            if (in(unary_ops,c)) {
                TreeNode *unique = tree_stack.top();
                tree_stack.pop();
                TreeNode *node = new TreeNode(c, unique);
                tree_stack.push(node);
            } else if (c == '\\') {
                escaped = true;
            } else {
                TreeNode *right = tree_stack.top();
                tree_stack.pop();
                TreeNode *left = tree_stack.top();
                tree_stack.pop();
                TreeNode *node = new TreeNode(c, left, right);
                tree_stack.push(node);
            }
        }
    }
    return tree_stack.top();
}