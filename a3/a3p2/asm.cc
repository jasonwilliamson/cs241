#include "kind.h"
#include "lexer.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <utility>
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

int main(int argc, char* argv[]){
    // Nested vector representing lines of Tokens
    // Needs to be used here to cleanup in the case
    // of an exception
    vector< vector<Token*> > tokLines;
    map<string, int> symbolTable;
    bool valid = true;
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
                
                //cout << "tokLines: " << tokLines.size() << endl;
                //cout << curToken << endl;
                
                //ONLY to accept DOTWORD and LABEL
                if (prevToken == ""){
                    if (curToken == "DOTWORD"){
                        prevToken = "DOTWORD";
                    }else if (curToken == "LABEL"){
                         //Store in symbol table at current address
                        string fullLabel = (*it2)->getLexeme();
                        string label = fullLabel.substr(0, fullLabel.size()-1);
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
                    
                    //INCREMENT VALID INSTRUCTION (so this counts as an instruction, so count this line!)
                    curAddress += addressIncr;
                }
                
            }
            
            
            //Here a DOTWORD instruction has not been followed by an integer value
            if (prevToken == "DOTWORD"){
                throw "ERROR: expecting integer got nothing after " + prevToken;
            }
             
        }
        
        validateLabels(symbolTable, curAddress);
        
    } catch(const string& msg){
        // If an exception occurs print the message and end the program
        cerr << msg << endl;
        valid = false;
    }
    
    //Here we print the symbol table
    if (valid) {
        map<string,int>::iterator itm2 = symbolTable.begin();
        for (itm2=symbolTable.begin(); itm2!=symbolTable.end(); ++itm2){
            cerr << itm2->first << " " << itm2->second << endl;
            cout << itm2->first << " " << itm2->second << endl;
        }
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
