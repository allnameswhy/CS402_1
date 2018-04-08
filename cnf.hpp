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
        vector<int> children_int;
        int index;
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
        int get_index();
        void set_value(string);
        void add_child(Node *);
        Node *replace_child(int, Node *);
        Node *negate();
        Node *remove_child(int);
        bool set_secret_children_num();
        int construct_tree(int);
        int numbering_literals();
};

class ExpressionTree {
    private:
        Node *root;
    public:
        ExpressionTree();
        Node *get_root();
        Node *_impl_free(Node *);
        Node *_nnf(Node *);
        Node *_distr(Node *, Node *);
        Node *_cnf(Node *);
        Node *_cnf_and_distr(vector<Node*>);
        void cnf();
        string infix();
        string prefix();
        bool validity();
        void miniSAT(string);
};

#endif
