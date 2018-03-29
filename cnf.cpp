#include <string>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include "cnf.hpp"

using namespace std;

static vector<string> ops = {"&", "|", "<", ">", "="};       /* list of operations */
static vector<string> param = {};
//static string param = "-&&&p----q&r&s-s&tg";                         /* contains command args */

static bool is_ops(const string cand) {                            /* is some character operator */
    vector<string>::iterator result = find(ops.begin(), ops.end(), cand);
    return result != ops.end();
}

template <typename Out>
static void _split (const string s, char delim, Out result) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
        *(result++) = item;
}

static vector<string> split(const string s, char delim) {
    vector<string> tokens;
    _split(s, delim, back_inserter(tokens));
    return tokens;
}

/* Construction & utility functions for class Node */
Node::Node(string _value) {
    parent = NULL;
    children_num = 0;
    secret_children_num = 0;
    value = _value;
}

vector<Node*> Node::get_children() {
    return children;
}

int Node::get_children_num() {
    return children_num;
}

Node* Node::get_ith_child(int i) {
    return children[i];
}

Node* Node::get_parent() {
    return parent;
}

vector<Node*> Node::get_siblings() {
    return parent->get_children();
}

string Node::get_value() {
    return value;
}

void Node::set_value(string new_value) {
    value = new_value;
}

void Node::add_child(Node *child) {
    children.push_back(child);
    child->parent = this;
    children_num++;
}

int Node::set_secret_children_num() {
    int maximum_secret_children = this->get_value() == "-" ? 1 : 2;
    if (secret_children_num < maximum_secret_children) ++secret_children_num;
    return secret_children_num < maximum_secret_children;
}

/* Constructor for class ExpressionTree
 * reads from a string of formula in prefix notation */
ExpressionTree::ExpressionTree() {
    if (param.size() > 0) root = new Node(param[0]);
    else root = NULL;

    cout << root->construct_tree(1) << endl;
}

int Node::construct_tree(int i) {
    cout <<  this->get_value() << param[i] << endl;
    if (this == NULL) return 0;
    if (i >= param.size()) {
        cout << "Reached end of the formula " << i << '\n';
        return --i;
    }

    if (is_ops(param[i])) {
        if ((this->get_value() == "&" || this->get_value() == "|") && (!param[i].compare(this->get_value()))) {
            int first_child_of_child = this->construct_tree(i + 1);
            int second_child_of_child = this->construct_tree(first_child_of_child + 1);
            if (this->set_secret_children_num())
                return this->construct_tree(second_child_of_child + 1);
            else
                return second_child_of_child;
        } else {
            Node *new_node = new Node(param[i]);
            this->add_child(new_node);
            int child = new_node->construct_tree(i + 1);
            if (this->set_secret_children_num())
                return this->construct_tree(child + 1);
            else
                return child;
        }
    }
    else {
        if (!param[i].compare("-")) {
            int how_many_neg = 0;
            while (!param[i+how_many_neg].compare("-"))
                how_many_neg++;
            Node *new_node = NULL;
            Node *new_new_node = NULL;
            if (is_ops(param[i+how_many_neg])) {
                if (!this->get_value().compare("-")) {
                    if (how_many_neg % 2) {
                        this->set_value(param[i+how_many_neg]);
                        return this->construct_tree(i+how_many_neg+1);
                    }
                    else {
                        new_node = new Node(param[i+how_many_neg]);
                        this->add_child(new_node);
                        this->set_secret_children_num();
                        return new_node->construct_tree(i+how_many_neg+1);
                    }
                }
                else {
                    if (how_many_neg % 2) {
                        new_node = new Node("-");
                        this->add_child(new_node);
                        new_new_node = new Node(param[i+how_many_neg]);
                        new_node->add_child(new_new_node);
                        new_node->set_secret_children_num();
                        int child_of_child = new_new_node->construct_tree(i+how_many_neg+1);
                        if (this->set_secret_children_num())
                            return this->construct_tree(child_of_child+1);
                        return child_of_child;
                    }
                    else {
                        new_node = new Node(param[i+how_many_neg]);
                        this->add_child(new_node);
                        int child_of_child = new_node->construct_tree(i+how_many_neg+1);
                        if (this->set_secret_children_num())
                            return this->construct_tree(child_of_child+1);
                        return child_of_child;
                    }
                }
            }
            else {
                if (!this->get_value().compare("-")) {
                    if (how_many_neg % 2)
                        this->set_value(param[i+how_many_neg]);
                    else
                        this->set_value("-" + param[i+how_many_neg]);
                    return i+how_many_neg;
                }
                else {
                    if (how_many_neg % 2)
                        new_node = new Node("-" + param[i+how_many_neg]);
                    else
                        new_node = new Node(param[i+how_many_neg]);
                    this->add_child(new_node);
                    if (this->set_secret_children_num())
                        return this->construct_tree(i+how_many_neg+1);
                    return i+how_many_neg;
                }
            }
        }
        else {
            Node *new_node = new Node(param[i]);
            this->add_child(new_node);
            if (this->set_secret_children_num())
                return this->construct_tree(i+1);
            else
                return i;
        }
    }
}

Node *ExpressionTree::get_root() {
    return root;
}

int main(int argc, char** argv) {
    if (argc != 2)
        return 0;

    ifstream arg_file;
    string param_str;
    arg_file.open(argv[1]);
    getline(arg_file, param_str);
    param = split(param_str, ' ');

    cout << param_str << endl;

    ExpressionTree *new_tree = new ExpressionTree();
    Node *first = new_tree->get_root();
    Node *second = first->get_ith_child(0);
    cout << first->get_value() << endl;
    cout << second->get_value() << endl;
    for (int j = 0; j < second->get_children_num(); j++) {
        cout << second->get_ith_child(j)->get_value() << endl;
    }
    return 0;
}