// ====================================================
// Jason Williamson (20552360)
// CS 241 Winter 2016
// Assignment 03, Problem 1
// File: traverse / MultiTree.h
// ====================================================
//

#ifndef MULTI_TREE_H
#define MULTI_TREE_H

class MultiTree {
    int data;
    int size;
    MultiTree **children;
    
public:
    MultiTree(int size,int data);
    MultiTree &operator=(const MultiTree&);
    ~MultiTree();
    void setChild(int index,MultiTree *value);
    MultiTree *getChild(int index);
    int getSize() const;
    int getData() const;
    void printTree() const;
};

#endif
