// ====================================================
// Jason Williamson (20552360)
// CS 241 Winter 2016
// Assignment 03, Problem 1
// File: traverse / MultiTree.cc
// ====================================================
//

#include "multitree.h"
#include <iostream>


MultiTree::MultiTree(int size,int data):data(data),size(size),
children(new MultiTree*[size]) {
    for (int i=0;i<size;i++) {
        children[i] = 0;
    }
}

MultiTree::~MultiTree() {
    for (int i=0;i<size;i++) {
        delete children[i];
    }
    delete [] children;
    //std::cout << "cleanup on " << this->data << std::endl;
}

void MultiTree::printTree() const{
    for (int i = 0; i < size; i++) {
        children[i]->printTree();
    }
    std::cout << this->data << " " << this->getSize() << std::endl;
}


void MultiTree::setChild(int index,MultiTree *value) {
    children[index] = value;
}

MultiTree *MultiTree::getChild(int index) {
    return children[index];
}

int MultiTree::getData() const {
    return data;
}

int MultiTree::getSize() const {
    return size;
}

MultiTree &MultiTree::operator=(const MultiTree &other) {
    if (this==&other) return *this;
    for (int i=0;i<size;i++) {
        delete children[i];
    }
    delete [] children;
    data = other.data;
    size = other.size;
    children = new MultiTree*[size];
    for (int i=0;i<size;i++) {
        children[i] = other.children[i] ? new MultiTree(*other.children[i]) : 0;
    }
    return *this;
}

