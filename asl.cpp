/**
This Program was written by:
Tamer Elsawaf, cssc0445, RedID: 821268507
Drew Meade, cssc0403, RedID: 822332778
CS 530 / Professor Lenoard / Project 2
 **/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <algorithm>
#include <iomanip>
#include <map>
#include <stdlib.h>


using namespace std;


// insertion order for the ESTAB
vector<string> insertionOrder;

// store all of the instructions as strings with each vector
// used an instruction line and stored in main vector
vector<vector<string> > instrucL;


// updates the memory when reading source listing files
unsigned int LOC = 0;
string csect;

// partitioning input source code into tokens
vector<string> getToken(const string str, char delim) {
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
// to format each line entered into ESTAB
struct formatESTAB{
    string controlSec;
    string instruction;
    unsigned int address;
    unsigned int length;    
};

// map holding symbol and corresponding address
map<string,formatESTAB> ESTmap;

// handles immediate addressing to seperate instruction and operand
vector<string> splitString(const string str){
    vector<string> tokens;
    string token;
    for (int i=0; i<str.length(); i++)
    {
        if (isdigit(str[i]))
            token.push_back(str[i]);
        else if((str[i] >= 'A' && str[i] <= 'Z') ||
                (str[i] >= 'a' && str[i] <= 'z'))
            token.push_back(str[i]);
        else{
            if(token != ""){
                tokens.push_back(token);
                token.clear();
            }
        }
    }
    if(str!="")
        tokens.push_back(token);
    return tokens;
}


void write2ESTAB(){
    ofstream outputESTAB;
    outputESTAB.open("ESTAB.st");
    for(int i = 0; i < insertionOrder.size(); i++){
        string str = insertionOrder[i];
        if(str == "")
            continue;
        else if (ESTmap[str].instruction == ""){
            unsigned int len = ESTmap[str].length;
            unsigned int addy = ESTmap[str].address;

            stringstream addS;
            addS << hex << addy;
            string address( addS.str() );

            stringstream sizS;
            sizS << hex << len;
            string length( sizS.str() );


            outputESTAB << str
                 << "       "
                 << setw(0)
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
            string address( stream.str());

            outputESTAB << " "
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
    map<string,formatESTAB>::iterator it;
    for(it = ESTmap.begin(); it != ESTmap.end(); it++){
        string reference = it->first;
        
        string controlSec = ESTmap[reference].controlSec;
        string instruction = ESTmap[reference].instruction;
        unsigned int address = ESTmap[reference].address;
        unsigned int length = ESTmap[reference].length;
    }
}

// parse through scanned source code
void buildESTAB(vector<string> vec, string instruction){
    // ---- FOR TESTING PURPOSES ----
    // for (int i = 0; i < vec.size(); i++)
    // {
    //     cout << vec[i] << endl; 
    // }
  
    formatESTAB data;
    
    if(vec.size() == 0 || vec[0] == ".")
        return;
    else if(vec.size() < 3)     //When encounting the end record
        return;
    else if(vec[2] == "START"){
        unsigned int address;
        unsigned int length;
        istringstream converter(vec[0].c_str());
        converter >> hex >> address;
        istringstream cast(vec[2].c_str());
        cast >> hex >> length;

        csect = vec[1];
        data.controlSec = csect;
        data.address = address + LOC;
        ESTmap[csect] = data;
        insertionOrder.push_back(csect);
    }
    else if(vec[1] == "EXTDEF"){

        vector<string> temp = getToken(vec[2], ',');
        for(vector<string>::size_type i = 0; i <= temp.size()-1; i++){
            unsigned int address;
            istringstream converter(vec[0].c_str());
            converter >> hex >> address;
            
            data.address = address;
            data.controlSec = csect;
            data.instruction = temp[i];
            ESTmap[temp[i]] = data;



            if(find(insertionOrder.begin(), insertionOrder.end(), 
                    temp[i]) != insertionOrder.end()){
                return;            
            } else {
                insertionOrder.push_back(temp[i]);
            }

        }
        

    }
    else if(vec[2] == "WORD" || vec[2] == "RESW"){
        string arg = vec[1];
        if(ESTmap.count(arg)){
            unsigned int address;
            istringstream converter(vec[0].c_str());
            converter >> hex >> address;
            // cout << address << endl;

            data.controlSec = csect;
            data.address = address + LOC;
            data.instruction = arg;
            ESTmap[arg] = data;
            if(find(insertionOrder.begin(), insertionOrder.end(), arg) 
                    != insertionOrder.end()){
                return;
            } else {
                insertionOrder.push_back(arg);
            }
        }
    }
    else if(vec[2] == "=C'EOF'"){
        unsigned int length = atoi(vec[0].c_str());
        istringstream converter(vec[0].c_str());
        converter >> hex >> length;
        
        ESTmap[csect].length = length+3;
        // cout << "ESTAB Length " << hex << ESTmap[csect].length << endl;
        if(LOC == 0)
            LOC = length+3;
        else
            LOC += length;
    }
    return;
}
// handling first line of object record to find project name
void headRec(vector<vector<string> > indin, string file){
    string temp = file.substr(0, file.find(".",0));
    temp += ".obj";


    ofstream outputFile;
    outputFile.open(temp.c_str());

    for(int i = 0; i < indin.size(); i++){

        if(indin[i][0] == ".")       
            continue;
        else if(indin[i].size() < 3) 
            continue;
        else if(indin[i][2] == "START"){
            string reference = indin[i][1];
            unsigned int address = ESTmap[reference].address;
            unsigned int length = ESTmap[reference].length;
            
            stringstream addS;
            stringstream sizS;
            addS << hex << address;
            string addy(addS.str());
            sizS << hex << length;
            string len(sizS.str());

            outputFile << "H"
                       << reference;
            for(int j = 6; j > reference.length(); j--){
                outputFile << " ";
            }
            outputFile 
            << setfill('0')
            << setw(6)
            << addy
            << setw(6)
            << len
            << endl;
        }
    }
    outputFile.close();
    return;
}
// writing external defined symbols to object file
void defRecord(vector<vector<string> > indin, string file){
    string temp = file.substr(0, file.find(".",0));
    temp += ".obj";

    ofstream outputFile;
    outputFile.open(temp.c_str(), ios::app);

    for(int i = 0; i < indin.size(); i++){
        if(indin[i][0] == ".")
            continue;
        else if(indin[i].size() < 3)
            continue;
        else if(indin[i][1] == "EXTDEF"){
            outputFile << "D";
            vector<string> symbols = getToken(indin[i][2], ',');
            for(int i = 0; i < symbols.size(); i++){
                unsigned int address = ESTmap[symbols[i]].address;
                
                stringstream stream;
                stream << hex << address;
                string addy(stream.str());

                outputFile << symbols[i];
                for (int j = 6; j > symbols[i].length(); j--)
                {
                    outputFile << " ";
                }
                           outputFile << setw(6)
                           << setfill('0')
                           << addy;
            }
            outputFile << endl;
        }
    }
    
    outputFile.close();
    return;
}
void refRecord(vector<vector<string> > indin, string file){
    string temp = file.substr(0, file.find(".",0));
    temp += ".obj";

    ofstream outputFile;
    outputFile.open(temp.c_str(), ios::app);

    for(int i = 0; i < indin.size(); i++){
        if(indin[i][0] == ".")
            continue;
        else if(indin[i].size() < 3)
            continue;
        else if(indin[i][1] == "EXTREF")
        {
            outputFile << "R";
            
            vector<string> symbols = getToken(indin[i][2], ',');
            for(int j = 0; j < symbols.size(); j++){
                if(symbols[j].size() == 6){
                    outputFile << symbols[j] << " ";
                }
                else{
                    outputFile << symbols[j] << " ";
                }
               
        

            }
            outputFile << endl;
           
        }
        
    }
    
    outputFile.close();
    return;
}
// writing text record
void textRecord(vector<vector<string> > indin, string file){
    string temp = file.substr(0, file.find(".",0));
    temp += ".obj";
    ofstream outputFile;
    outputFile.open(temp.c_str(), ios::app);

    const unsigned int textSize = 30;
    unsigned int size=0;
    unsigned int memory=0;
    unsigned int tempMem=0;
    vector<string> codes;
    for(int i = 4; i < indin.size(); i++){
        
        if(indin[i][0] == ".")             
            continue;
        if(indin[i][2] == "WORD" || indin[i][2] == "RESW")
            break;
        else if(indin[i].size() < 4)       
            continue;
        
        while(true){

            if(i == indin.size())                
                break;
            if(indin[i][2] == "WORD" || indin[i][2] == "RESW")
                break;
            if(indin[i][0] == "."){              
                i++; continue;
            }
            else if(indin[i].size() < 4){      
                i++; continue;
            }

            if(indin[i].size()==5){            
                string str = indin[i][4];
                if(str.size() > 6){
                    if(size+4 <= textSize){
                        codes.push_back(str);
                        size+=4; memory+=4; i++;
                    } else
                        break;
                }else{
                    if(size+3 <= textSize){
                        codes.push_back(str);
                        size+=3; memory+=3; i++;
                    }else
                        break;
                }
            }else{                                 
                string str = indin[i][3];
                if(str.size() > 6){
                    if(size+4 <= textSize){
                        codes.push_back(str);
                        size+=4; memory+=4; i++;
                    }else
                        break;
                }else{
                    if(size+3 <= textSize){
                        codes.push_back(str);
                        size+=3; memory+=3; i++;
                    }else
                        break;
                }
            }
        }
        stringstream memStream;
        memStream << hex << tempMem;
        string MEM(memStream.str());

        stringstream sizS;
        sizS << hex << size;
        string SZ(sizS.str());

        outputFile << "T" 
             << setw(6) 
             << setfill('0') 
             << MEM 
             << setw(2) 
             << setfill('0') 
             << SZ;

        for(int j = 0; j < codes.size(); j++){
            outputFile << codes[j];
        }
        outputFile << endl;
        
        tempMem = memory;
        size = 0;
        codes.clear();
        i--;
    }

    int programSize = indin.size()-1;
    unsigned int programLength = ESTmap[csect].length;
    stringstream streamC;
    streamC << hex << programLength;
    string csectLength(streamC.str());
        outputFile << "T"
                   << setw(6) << setfill('0')
                   << csectLength
                   << "03"
                   << setw(6) << setfill('0')
                   << indin[programSize][3]
                   << endl;

    outputFile.close();
    return;
}

// finding what addresses need modification depending on if extended instruction
// or if symbol used is an external reference
void modrx(vector<string> symbols, vector<vector<string> > indin,string file, int i, int index){

    string temp = file.substr(0, file.find(".",0));
    temp += ".obj";    
    ofstream outputFile;
    outputFile.open(temp.c_str(), ios::app);

    string str = indin[i][index];
    vector<string> args = splitString(str);

    
    if(args.size() == 0 || args[0] == ""){
        return;
    }
    
    for(int j = 0; j < symbols.size(); j++){
        for(int k = 0; k < args.size(); k++){
            if(symbols[j] == args[k]){

                unsigned int address;
                istringstream converter(indin[i][0].c_str());
                converter >> hex >> address;
                address+=1;

                stringstream stream;
                stream << hex << address;
                string addy(stream.str());

                outputFile << "M"
                        << setw(6)
                        << setfill('0')
                        << addy
                        << "05"
                        << "+"
                        << args[k]
                        << endl;
            }
        }
    }
    outputFile.close();
    return;
}
// formatting modification record for format 4
void mod4Rec(vector<string> symbols, 
        vector<vector<string> > indin,
        string file, int i, int index){

    string temp = file.substr(0, file.find(".",0));
    temp += ".obj";    
    ofstream outputFile;
    outputFile.open(temp.c_str(), ios::app);

    string str = indin[i][index];
    vector<string> args = splitString(str);
    
    if(args.size() == 0 || args[0] == ""){
        return;
    }

    //if EXTREF do not print format 4 since already handles 
    if(find(symbols.begin(), symbols.end(), 
            args[0]) != symbols.end()){
        return;     
    }

    unsigned int address;
    istringstream converter(indin[i][0].c_str());
    converter >> hex >> address;
    address+=1;

    stringstream stream;
    stream << hex << address;
    string addy(stream.str());

    outputFile << "M"
            << setw(6)
            << setfill('0')
            << addy
            << "05"
            << "+"
            << csect
            << endl;

    outputFile.close();
    return;
}
// function for generating modification record
void modRec(vector<vector<string> > indin, string file){

    const unsigned int size = 30;
    unsigned int count;
    vector<string> symbols;
    for(int i = 0; i < indin.size(); i++){
        if(indin[i][0] == ".")         
            continue;
        else if(indin[i].size() < 3)
            continue;
        else if(indin[i][1] == "EXTREF"){  
            symbols = getToken(indin[i][2], ',');
        }
        else if(symbols.size() != 0){           //enter if external refs present
            
            if(indin[i].size() == 5){                   
                modrx(symbols,indin,file,i,3);
            } else if(indin[i].size() == 4){            
                modrx(symbols,indin,file,i,2);
            }
            int temp = indin[i].size()-1;
            if(indin[i][temp].size() > 6)        
                mod4Rec(symbols,indin,file,i,2);

        }
    }
    return;
}
// creating end record depending on final instruction
// void endRecord(vector<vector<string> > indin, string file){
//     string temp = file.substr(0, file.find(".",0));
//     temp += ".obj";
//     cout << indin[indin.size()-1][0] << endl;
//     ofstream outputFile;
//     outputFile.open(temp.c_str(), ios::app);

//     bool secondPass = false;
//     string symbol;
//     for(int i = 0; i < indin.size(); i++){
//         if(indin[i][0] == ".")
//             continue;
//         if(indin[i][0] != "END")
//             continue;
//         else if(indin[i].size() == 1)
//             outputFile << "E" << endl;
//         else {
//             symbol = indin[i][1];
//             secondPass = true;
//         }
//     }
//     if(secondPass){
//         string address = "";
//         for(int j = 1; j < indin.size(); j++){
//             if(indin[j][1] == symbol)
//                 address = indin[j][0];
//             if(address != ""){
                
//                 outputFile << "E"
//                            << address
//                            << endl;
//                 break;
//             }
//         }
//     }
//     outputFile.close();
//     return;
// }
void endRecord(vector<vector<string> > indin, string file) {
    string temp = file.substr(0, file.find(".",0));
    temp += ".obj";

    ofstream outputFile;
    outputFile.open(temp.c_str(), ios::app);

    string endRecord, endAddress;
    for (int i = 0; i < indin.size(); i++) {
        for (int j = 0; j < indin[i].size(); j++) {
            if (indin[i][j] == "END") {
                temp = indin[i][j + 1];
            }
        }
    }

    if (temp.empty()) {
        outputFile << "E" << endl;
    } else {
        for (int i = 0; i < indin.size(); i++) {
            if (indin[i][1] == temp) {
                endAddress = indin[i][0];
                stringstream ss;
                ss << "E" << setw(6) << setfill('0') << endAddress;
                endRecord = ss.str();
                outputFile << endRecord;
            }
        }
    }
    outputFile.close();
    return;

}

// read in source code for listing file and call functions for 
// parsing and creating listing file
int readFileESTAB(const char* input){
    ifstream file(input);
    string line;
    vector<string> temp;
    if (file.is_open()) {
        while (getline(file, line)) {
            temp = getToken(line, ' ');
            instrucL.push_back(temp);
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
// handle source code and call record functions for writing complete object file
void handleObj(const char* input){

    string temp = input;
    string fileName = temp.substr(0,temp.find(".",0));

    ifstream file(input);
    string line;
    vector<vector<string> > indin;
    vector<string> sourceCode;
    if (file.is_open()) {
        while (getline(file, line)) {
            vector<string> temp;
            temp = getToken(line, ' ');
            indin.push_back(temp);
            sourceCode.push_back(line);
        }
    }
    file.close();

    // call functions for creating object file
    headRec(indin, input);
    defRecord(indin, input);
    refRecord(indin, input);
    textRecord(indin, input);
    modRec(indin, input);
    endRecord(indin, input);
    
    return;
}
// taking input from user and calling functions for creating object file and ESTAB
int main(int argc, char *argv[]){
     printf("You submitted %d listing file(s).\n", argc-1);
     if(argc < 2){
        cout << "You did not include a .lis file" << endl;
        return 0;
    } else if(argc > 4){
        cout << "Provide only 1-4 .lis files to convert" << endl;
        return 0;
    }
    for(int i = 1; i < argc; i++){
        readFileESTAB(argv[i]);
        handleObj(argv[i]);
    }

    return 0;
}

