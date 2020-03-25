// ====================================================
// Jason Williamson (20552360)
// CS 241 Winter 2016
// Assignment 03, Problem 1
// File: traverse.cc
// ====================================================
//

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <utility>
#include <stack>
#include "multitree.h"
using namespace std;

void popParent(stack< pair<MultiTree*, int > > &pStack){
    if (!pStack.empty()) {
        if (pStack.top().first->getSize() == pStack.top().second) {
            pStack.pop();
            popParent(pStack);
        }
    }
}


int main(int argc, const char * argv[]) {
    bool firstLine = true;
    int data;
    int numChildren;
    cin >> data;
    cin >> numChildren;
    MultiTree *root = new MultiTree(numChildren, data);
    stack< pair<MultiTree*, int> > parentStack;              //to store current parent and child count
    int zero = 0;
    
    string line;
    
    while (getline(cin, line)){
        
        //setup for root node
        if (firstLine) {
            firstLine = false;
            pair<MultiTree*, int> p(root, zero);
            parentStack.push(p);
            continue;
        }
        istringstream ss(line);
        ss >> data;
        ss >> numChildren;
        
        //child will always be added to a parent node
        MultiTree *child = new MultiTree(numChildren, data);
        int index = parentStack.top().second;
        parentStack.top().first->setChild(index, child);
        parentStack.top().second++;
        
        //if the child has no children itself then we are ready to add another child
        //otherwise we must push the child to the top of the stack to make ready to accept children
        if (numChildren > 0) {
            pair<MultiTree*, int> p(child, zero);     //push parent to stack, along with parents indexCount
            parentStack.push(p);
        }else if(parentStack.top().second == parentStack.top().first->getSize()){
            //we are done adding nodes children and it can be popped of the stack
            popParent(parentStack);
        }
        
        if (parentStack.empty()) {
            //cout << "parentStack is empty" << '\n';
            break;
        }
    }
    
    //printing and cleanup
    root->printTree();
    delete root;
    root = NULL;
    
    return 0;
}
