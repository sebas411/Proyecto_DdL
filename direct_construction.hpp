#include "lib.hpp"

TreeNode* fixTree(TreeNode *tree) {
    if (tree->left == NULL && tree->right == NULL) {
        return new TreeNode((tree->value));
    } else if (tree->value == '?') {
        TreeNode *left = fixTree(tree->left);
        TreeNode *right = new TreeNode((char)(-1));
        TreeNode *newTree = new TreeNode('|', left, right);
        return newTree;
    } else if (tree->value == '+') {
        TreeNode *left = fixTree(tree->left);
        TreeNode *subright = fixTree(tree->left);
        TreeNode *right = new TreeNode('*', subright);
        TreeNode *newTree = new TreeNode('~', left, right);
        return newTree;
    } else if (tree->value == '*') {
        TreeNode *unique = fixTree(tree->left);
        TreeNode *newTree = new TreeNode('*', unique);
        return newTree;
    }
    //only ~ and | remaining
    TreeNode *left = fixTree(tree->left);
    TreeNode *right = fixTree(tree->right);
    TreeNode *newTree = new TreeNode((tree->value), left, right);
    return newTree;
}

bool setFollowPos(TreeNode *tree, set<int> *followPos) {
    if (tree->left == NULL && tree->right == NULL) return true;
    setFollowPos(tree->left, followPos);
    if (tree->value != '*') {
        setFollowPos(tree->right, followPos);
    }
    if (tree->value == '~') {
        for (int i: tree->left->lastPos) {
            cout << "open";
            followPos[i-1].merge(tree->right->firstPos);
            cout << " close" << endl;
        }
    } else if (tree->value == '*') {
        for (int i: tree->lastPos) {
            cout << "open";
            followPos[i-1].merge(tree->firstPos);
            cout << " close";
        }
    }
    cout << tree->value << endl;
    return true;
}

DFA directConstruction(TreeNode *tree) {
    //printTree(tree);
    TreeNode *fixedTree = fixTree(tree);
    //printTree(fixedTree);
    TreeNode *rightNode = new TreeNode('#');
    TreeNode *augmented = new TreeNode('~', fixedTree, rightNode);
    cout << 1 << endl;
    int max_position = augmented->setPositions();
    for (int i: augmented->firstPos) {
        cout << i << " ";
    }
    cout << endl;
    cout << 2 << endl;
    set<int> follow_positions[max_position];
    setFollowPos(tree, follow_positions);
    cout << 3 << endl;

    for (int i = 0; i < max_position; i++) {
        cout << "Posicion " << i+1 << ": {";
        for (int j: follow_positions[i]) {
            cout << ' ' << j;
        }
        cout << '}' << endl;
    }


    //printTree(augmented);
    DFA returned = DFA();
    delete augmented;
    return returned;
}