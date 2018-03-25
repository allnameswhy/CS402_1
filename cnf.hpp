/* cnf.hpp: classes and data structure for cnf converter */

#ifndef CS402_1_CNF_HPP
#define CS402_1_CNF_HPP

#include <string>
#include <vector>

using namespace std;

/* Expression tree to compute formulas, NOT binary */
class Node {
    private:
        Node* parent;
        vector<Node*> children;
        int children_num;
        string value;
    public:
        Node(string);
        vector<Node*> get_children();
        Node* get_ith_child(int);
        Node* get_parent();
        vector<Node*> get_siblings();
        string get_value();
        void add_child(Node *);
};

class ExpressionTree {
    private:
        Node *root;
    public:
        ExpressionTree(string);
};

#endif CS402_1_CNF_HPP