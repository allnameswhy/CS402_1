#include <string>
#include <cstdlib>
#include <iostream>
#include "cnf.hpp"

using namespace std;

Node::Node(string _value) {
    parent = NULL;
    children_num = 0;
    value = _value;
}

vector<Node*> Node::get_children() {
    return children;
}

Node* Node::get_ith_child(int i) {
    cout << i;
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

int main() {
    Node *my_node = new Node("a");
    Node *my_node_child = new Node("b");
    my_node->add_child(my_node_child);
    cout << my_node->get_value();
    cout << my_node->get_children()[0]->get_value();

    return 0;
}