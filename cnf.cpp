#include <string>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <vector>
#include "cnf.hpp"

using namespace std;

static vector<string> ops = {"&", "|", "<", ">", "="};       /* list of operations */
static string param = "-&&&p----q&r&s-s&tg";                         /* contains command args */

static bool is_ops(string cand) {                            /* is some character operator */
    vector<string>::iterator result = find(ops.begin(), ops.end(), cand);
//    cout << *result << endl;
    return result != ops.end();
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
    if (param.length() > 0) root = new Node(param.substr(0, 1));
    else root = NULL;

    cout << root->construct_tree(1) << endl;
}

int Node::construct_tree(int i) {
    cout <<  this->get_value() << param[i] << endl;
    if (this == NULL) return 0;
    if (i >= param.length()) {
        cout << "Reached end of the formula " << i << '\n';
        return --i;
    }

    if (is_ops(param.substr(i, 1))) {
        if ((this->get_value() == "&" || this->get_value() == "|") && (!param.compare(i, 1, this->get_value()))) {
            int first_child_of_child = this->construct_tree(i + 1);
            int second_child_of_child = this->construct_tree(first_child_of_child + 1);
            if (this->set_secret_children_num())
                return this->construct_tree(second_child_of_child + 1);
            else
                return second_child_of_child;
        } else {
            Node *new_node = new Node(param.substr(i, 1));
            this->add_child(new_node);
            int child = new_node->construct_tree(i + 1);
            if (this->set_secret_children_num())
                return this->construct_tree(child + 1);
            else
                return child;
        }
    }
    else {
        if (param[i] == '-') {
            int how_many_neg = 0;
            while (param[i+how_many_neg] == '-')
                how_many_neg++;
            Node *new_node = NULL;
            Node *new_new_node = NULL;
            if (is_ops(param.substr(i+how_many_neg, 1))) {
                if (!this->get_value().compare("-")) {
                    if (how_many_neg % 2) {
                        this->set_value(param.substr(i+how_many_neg, 1));
                        return this->construct_tree(i+how_many_neg+1);
                    }
                    else {
                        new_node = new Node(param.substr(i+how_many_neg, 1));
                        this->add_child(new_node);
                        this->set_secret_children_num();
                        return new_node->construct_tree(i+how_many_neg+1);
                    }
                }
                else {
                    if (how_many_neg % 2) {
                        new_node = new Node("-");
                        this->add_child(new_node);
                        new_new_node = new Node(param.substr(i+how_many_neg, 1));
                        new_node->add_child(new_new_node);
                        new_node->set_secret_children_num();
                        int child_of_child = new_node->construct_tree(i+how_many_neg+1);
                        if (this->set_secret_children_num())
                            return this->construct_tree(child_of_child+1);
                        return child_of_child;
                    }
                    else {
                        new_node = new Node(param.substr(i+how_many_neg, 1));
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
                        this->set_value(param.substr(i+how_many_neg, 1));
                    else
                        this->set_value("-" + param.substr(i+how_many_neg, 1));
                    return i+how_many_neg;
                }
                else {
                    if (how_many_neg % 2)
                        new_node = new Node("-" + param.substr(i+how_many_neg, 1));
                    else
                        new_node = new Node(param.substr(i+how_many_neg, 1));
                    this->add_child(new_node);
                    if (this->set_secret_children_num())
                        return this->construct_tree(i+how_many_neg+1);
                    return i+how_many_neg;
                }
            }
        }
        else {
            Node *new_node = new Node(param.substr(i, 1));
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


int main() {
    Node *my_node = new Node("a");
    Node *my_node_child = new Node("b");
    my_node->add_child(my_node_child);
//    cout << my_node->get_value();
//    cout << my_node->get_children()[0]->get_value();

    ExpressionTree *new_tree = new ExpressionTree();
    Node *root = new_tree->get_root();
    Node *second = root->get_ith_child(0);
    Node *third = second->get_ith_child(3);
//    for (int i = 0; i < new_tree->get_root()->get_children_num(); i++) {
//        cout << new_tree->get_root()->get_children()[i]->get_value() << endl;
//    }
    cout << root->get_value() << second->get_value() << third->get_value() << endl;
//    cout << new_tree->get_root()->get_value() << endl;
    return 0;
}