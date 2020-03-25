// ====================================================
// Jason Williamson (20552360)
// CS 241 Winter 2016
// Assignment 04, Problem 5
// File: asm.cc
// ====================================================
//

#include "kind.h"
#include "lexer.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <cstdio>
#include <sstream>
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
using std::istringstream;
using std::ostringstream;

typedef pair<string,string> state_token_key_type;
typedef map<state_token_key_type, string> state_map_type;
typedef pair<state_token_key_type, string> state_value_type;


const string ID = "ID";
const string INT = "INT";
const string LABEL = "LABEL";
const string HEXINT = "HEXINT";
const string REGISTER = "REGISTER";
const string COMMA = "COMMA";

const string START = "START";
const string ERROR = "ERROR";
const string DOTWORD = "DOTWORD";
const string DOTWORD_READ = "DOTWORD_READ";

const string JR_READ = "JR_READ";
const string JALR_READ = "JALR_READ";

const string ADD_R1 = "ADD_R1";
const string ADD_R1_C = "ADD_R1_C";
const string ADD_R2 = "ADD_R2";
const string ADD_R2_C = "ADD_R2_C";
const string ADD_R3 = "ADD_R3";

const string SUB_R1 = "SUB_R1";
const string SUB_R1_C = "SUB_R1_C";
const string SUB_R2 = "SUB_R2";
const string SUB_R2_C = "SUB_R2_C";
const string SUB_R3 = "SUB_R3";

const string SLT_R1 = "SLT_R1";
const string SLT_R1_C = "SLT_R1_C";
const string SLT_R2 = "SLT_R2";
const string SLT_R2_C = "SLT_R2_C";
const string SLT_R3 = "SLT_R3";

const string SLTU_R1 = "SLTU_R1";
const string SLTU_R1_C = "SLTU_R1_C";
const string SLTU_R2 = "SLTU_R2";
const string SLTU_R2_C = "SLTU_R2_C";
const string SLTU_R3 = "SLTU_R3";

const string BEQ_R1 = "BEQ_R1";
const string BEQ_R1_C = "BEQ_R1_C";
const string BEQ_R2 = "BEQ_R2";
const string BEQ_R2_C = "BEQ_R2_C";
const string BEQ_R3I = "BEQ_R3I";

const string BNE_R1 = "BNE_R1";
const string BNE_R1_C = "BNE_R1_C";
const string BNE_R2 = "BNE_R2";
const string BNE_R2_C = "BNE_R2_C";
const string BNE_R3I = "BNE_R3I";

const string LIS_R1 = "LIS_R1";
const string MFLO_R1 = "MFLO_R1";
const string MFHI_R1 = "MFHI_R1";

const string BY_CASE = "BY_CASE";
const string JR_ID = "jr";
const string JALR_ID = "jalr";
const string ADD_ID = "add";
const string SUB_ID = "sub";
const string SLT_ID = "slt";
const string SLTU_ID = "sltu";
const string BNE_ID = "bne";
const string BEQ_ID = "beq";
const string LIS_ID = "lis";
const string MFLO_ID = "mflo";
const string MFHI_ID = "mfhi";

void output_word(int word){
    putchar(word >> 24);
    putchar(word >> 16);
    putchar(word >> 8);
    putchar(word);
}

void output_lis_mflo_mfhi(int d, string type){
    int identity;
    if (LIS_R1 == type) {
        identity = 20;
    }else if (MFLO_R1 == type){
        identity = 18;
    }else{
        identity = 16;
    }
    putchar(0);
    putchar(0);
    putchar((d << 3) &0xff);
    putchar(identity);
}

void output_add_sub_slt_sltu(int d, int s, int t, string type){
    int identity;
    if (ADD_R3 == type) {
        identity = 32;
    }else if (SUB_R3 == type){
        identity = 34;
    }else if (SLT_R3 == type){
        identity = 42;
    }else{
        identity = 43;
    }
    
    putchar((s >> 3) &0xff);
    putchar(((s << 5) | (t)) &0xff);
    putchar((d << 3) &0xff);
    putchar(identity);
}

void output_branch_type(int s, int t, int i, string type){
    int identity;
    if (BEQ_R3I == type) {
        identity = 16;
    }else{
        identity = 20;
    }
    putchar(((identity) | (s >> 3)) &0xff);
    putchar(((s << 5) | (t)) &0xff);
    putchar( i >> 8 );
    putchar( i );
}

