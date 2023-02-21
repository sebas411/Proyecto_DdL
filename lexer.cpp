#include <iostream>
#include <stack>
#include <set>
#include <unordered_map>
using namespace std;

// Declaración de clases y funciones
class TreeNode {
    public:
        char value;
        // parametros
        TreeNode *left;
        TreeNode *right;

        //constructores
        TreeNode(char x) {
            value = x;
            left = right = NULL;
        }
        TreeNode(char x, TreeNode *lnode, TreeNode *rnode) {
            value = x;
            left = lnode;
            right = rnode;
        }
        TreeNode(char x, TreeNode *unique) {
            value = x;
            left = unique;
            right = NULL;
        }
};

bool in(string s, char c);

void printTree(TreeNode *node);

string shuntingYard(string infix);

TreeNode* postfixToTree(string postfix);


// main
int main(){
    string postfix = shuntingYard("(a|b)*abb");
    cout << "Resultado: " << postfix << endl;
    TreeNode *expression_tree = postfixToTree(postfix);
    printTree(expression_tree);
    //cout << expression_tree.left->left->left->left->value << endl;
    set<int> hola;
    hola.insert(34);
    hola.insert(36);
    cout << hola.count(34) << endl;

    return 0;
}


// funciones

bool in(string s, char c) {
    for (char ch:s) {
        if (ch == c) {
            return true;
        }
    }
    return false;
}

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

    concatted += infix[0];
    for (int i = 1; i < infix.length(); i++) {
        char prevc = infix[i-1];
        char c = infix[i];
        if (prevc != '\0' && 
           (in(left_concat, prevc) || !in(ops, prevc)) && // left
           (in(right_concat, c)    || !in(ops, c)))       // right
        { 
            concatted += '~';
        } 
        concatted += c;
    }

    // cout << concatted << endl;

    stack<char> operators;
    // operators: () > *+? > |
    for (int i=0; i < concatted.length(); i++) {
        char token = concatted[i];
        //cout << postfix << endl;
        //printf("Token actual: %c ", token);
        if (token == '(') {
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
    string ops = "*|+?~";
    string unary_ops = "+*?";

    for (char c: postfix) {
        if (!in(ops, c)) {
            TreeNode *node = new TreeNode(c);
            tree_stack.push(node);
        } else {
            if (in(unary_ops,c)) {
                TreeNode *unique = tree_stack.top();
                tree_stack.pop();
                TreeNode *node = new TreeNode(c, unique);
                tree_stack.push(node);
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

void printTree(TreeNode *node) {
    //cout << "hola" << endl;
    if (node) {
        //cout << node->value << endl;
        printTree(node->left);

        printTree(node->right);
        cout << node->value << " ";
    }
}

