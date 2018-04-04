//
// Created by Sunmin Lee on 2018-04-03.
//

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
static vector<string> param = {}; /* contains command args */

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
index = -1;
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

int Node::get_index() {
    return index;
}

void Node::set_value(string new_value) {
value = new_value;
}

void Node::add_child(Node *child) {
    children.push_back(child);
    child->parent = this;
    child->index = children_num;
    children_num++;
}

Node *Node::replace_child(int i, Node *new_child) {
    Node *prev_child = children[i];
    children[i] = new_child;
    new_child->parent = this;
    new_child->index = i;
    return prev_child;
}

Node *Node::negate() {
    if (!value.compare("-")) { // 'this' is freed and returns the child
        Node *child  = children[0];
        if (parent) {
            free(parent->replace_child(index, child));
            return child;
        }
        free(this);
        return NULL;
    }
    else { // 'this' is still alive, pointing after the negation
        Node *intermediate = new Node("-");
        if (parent) {
            parent->replace_child(index, intermediate);
        }
        intermediate->add_child(this);
        return intermediate;
    }
}

Node* Node::remove_child(int i) {
    Node *prev = children[i];
    children.erase(children.begin()+i);
    children_num--;
    return prev;
}

bool Node::set_secret_children_num() {
    int maximum_secret_children = this->get_value().compare("-") ? 2 : 1;
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
    if (this == NULL) return 0;
    if (i >= param.size()) {
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
                    else {
                        new_node = new Node(param[i+how_many_neg]);
                        this->add_child(new_node);
                    };
                    return i+how_many_neg;
                }
                else {
                    if (how_many_neg % 2) {
                        new_node = new Node("-");
                        this->add_child(new_node);
                        new_new_node = new Node(param[i+how_many_neg]);
                        new_node->add_child(new_new_node);
                        if (this->set_secret_children_num())
                            return this->construct_tree(i+how_many_neg+1);
                    }
                    else {
                        new_node = new Node(param[i+how_many_neg]);
                        this->add_child(new_node);
                        if (this->set_secret_children_num())
                            return this->construct_tree(i+how_many_neg+1);
                    }
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

Node *ExpressionTree::_impl_free(Node *cur) {
    if (!cur) return cur;

    if (!cur->get_value().compare(">")) {
        Node *new_node = new Node("|");
        Node *first_child = _impl_free(cur->get_ith_child(0));
        Node *second_child = _impl_free(cur->get_ith_child(1));

        new_node->add_child(first_child);
        first_child = first_child->negate();
        new_node->add_child(second_child);

        if (!first_child->get_value().compare("|")) {
            for (int i = 0; i < first_child->get_children_num(); i++) {
                if (!i) {
                    cur->replace_child(0, first_child->get_ith_child(0));
                    continue;
                }
                cur->add_child(first_child->get_ith_child(i));
            }
        }

        if (!second_child->get_value().compare("|")) {
            for (int i = 0; i < second_child->get_children_num(); i++) {
                if (!i) {
                    cur->replace_child(1, second_child->get_ith_child(0));
                    continue;
                }
                cur->add_child(second_child->get_ith_child(i));
            }
        }

//        free(cur);
        return new_node;
    }
    else if (!cur->get_value().compare("<")) {
        Node *new_node = new Node("|");
        Node *first_child = _impl_free(cur->get_ith_child(0));
        Node *second_child = _impl_free(cur->get_ith_child(1));

        new_node->add_child(first_child);
        new_node->add_child(second_child);
        second_child = second_child->negate();

        if (!first_child->get_value().compare("|")) {
            for (int i = 0; i < first_child->get_children_num(); i++) {
                if (!i) {
                    cur->replace_child(0, first_child->get_ith_child(0));
                    continue;
                }
                cur->add_child(first_child->get_ith_child(i));
            }
        }

        if (!second_child->get_value().compare("|")) {
            for (int i = 0; i < second_child->get_children_num(); i++) {
                if (!i) {
                    cur->replace_child(1, second_child->get_ith_child(0));
                    continue;
                }
                cur->add_child(second_child->get_ith_child(i));
            }
        }

//        free(cur);
        return new_node;
    }
    else if (!cur->get_value().compare("=")) {
        Node *first_child = _impl_free(cur->get_ith_child(0));
        Node *first_child_2 = _impl_free(cur->get_ith_child(0));
        Node *second_child = _impl_free(cur->get_ith_child(1));
        Node *second_child_2 = _impl_free(cur->get_ith_child(1));
        Node *first_child_new = new Node("|");
        Node *second_child_new = new Node("|");

        Node *new_node = new Node("&");
        new_node->add_child(first_child_new);
        new_node->add_child(second_child_new);

        first_child_new->add_child(first_child);
        first_child = first_child_new->get_ith_child(0)->negate();
        first_child_new->add_child(second_child);

        second_child_new->add_child(first_child_2);
        second_child_new->add_child(second_child_2);
        second_child_2 = second_child_new->get_ith_child(1)->negate();

        if (!first_child->get_value().compare("|")) {
            for (int i = 0; i < first_child->get_children_num(); i++) {
                if (!i) {
                    first_child_new->replace_child(0, first_child->get_ith_child(0));
                    continue;
                }
                first_child_new->add_child(first_child->get_ith_child(i));
            }
        }

        if (!second_child->get_value().compare("|")) {
            for (int i = 0; i < second_child->get_children_num(); i++) {
                if (!i) {
                    first_child_new->replace_child(1, second_child->get_ith_child(0));
                    continue;
                }
                first_child_new->add_child(second_child->get_ith_child(i));
            }
        }

        if (!first_child_2->get_value().compare("|")) {
            for (int i = 0; i < first_child_2->get_children_num(); i++) {
                if (!i) {
                    second_child_new->replace_child(0, first_child_2->get_ith_child(0));
                    continue;
                }
                second_child_new->add_child(first_child_2->get_ith_child(i));
            }
        }

        if (!second_child_2->get_value().compare("|")) {
            for (int i = 0; i < second_child_2->get_children_num(); i++) {
                if (!i) {
                    second_child_new->replace_child(1, second_child_2->get_ith_child(0));
                    continue;
                }
                second_child_new->add_child(second_child_2->get_ith_child(i));
            }
        }

//        free(cur);
        return new_node;
    }
    else {
        Node *new_node = new Node(cur->get_value());
        Node *child;
        for (int i = 0; i < cur->get_children_num(); i++) {
            child = _impl_free(cur->get_ith_child(i));
            new_node->add_child(child);
            free(cur->get_ith_child(i));
        }
//        free(cur);
        return new_node;
    }
}

Node *ExpressionTree::_nnf(Node *cur) {
    if (!cur) return cur;

    //cout << "nnf " << cur->get_value()  << endl;

    Node *new_child = cur;
    if (!cur->get_value().compare("-")) {
        if (!cur->get_ith_child(0)->get_value().compare("-")) {
            new_child = _nnf(cur->get_ith_child(0)->get_ith_child(0));
            if (cur->get_parent())
                free(cur->get_parent()->replace_child(cur->get_index(), new_child)); //free
        }
        else if (!cur->get_ith_child(0)->get_value().compare("&")) {
            new_child = new Node("|");
            for (int i = 0; i < cur->get_ith_child(0)->get_children_num(); i++)
                new_child->add_child(cur->get_ith_child(0)->get_ith_child(i)->negate());
            new_child = _nnf(new_child);
            if (cur->get_parent())
                free(cur->get_parent()->replace_child(cur->get_index(), new_child)); //free
        }
        else if (!cur->get_ith_child(0)->get_value().compare("|")) {
            new_child = new Node("&");
            for (int i = 0; i < cur->get_ith_child(0)->get_children_num(); i++)
                new_child->add_child(cur->get_ith_child(0)->get_ith_child(i)->negate());
            new_child = _nnf(new_child);
            if (cur->get_parent())
                free(cur->get_parent()->replace_child(cur->get_index(), new_child)); //free
        }
    }
    else if (is_ops(cur->get_value())) {
        new_child = new Node(cur->get_value());
        Node *child;
        for (int i = 0; i < cur->get_children_num(); i++) {
            child = _nnf(cur->get_ith_child(i));
            if (!new_child->get_value().compare(child->get_value())) {
                for (int j = 0; j < child->get_children_num(); j++) {
                    new_child->add_child(child->get_ith_child(j));
                }
            }
            else
                new_child->add_child(child);
        }
        if (cur->get_parent())
            free(cur->get_parent()->replace_child(cur->get_index(), new_child)); //free
    }
//    else {
//        free(new_child);
//        return cur;
//    }
    return new_child;
}

Node *ExpressionTree::_distr(Node *one, Node *two) {
    if (!one || !two) return NULL;

    Node *new_node = new Node("&");
    Node *child;
    if (!one->get_value().compare("&")) {
        for (int i = 0; i < one->get_children_num(); i++) {
            child = _distr(one->get_ith_child(i), two);
            if (!child->get_value().compare("&")) {
                for (int j = 0; j < child->get_children_num(); j++) {
                    new_node->add_child(child->get_ith_child(j));
                }
            }
            else
                new_node->add_child(child);
        }
    }
    else if (!two->get_value().compare("&")) {
        for (int i = 0; i < two->get_children_num(); i++) {
            child = _distr(one, two->get_ith_child(i));
            if (!child->get_value().compare("&")) {
                for (int j = 0; j < child->get_children_num(); j++) {
                    new_node->add_child(child->get_ith_child(j));
                }
            }
            else
                new_node->add_child(child);
        }
    }
    else {
        new_node->set_value("|");
        if (!one->get_value().compare("|")) {
            for (int j = 0; j < one->get_children_num(); j++) {
                new_node->add_child(one->get_ith_child(j));
            }
        }
        else
            new_node->add_child(one);
        if (!two->get_value().compare("|")) {
            for (int j = 0; j < two->get_children_num(); j++) {
                new_node->add_child(two->get_ith_child(j));
            }
        }
        else
            new_node->add_child(two);
    }
    return new_node;
}


Node *ExpressionTree::_cnf(Node *cur) {
    if (!cur) return NULL;

//    //cout << "cnf " << cur->get_value() << endl;

    if (!cur->get_value().compare("|")) {
        Node *new_node = _cnf_and_distr(cur->get_children());
        if (cur->get_parent()) {
            free(cur->get_parent()->replace_child(cur->get_index(), new_node));
            if (!new_node->get_value().compare(new_node->get_parent()->get_value())) {
                for (int j = 0; j < new_node->get_children_num(); j++) {
                    if (!j) {
                        new_node->get_parent()->replace_child(new_node->get_index(), new_node->get_ith_child(j));
                        continue;
                    }
                    new_node->get_parent()->add_child(new_node->get_ith_child(j));
                }
            }
        }
        return new_node;
    }
    else if (!cur->get_value().compare("&")) {
        Node *child;
        for (int i = 0; i < cur->get_children_num(); i++) {
            child = _cnf(cur->get_ith_child(i));
            if (!child->get_value().compare("&")) {
                for (int j = 0; j < child->get_children_num(); j++) {
                    if (!j) {
                        cur->replace_child(i, child->get_ith_child(j));
                        continue;
                    }
                    cur->add_child(child->get_ith_child(j));
                }
            }
            else
                cur->replace_child(i, child);
        }
    }
    return cur;
}

Node *ExpressionTree::_cnf_and_distr(vector<Node*> children) {
//    //cout << "distr" << endl;
    if (children.size() < 2)
        return NULL;
    else if (children.size() == 2) {
        return _distr(_cnf(children[0]), _cnf(children[1]));
    }
    else {
        vector<Node*>::const_iterator first = children.begin() + 1;
        vector<Node*>::const_iterator last = children.begin() + children.size();
        vector<Node*> subchildren(first, last);
        return _distr(_cnf(children[0]), _cnf_and_distr(subchildren));
    }
}

void ExpressionTree::cnf() {
    cout << "impl" << endl;
    root = _impl_free(root);
    cout << "nnf" << endl;
    root = _nnf(root);
    cout << "cnf" << endl;
    root = _cnf(root);
}

string ExpressionTree::infix() {
    if (!root || root->get_value().compare("&"))
        return "";

    vector<Node*> children = root->get_children();
    string str;
    for (vector<Node*>::iterator it = children.begin(); it != children.end(); it++) {
        if (!(*it)->get_value().compare("|")) {
            str.append("( ");
            for (int j = 0; j < (*it)->get_children_num(); j++) {
                if ((*it)->get_ith_child(j)->get_value().compare("-"))
                    str.append((*it)->get_ith_child(j)->get_value());
                else {
                    str.append((*it)->get_ith_child(j)->get_value());
                    str.append(" ");
                    str.append((*it)->get_ith_child(j)->get_ith_child(0)->get_value());
                }
                str.append(" | ");
            }
            str.pop_back();
            str.pop_back();
            str.pop_back();
            str.append(" )");
        }
        else if (!(*it)->get_value().compare("-")) {
            str.append((*it)->get_value());
            str.append(" ");
            str.append((*it)->get_ith_child(0)->get_value());
        }
        else
            str.append((*it)->get_value());
        str.append(" & ");
    }
    str.pop_back();
    str.pop_back();
    str.pop_back();
    return str;
}

string ExpressionTree::prefix() {
    if (!root || root->get_value().compare("&"))
        return "";

    string str;
    for (int i = 0; i < root->get_children_num(); i ++) {
        if (i < root->get_children_num()-1)
            str.append("& ");
        Node *cur = root->get_ith_child(i);
        if (!cur->get_value().compare("|")) {
            for (int j = 0; j < cur->get_children_num(); j++) {
                if (j < cur->get_children_num()-1)
                    str.append("| ");
                if (!cur->get_ith_child(j)->get_value().compare("-")) {
                    str.append(cur->get_ith_child(j)->get_value());
                    str.append(" ");
                    str.append(cur->get_ith_child(j)->get_ith_child(0)->get_value());
                    str.append(" ");
                }
                else{
                    str.append(cur->get_ith_child(j)->get_value());
                    str.append(" ");
                }
            }
        }
        else if (!cur->get_value().compare("-")) {
            str.append(cur->get_value());
            str.append(" ");
            str.append(cur->get_ith_child(0)->get_value());
            str.append(" ");
        }
        else {
            str.append(cur->get_value());
            str.append(" ");
        }
    }
    str.pop_back();
    return str;
}

/* check the validity of the given normal formula
 * all clauses should contain pair of negative literals */
bool ExpressionTree::validity() {
    vector<Node*> children = root->get_children();
    vector<Node*> ch_children;
    vector<Node*>::iterator it, it2;
    vector<string> pos = {};
    vector<string> neg = {};
    vector<string>::iterator comp, already;
    bool clause = false;



    for (it = children.begin(); it != children.end(); it++) {
        if (!(*it)->get_children_num()) // if literal directly under &
            continue;
        ch_children = (*it)->get_children();
        for (it2 = ch_children.begin(); it2 != ch_children.end(); it2++) {
            if ((*it2)->get_value().compare("-")) { // positive literal
                comp = find(neg.begin(), neg.end(), (*it2)->get_value());
                already = find(pos.begin(), pos.end(), (*it2)->get_value());
                if (already != pos.end()) { // already exists
                    (*it)->remove_child(it2 - ch_children.begin());
                }
                if (comp != neg.end()) { // there is a complimentary pair
                    clause = true;
                    root->remove_child(it - children.begin());
                    break; // break the inner loop for a single clause
                }
                else {
                    pos.push_back((*it2)->get_value());
                }
            }
            else {
                comp = find(pos.begin(), pos.end(), (*it2)->get_ith_child(0)->get_value());
                already = find(neg.begin(), neg.end(), (*it2)->get_ith_child(0)->get_value());
                if (already != neg.end()) {
                    (*it)->remove_child(it2 - ch_children.begin());
                }
                if (comp != pos.end()) {
                    clause = true;
                    root->remove_child(it - children.begin());
                    break;
                }
                else
                    neg.push_back((*it2)->get_ith_child(0)->get_value());
            }
        }
        if (!clause) {
            cout << "Not Valid" << endl;
            return false;
        }
    }
    cout << "Valid" << endl;
    return true;
}

int main(int argc, char** argv) {
    if (argc != 2)
        return 0;

    ifstream arg_file;
    string param_str;
    arg_file.open(argv[1]);
    getline(arg_file, param_str);
    param = split(param_str, ' ');

    ExpressionTree *new_tree = new ExpressionTree();
    new_tree->cnf();

    Node *first = new_tree->get_root();

//    cout << "\n" << endl;
//    cout << first->get_value() << endl;
//    for (int j = 0; j < first->get_children_num(); j++) {
//        cout << first->get_ith_child(j)->get_value() << endl;
//    }

    cout << "\n" << endl;
//    cout << new_tree->infix() << endl;
//    cout << new_tree->prefix() << endl;
    new_tree->validity();
}