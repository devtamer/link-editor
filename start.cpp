/**
This Program was written by:
Tamer Elsawaf, cssc0445, RedID: 821268507
Drew Meade, cssc0403, RedID: 822332778
CS 530 / Professor Lenoard / Project 2
 **/

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <map>
#include <stdlib.h>
#include <vector>
#include <string>
using namespace std;


vector<vector<string> > lines;

// struct to hold table for ESTAB
struct ESTAB_format{
    string csect;
    string instruction;
    unsigned int address;
    unsigned int length;    
};

// This map holds the Symbol name as the key and address as the value
map<string,ESTAB_format> ESTmap;

// updates the memory when reading listing files
unsigned int memLoc = 0;
string csectName;

// remembering order for the ESTAB
vector<string> estabOrder;

vector<string> split(const string str, char delim) {
    vector<string> result;
    istringstream ss(str);
    string token;

    while (getline(ss, token, delim)) {
        if (!token.empty()) {
            result.push_back(token);
        }
    }
    return result;
}

void parseESTAB(){
    map<string,ESTAB_format>::iterator it;
    for(it = ESTmap.begin(); it != ESTmap.end(); it++){
        string reference = it->first;
        // print out the reference
        cout << reference << endl;

        string controlSection = ESTmap[reference].csect;
        string instruction = ESTmap[reference].instruction;
        unsigned int address = ESTmap[reference].address;
        unsigned int length = ESTmap[reference].length;

        if(instruction == "")
            continue;
        else {
            unsigned int lowerBound = ESTmap[controlSection].address;
            unsigned int upperBound = ESTmap[controlSection].length+lowerBound;

            if(address < lowerBound || address > upperBound){
                string exceptionMessage = "ERROR: You are "
                    "accessing illegal memory.";
                throw(exceptionMessage);
            }
        }
    }
}

void generateESTAB(vector<string> vec, string instruction){
    
    ESTAB_format data;
    
    if(vec.size() == 0 || vec[0] == ".")
        return;
    else if(vec.size() < 3)     //When encounting the end record
        return;
    else if(vec[2] == "START"){
        unsigned int address;
        unsigned int length;
        istringstream stringConv(vec[0].c_str());
        stringConv >> hex >> address;
        istringstream cast(vec[2].c_str());
        cast >> hex >> length;

        csectName = vec[1];
        data.csect = csectName;
        data.address = address + memLoc;
        ESTmap[csectName] = data;
        estabOrder.push_back(csectName);
    }
    else if(vec[1] == "EXTDEF"){

        vector<string> temp = split(vec[2], ',');
        for(vector<string>::size_type i = 0; i != temp.size(); i++){
            unsigned int address;
            istringstream stringConv(vec[0].c_str());
            stringConv >> hex >> address;

            data.address = address;
            data.csect = csectName;
            data.instruction = temp[i];
            ESTmap[temp[i]] = data;

            /*
             * Insert in vector if not already present
             */
            if(find(estabOrder.begin(), estabOrder.end(), 
                    temp[i]) != estabOrder.end()){
                return;            
            } else {
                estabOrder.push_back(temp[i]);
            }
        }
        

    }
    else if(vec[2] == "WORD" || vec[2] == "RESW"){
        string arg = vec[1];
        if(ESTmap.count(arg)){
            unsigned int address;
            istringstream stringConv(vec[0].c_str());
            stringConv >> hex >> address;

            data.csect = csectName;
            data.address = address + memLoc;
            data.instruction = arg;
            ESTmap[arg] = data;
            if(find(estabOrder.begin(), estabOrder.end(), arg) 
                    != estabOrder.end()){
                return;
            } else {
                estabOrder.push_back(arg);
            }
        }
    }
    else if(vec[2] == "=C'EOF'"){
        unsigned int length = atoi(vec[0].c_str());
        istringstream stringConv(vec[0].c_str());
        stringConv >> hex >> length;
        
        ESTmap[csectName].length = length+3;
        if(memLoc == 0)
            memLoc = length+3;
        else
            memLoc += length;
    }
    return;
}

int readforESTAB(const char* input){
    ifstream file(input);
    string line;
    vector<string> temp;
    if (file.is_open()) {
        while (getline(file, line)) {
            temp = split(line, ' ');
            lines.push_back(temp);
            generateESTAB(temp,line);
        }
    }
    try{
        parseESTAB();
    } catch(string error){
        cout << error << endl;
        exit(0);
    }
    

    // printESTAB();
    file.close();
    return 0;
}

int main(int argc, char *argv[]) {
     if(argc < 2){
        cout << "Error: Please specify a listing file to parse" << endl;
        return 0;
    } else if(argc > 4){
        cout << "Error: Please provide 1-4 listing files to parse" << endl;
        return 0;
    }

    // read files into estab and object code generators
     for(int i = 1; i < argc; i++){
        readforESTAB(argv[i]);
        // readFileObjectFile(argv[i]);
    }
}