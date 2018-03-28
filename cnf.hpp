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
        int secret_children_num;
        string value;
    public:
        Node(string);
        vector<Node*> get_children();
        int get_children_num();
        Node* get_ith_child(int);
        Node* get_parent();
        vector<Node*> get_siblings();
        string get_value();
        void set_value(string);
        void add_child(Node *);
        int set_secret_children_num();
        int construct_tree(int);
};

class ExpressionTree {
    private:
        Node *root;
    public:
        ExpressionTree();
        Node *get_root();
};

#endif CS402_1_CNF_HPP