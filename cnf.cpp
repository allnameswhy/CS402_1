#include <string>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <vector>
#include "cnf.hpp"

using namespace std;

static vector<string> ops = {"&", "|", "<", ">", "="};       /* list of operations */
static string param = ">&-pq&p>rq";                         /* contains command args */

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

void Node::add_child(Node *child) {
    children.push_back(child);
    child->parent = this;
    children_num++;
}

int Node::set_secret_children_num() {
    if (secret_children_num < 2)
        return ++secret_children_num;
    else
        return secret_children_num;
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

    if  (is_ops(param.substr(i, 1))) {
        if ((this->get_value() == "&" || this->get_value() == "|") && (!param.compare(i, 1, this->get_value()))) {
            int first_child_of_child = this->construct_tree(i + 1);
            int second_child_of_child = this->construct_tree(first_child_of_child + 1);
            if (this->set_secret_children_num() < 2)
                return this->construct_tree(second_child_of_child + 1);
            else
                return second_child_of_child;
        } else {
            Node *new_node = new Node(param.substr(i, 1));
            this->add_child(new_node);
            int child = new_node->construct_tree(i + 1);
            if (this->set_secret_children_num() < 2)
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
            int next_index = (how_many_neg%2) + 1;
            Node *new_node = new Node(param.substr(i, next_index));
            this->add_child(new_node);
            next_index += i;
            if (this->set_secret_children_num() < 2)
                return this->construct_tree(next_index);
            else
                return next_index;
        }
        else {
            Node *new_node = new Node(param.substr(i, 1));
            this->add_child(new_node);
            if (this->set_secret_children_num() < 2)
                return this->construct_tree(i+1);
            else
                return i;
        }
    }
}


int main() {
    Node *my_node = new Node("a");
    Node *my_node_child = new Node("b");
    my_node->add_child(my_node_child);
//    cout << my_node->get_value();
//    cout << my_node->get_children()[0]->get_value();

    ExpressionTree *new_tree = new ExpressionTree();
    return 0;
}