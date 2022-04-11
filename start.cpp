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

// updates the memory when reading listing files
unsigned int memLoc = 0;
string csectName;

// struct to hold format for ESTAB (control section name, instruction address
// and address of symbol)
struct ESTAB_format{
    string csect;
    string instruction;
    unsigned int address;
    unsigned int length;    
};
// This map holds the Symbol name as the key and address as the value
map<string,ESTAB_format> ESTmap;

// remembering order for the ESTAB
vector<string> estabOrder;

// for splitting the source coude into respective parts
vector<string> partition(const string str, char delim) {
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
// This function creates the listing file and writes the data to it
void write2ESTAB(){
    ofstream outputfile;

    // --- FOR TESTING ---
    // printf("ESTAB:\n");
    // printf("-------------------------\n");
    // for(int i = 0; i < estabOrder.size(); i++){
    //     outputfile << estabOrder[i] << ' ';
    //     outputfile << ESTmap[estabOrder[i]].csect << ' ';
    //     outputfile << ESTmap[estabOrder[i]].instruction << ' ';
    //     outputfile << ESTmap[estabOrder[i]].address << ' ';
    //     outputfile << ESTmap[estabOrder[i]].length << '\n';
    //     printf("%s %s %d %d\n", estabOrder[i].c_str(), ESTmap[estabOrder[i]].csect.c_str(), ESTmap[estabOrder[i]].address, ESTmap[estabOrder[i]].length);
    // }
    outputfile.open("ESTAB.st");
    for(int i = 0; i < estabOrder.size(); i++){
        string str = estabOrder[i];
        if(str == "")
            continue;
        else if (ESTmap[str].instruction == ""){
            unsigned int len = ESTmap[str].length;
            unsigned int addy = ESTmap[str].address;

            stringstream streamA;
            streamA << hex << addy;
            string address(streamA.str());

            stringstream streamB;
            streamB << hex << len;
            string length( streamB.str() );


            outputfile << str
                 << "       "
                 << setw(6)
                 << setfill('0')
                 << address
                 << " "
                 << setw(6)
                 << length
                 << endl;
        }
        else if(ESTmap[str].instruction != ""){
            stringstream stream;
            stream << hex << ESTmap[str].address;
            cout << ESTmap[str].address;
            string address( stream.str());

            outputfile << " "
                 << setw(10)
                 << setfill(' ')
                 << str
                 << " "
                 << setw(6)
                 << setfill('0')
                 << address
                 << endl;
        }
    }
    return;
}
void readESTAB(){
    map<string,ESTAB_format>::iterator iter;
    for(iter = ESTmap.begin(); iter != ESTmap.end(); iter++){
        string reference = iter->first;
        string csect = ESTmap[reference].csect;
        string instruction = ESTmap[reference].instruction;
        unsigned int address = ESTmap[reference].address;
        unsigned int length = ESTmap[reference].length;
    }
}

void buildESTAB(vector<string> vec, string instruction){

    for (int i = 0; i < vec.size(); i++)
    {
        cout << vec[i] << endl; 
    }

    ESTAB_format data;
    
    if(vec.size() == 0 || vec[0] == "."){}
    else if(vec.size() < 3){ }
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

        vector<string> temp = partition(vec[2], ',');
        for(vector<string>::size_type i = 0; i != temp.size(); i++){
            unsigned int address;
            istringstream stringConv(vec[0].c_str());
            stringConv >> hex >> address;

            data.address = address;
            data.csect = csectName;
            data.instruction = temp[i];
            ESTmap[temp[i]] = data;
       

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

int readinESTAB(const char* input){
    ifstream file(input);
    string line;
    vector<string> temp;
    if (file.is_open()) {
        while (getline(file, line)) {
            temp = partition(line, ' ');
            lines.push_back(temp);
            buildESTAB(temp,line);
        }
    }
    try{
        readESTAB();
    } catch(string error){
        cout << error << endl;
        exit(0);
    }
    write2ESTAB();
    file.close();
    return 0;
}

int main(int argc, char *argv[]) {
    printf("You submitted %d listing file(s).\n", argc-1);
     if(argc < 2){
        cout << "You did not include a .lis file" << endl;
        return 0;
    } else if(argc > 4){
        cout << "Provide only 1-4 .lis files to convert" << endl;
        return 0;
    }

    // read files into estab and object code generators
     for(int i = 1; i < argc; i++){
        readinESTAB(argv[i]);
        // readFileObjectFile(argv[i]);
    }
}