void jump_output_by_type(string type, int s){
    int identity;
    if (JR_READ == type) {
        identity = 8;
    }else{
        identity = 9;
    }
    putchar((s >> 3 ) &0xff);
    putchar((s << 5 ) &0xff);
    putchar(0);
    putchar(identity);
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

string stateLookup(state_map_type &stateMap, string state, string token){
    state_map_type::iterator it = stateMap.find(state_token_key_type(state, token));
    if (it != stateMap.end()) {
        return (*it).second;
    }
    return "ERROR";
}

int stringToInteger(string str){
    istringstream buffer(str);
    int value;
    if(buffer >> value){
        return value;
    }else{
        throw "ERROR: not a valid integer: " + str;
    }
    return 0;
}

string numberToString(int num){
    ostringstream convert;
    convert << num;
    return convert.str();
}

bool isValidRegister(int val){
    if ((val >= 0) && (val <= 31)) {
        return true;
    }else{
        string msg = numberToString(val);
        throw "ERROR: not a valid register: " + msg;
    }
    return false;
}

bool isValid16BitSigned(int num){
    if (num <= 0xffff) {
        if ((num <= 32768) || (num == 0xffff)){
            return true;
        }else{
            string msg = numberToString(num);
            throw "ERROR: not a valid 16bit signed number : " + msg;
        }
    }else{
        if(num <= 32767) {
            return true;
        }else{
            string msg = numberToString(num);
            throw "ERROR: not a valid 16bit signed number : " + msg;
        }
    }
    return false;
}

bool isValid16BitUnsigned(int num){
    if ((num <= 65535) && (num >= 0)) {
        return true;
    }else{
        string msg = numberToString(num);
        throw "ERROR: not a valid 16bit unsigned number : " + msg;
    }
    return false;
}

string getStateByCase(string id_value){
    string stateValue;
    if (JR_ID == id_value) {
        stateValue = JR_READ;
    }else if (JALR_ID == id_value){
        stateValue = JALR_READ;
    }else if (ADD_ID == id_value){
        stateValue = ADD_R1;
    }else if (SUB_ID == id_value){
        stateValue = SUB_R1;
    }else if (SLT_ID == id_value){
        stateValue = SLT_R1;
    }else if (SLTU_ID == id_value){
        stateValue = SLTU_R1;
    }else if (BNE_ID == id_value){
        stateValue = BNE_R1;
    }else if (BEQ_ID == id_value){
        stateValue = BEQ_R1;
    }else if (LIS_ID == id_value){
        stateValue = LIS_R1;
    }else if (MFLO_ID == id_value){
        stateValue = MFLO_R1;
    }else if (MFHI_ID == id_value){
        stateValue = MFHI_R1;
    }else{
        throw "ERROR: ID value not valid: " + id_value;
    }
    return stateValue;
}

int main(int argc, char* argv[]){
    
    
    state_map_type stateMap;
    stateMap.insert(state_value_type(state_token_key_type(START, DOTWORD), DOTWORD_READ));
    stateMap.insert(state_value_type(state_token_key_type(START, LABEL), START));
    stateMap.insert(state_value_type(state_token_key_type(START, ID), BY_CASE));
    stateMap.insert(state_value_type(state_token_key_type(DOTWORD_READ, ID), START));
    stateMap.insert(state_value_type(state_token_key_type(DOTWORD_READ, INT), START));
    stateMap.insert(state_value_type(state_token_key_type(DOTWORD_READ, HEXINT), START));
    stateMap.insert(state_value_type(state_token_key_type(JR_READ, REGISTER), START));
    stateMap.insert(state_value_type(state_token_key_type(JALR_READ, REGISTER), START));
    
    stateMap.insert(state_value_type(state_token_key_type(ADD_R1, REGISTER), ADD_R1_C));
    stateMap.insert(state_value_type(state_token_key_type(ADD_R1_C, COMMA), ADD_R2));
    stateMap.insert(state_value_type(state_token_key_type(ADD_R2, REGISTER), ADD_R2_C));
    stateMap.insert(state_value_type(state_token_key_type(ADD_R2_C, COMMA), ADD_R3));
    stateMap.insert(state_value_type(state_token_key_type(ADD_R3, REGISTER), START));
    
    stateMap.insert(state_value_type(state_token_key_type(SUB_R1, REGISTER), SUB_R1_C));
    stateMap.insert(state_value_type(state_token_key_type(SUB_R1_C, COMMA), SUB_R2));
    stateMap.insert(state_value_type(state_token_key_type(SUB_R2, REGISTER), SUB_R2_C));
    stateMap.insert(state_value_type(state_token_key_type(SUB_R2_C, COMMA), SUB_R3));
    stateMap.insert(state_value_type(state_token_key_type(SUB_R3, REGISTER), START));
    
    stateMap.insert(state_value_type(state_token_key_type(SLT_R1, REGISTER), SLT_R1_C));
    stateMap.insert(state_value_type(state_token_key_type(SLT_R1_C, COMMA), SLT_R2));
    stateMap.insert(state_value_type(state_token_key_type(SLT_R2, REGISTER), SLT_R2_C));
    stateMap.insert(state_value_type(state_token_key_type(SLT_R2_C, COMMA), SLT_R3));
    stateMap.insert(state_value_type(state_token_key_type(SLT_R3, REGISTER), START));
    
    stateMap.insert(state_value_type(state_token_key_type(SLTU_R1, REGISTER), SLTU_R1_C));
    stateMap.insert(state_value_type(state_token_key_type(SLTU_R1_C, COMMA), SLTU_R2));
    stateMap.insert(state_value_type(state_token_key_type(SLTU_R2, REGISTER), SLTU_R2_C));
    stateMap.insert(state_value_type(state_token_key_type(SLTU_R2_C, COMMA), SLTU_R3));
    stateMap.insert(state_value_type(state_token_key_type(SLTU_R3, REGISTER), START));
    
    stateMap.insert(state_value_type(state_token_key_type(BEQ_R1, REGISTER), BEQ_R1_C));
    stateMap.insert(state_value_type(state_token_key_type(BEQ_R1_C, COMMA), BEQ_R2));
    stateMap.insert(state_value_type(state_token_key_type(BEQ_R2, REGISTER), BEQ_R2_C));
    stateMap.insert(state_value_type(state_token_key_type(BEQ_R2_C, COMMA), BEQ_R3I));
    stateMap.insert(state_value_type(state_token_key_type(BEQ_R3I, INT), START));
    stateMap.insert(state_value_type(state_token_key_type(BEQ_R3I, HEXINT), START));
    stateMap.insert(state_value_type(state_token_key_type(BEQ_R3I, ID), START));
    
    stateMap.insert(state_value_type(state_token_key_type(BNE_R1, REGISTER), BNE_R1_C));
    stateMap.insert(state_value_type(state_token_key_type(BNE_R1_C, COMMA), BNE_R2));
    stateMap.insert(state_value_type(state_token_key_type(BNE_R2, REGISTER), BNE_R2_C));
    stateMap.insert(state_value_type(state_token_key_type(BNE_R2_C, COMMA), BNE_R3I));
    stateMap.insert(state_value_type(state_token_key_type(BNE_R3I, INT), START));
    stateMap.insert(state_value_type(state_token_key_type(BNE_R3I, HEXINT), START));
    stateMap.insert(state_value_type(state_token_key_type(BNE_R3I, ID), START));
    
    stateMap.insert(state_value_type(state_token_key_type(LIS_R1, REGISTER), START));
    stateMap.insert(state_value_type(state_token_key_type(MFLO_R1, REGISTER), START));
    stateMap.insert(state_value_type(state_token_key_type(MFHI_R1, REGISTER), START));
    
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
        
        string currentState = START;
        string currentToken = "";
        int addressIncr = 4;
        int curAddress = 0;
        
        //****************************************************
        // PASS ONE:
        //****************************************************
        
        int lineCount = 0;
        int lineOfLastInstruction = -1;
        vector<vector<Token*> >::iterator it;
        for(it = tokLines.begin(); it != tokLines.end(); ++it){
            vector<Token*>::iterator it2;
            ++lineCount;
            for(it2 = it->begin(); it2 != it->end(); ++it2){
                
                currentToken = (*it2)->toString();
                
                //cout << "currentToken: " << currentToken << endl;
                //cout << "getLexeme: " << (*it2)->getLexeme() << endl;
                
                //START STATE:
                if ((START == currentState) && (lineCount != lineOfLastInstruction)) {
                    string stateValue = stateLookup(stateMap, currentState, currentToken);
                    
                    //LABEL ACCEPTING STATE: (condition token is label)
                    if(LABEL == currentToken){
                        string fullLabel = (*it2)->getLexeme();
                        string label = fullLabel.substr(0, fullLabel.size()-1);
                        if (lookup(symbolTable, label)) {
                            throw "ERROR: label already defined for: " + label;
                        }
                        symbolTable.insert(pair<string, int>(label, curAddress));
                    }
                    
                    //BY_CASE: (START recieved an ID, we determine if ID is valid here)
                    else if (BY_CASE == stateValue){
                        string id_value = (*it2)->getLexeme();
                        stateValue = getStateByCase(id_value);
                    }
                    
                    //ERROR: (conditions from START state not met)
                    else if (ERROR == stateValue){
                        throw "ERROR: Not a valid token at: " + currentToken;
                    }
                    currentState = stateValue;
                }
                
                //READ INTO ACCEPTING STATES:
                else if ((DOTWORD_READ == currentState) || (JR_READ == currentState) || (ADD_R3 == currentState) ||
                         (SUB_R3 == currentState) || (SLT_R3 == currentState) || (SLTU_R3 == currentState) ||
                         (BNE_R3I == currentState) || (BEQ_R3I == currentState) || (LIS_R1 == currentState) ||
                         (MFHI_R1 == currentState) || (MFLO_R1 == currentState)){
                    string stateValue = stateLookup(stateMap, currentState, currentToken);
                    if (ERROR == stateValue) {
                        throw "ERROR: expected valid token but got " + currentToken;
                    }
                    // ASSUMING REGISTER IS VALID.(we leave this for PASS 2 to determine)
                    lineOfLastInstruction = lineCount;
                    curAddress += addressIncr;
                    currentState = stateValue;
                }
                
                //TRANSITIONAL READ STATES:
                else{
                    string stateValue = stateLookup(stateMap, currentState, currentToken);
                    if (ERROR == stateValue) {
                        throw "ERROR: TRANSITIONAL expected valid token but got " + currentToken;
                    }
                    currentState = stateValue;
                }
            }
            
            //set of instructions for a line has ended, therefore should be in a START state
            if (START != currentState){
                throw "ERROR: expecting an instruction value: " + currentState;
            }
            
        }
        
        //****************************************************
        // PASS TWO:
        //****************************************************
        
        currentState = START;
        currentToken = "";
        int register1 = 0;
        int register2 = 0;
        int registerFinal = 0;
        int sixteenBitInt = 0;
        curAddress = 0;
        int pc = curAddress + addressIncr;
        
        for (it = tokLines.begin(); it != tokLines.end(); ++it) {
            vector<Token*>::iterator it2;
            for (it2 = it->begin(); it2 != it->end(); ++it2) {
                
                currentToken = (*it2)->toString();
                //cout << "currentState: " << currentState << endl;
                //cout << "currentToken: " << currentToken << endl;
                
                //START:(error handling in PASS ONE)
                if (START == currentState) {
                    string stateValue = stateLookup(stateMap, currentState, currentToken);
                    
                    //BY_CASE: we must identify the ID passed, and set the state manually before proceeding
                    if (BY_CASE == stateValue){
                        string id_value = (*it2)->getLexeme();
                        stateValue = getStateByCase(id_value);
                    }
                    currentState = stateValue;
                }
                
                //DOTWORD_READ: (output .word, if ID is read in perform symbol table lookup
                //               and convert otherwise just convert )
                //ACCEPTING
                else if (DOTWORD_READ == currentState){
                    int word;
                    string stateValue = stateLookup(stateMap, currentState, currentToken);
                    if (ID == currentToken) {
                        string label = (*it2)->getLexeme();
                        map<string, int>::iterator it = symbolTable.find(label);
                        if (it != symbolTable.end()) {
                            word = it->second;
                        }else{
                            throw "ERROR: label not found in symbol table under: " + label;
                        }
                    }else{
                        word = (*it2)->toInt();
                    }
                    output_word(word);
                    currentState = stateValue;
                    
                    curAddress += addressIncr;
                    pc += addressIncr;
                }
                
                //STATES that store a register1 for later
                //ADD_R1 || SUB_R1 || SLT_R1 || SLTU_R1 (accepting registers, we check for validity and store
                //BNE_R1 || BEQ_R1
                else if ((ADD_R1 == currentState) || (SUB_R1 == currentState) || (SLT_R1 == currentState)
                         || (SLTU_R1 == currentState) || (BNE_R1 == currentState) || (BEQ_R1 == currentState)){
                    string stateValue = stateLookup(stateMap, currentState, currentToken);
                    string token_value = (*it2)->getLexeme();
                    string register_value = token_value.substr(1, token_value.size());
                    register1 = stringToInteger(register_value);
                    isValidRegister(register1);
                    currentState = stateValue;
                }
                
                //STATES that store a register2 for later
                //ADD_R2 || SUB_R2 || SLT_R2 || SLTU_R2 (accepting registers, we check for validity and store
                //BNE_R2 || BEQ_R2
                else if ((ADD_R2 == currentState) || (SUB_R2 == currentState) || (SLT_R2 == currentState)
                         || (SLTU_R2 == currentState) || (BNE_R2 == currentState) || (BEQ_R2 == currentState)){
                    string stateValue = stateLookup(stateMap, currentState, currentToken);
                    string token_value = (*it2)->getLexeme();
                    string register_value = token_value.substr(1, token_value.size());
                    register2 = stringToInteger(register_value);
                    isValidRegister(register2);
                    currentState = stateValue;
                }
                
                //ACCEPTING STATES take in a register and output
                //JR_READ || JALR_READ || ADD_R3 || SUB_R3 || SLT_R3 || SLTU_R3
                else if ((JR_READ == currentState) || (JALR_READ == currentState) ||
                         (ADD_R3 == currentState) || (SUB_R3 == currentState) || (SLT_R3 == currentState) ||
                         (SLTU_R3 == currentState) || (LIS_R1 == currentState) || (MFLO_R1 == currentState) ||
                         (MFHI_R1 == currentState)){
                    string stateValue = stateLookup(stateMap, currentState, currentToken);
                    string token_value = (*it2)->getLexeme();
                    string register_value = token_value.substr(1, token_value.size());
                    registerFinal = stringToInteger(register_value);
                    isValidRegister(registerFinal);
                    if ((JR_READ == currentState) || (JALR_READ == currentState)) {
                        jump_output_by_type(currentState, registerFinal);
                    }else if((ADD_R3 == currentState) || (SUB_R3 == currentState) || (SLT_R3 == currentState) ||
                             (SLTU_R3 == currentState)){
                        output_add_sub_slt_sltu(register1, register2, registerFinal, currentState);
                    }else if ((LIS_R1 == currentState) || (MFLO_R1 == currentState) ||(MFHI_R1 == currentState)){
                        output_lis_mflo_mfhi(registerFinal, currentState);
                    }
                    currentState = stateValue;
                    register1 = 0;
                    register2 = 0;
                    registerFinal = 0;
                    
                    curAddress += addressIncr;
                    pc += addressIncr;
                }
                
                //ACCEPTING STATES take in integer and output
                else if ((BNE_R3I == currentState) || (BEQ_R3I == currentState)){
                    string stateValue = stateLookup(stateMap, currentState, currentToken);
                    
                    if (ID == currentToken) {
                        string label = (*it2)->getLexeme();
                        map<string, int>::iterator it = symbolTable.find(label);
                        if (it != symbolTable.end()) {
                            int labelAddress;
                            labelAddress = it->second;
                            sixteenBitInt = ((labelAddress - pc) / 4);
                        }else{
                            throw "ERROR: label not found in symbol table under: " + label;
                        }
                    }else{
                        sixteenBitInt = (*it2)->toInt();
                    }
                    isValid16BitSigned(sixteenBitInt);
                    output_branch_type(register1, register2, sixteenBitInt, currentState);
                    currentState = stateValue;
                    register1 = 0;
                    register2 = 0;
                    sixteenBitInt = 0;
                    
                    curAddress += addressIncr;
                    pc += addressIncr;
                }
                
                //TRANSITIONAL READ STATES:
                else{
                    string stateValue = stateLookup(stateMap, currentState, currentToken);
                    currentState = stateValue;
                }
            }
        }
        
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




