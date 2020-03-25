#include "kind.h"
#include "lexer.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <cstdio>
// Use only the neeeded aspects of each namespace
using std::string;
using std::vector;
using std::endl;
using std::cerr;
using std::cin;
using std::pair;
using std::map;
using std::getline;
using ASM::Token;
using ASM::Lexer;
using std::cout;
using std::endl;

void output_word(int word){
    putchar(word >> 24);
    putchar(word >> 16);
    putchar(word >> 8);
    putchar(word);
}

void validateLabels(map<string, int> &symTbl, int currentAddress){
    map<string,int>::iterator it = symTbl.begin();
    for (it=symTbl.begin(); it!=symTbl.end(); ++it){
        //cout << it->first << " => " << it->second << '\n';
        if (it->second == currentAddress) {
            string id = it->first;
            throw "ERROR: label does not point to valid instruction: " + id;
        }
    }
}

//Lookup returns true if element exists within the map
bool lookup(map<string, int> &symTbl, string label){
    map<string,int>::iterator it = symTbl.find(label);
    if(it == symTbl.end())
    {
        return false;
    }
    return true;
}

int main(int argc, char* argv[]){
    // Nested vector representing lines of Tokens
    // Needs to be used here to cleanup in the case
    // of an exception
    vector< vector<Token*> > tokLines;
    map<string, int> symbolTable;
    try{
        // Create a MIPS recognizer to tokenize
        // the input lines
        Lexer lexer;
        // Tokenize each line of the input
        string line;
        while(getline(cin,line)){
            tokLines.push_back(lexer.scan(line));
        }
        
        // Iterate over the lines of tokens and print them
        // to standard error
        string prevToken = "";
        string curToken = "";
        int addressIncr = 4;
        int curAddress = 0;
        
        vector<vector<Token*> >::iterator it;
        for(it = tokLines.begin(); it != tokLines.end(); ++it){
            vector<Token*>::iterator it2;
            for(it2 = it->begin(); it2 != it->end(); ++it2){
                curToken = (*it2)->toString();
                 
                //ONLY to accept DOTWORD and LABEL
                if (prevToken == ""){
                    if (curToken == "DOTWORD"){
                        prevToken = "DOTWORD";
                    }else if (curToken == "LABEL"){
                        string fullLabel = (*it2)->getLexeme();
                        string label = fullLabel.substr(0, fullLabel.size()-1);
                        
                        //do not allow double defined labels
                        if (lookup(symbolTable, label)) {
                            throw "ERROR: label already defined for: " + label;
                        }
                        //Store in symbol table at current address
                        symbolTable.insert(pair<string, int>(label, curAddress));
                        prevToken = "";
                    }else{
                        throw "ERROR: invalid token at: " + curToken;
                    }
                    
                }
                else if (prevToken == "DOTWORD") {
                    if ((curToken != "INT") && (curToken != "HEXINT")) {
                        throw "ERROR: invalid '.word' followed by: " + curToken;
                    }
                    //OUTPUT HERE
                    int word = (*it2)->toInt();
                    output_word(word);
                    prevToken = "";
                    
                    //INCREMENT VALID INSTRUCTION
                    curAddress += addressIncr;
                }
                
            }
            
            
            //Here a DOTWORD instruction has not been followed by an integer value
            if (prevToken == "DOTWORD"){
                throw "ERROR: expecting integer got nothing after " + prevToken;
            }
             
        }
        
       // validateLabels(symbolTable, curAddress);
        
    } catch(const string& msg){
        // If an exception occurs print the message and end the program
        cerr << msg << endl;
    }
    
    
    //Here we print the symbol table
    map<string,int>::iterator itm2 = symbolTable.begin();
    for (itm2=symbolTable.begin(); itm2!=symbolTable.end(); ++itm2){
        cerr << itm2->first << " " << itm2->second << endl;
    }
   
    
    // Delete the Tokens that have been made
    vector<vector<Token*> >::iterator it;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
        vector<Token*>::iterator it2;
        for(it2 = it->begin(); it2 != it->end(); ++it2){
            delete *it2;
        }
    }
}

