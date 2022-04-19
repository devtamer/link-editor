## This Program was written by:

- Tamer Elsawaf
- Drew Meade
- CS 530 Systems Programming / Professor Lenoard / Project 2: link-editor

## # XE LINK-EDITOR

The XE link-editor program shall open SIC/XE assembler listing files and generate executable object file(s) for the XE machine and
ESTAB for the project

## Compile using make

make

## To Compile in g++:

g++ -o asl asl.cpp

## Input:

The user will start the program (the program shall be named "asl") and will
provide the listing file(s) as arguments on the command-line each separated by
spaces, i.e.:

- asl first.lis second.lis third.list"
- Example of listing file (input)
  ![alt text](/testingFiles/listingfile_ex.png)

## Output

The SIC (XE variant) object file(s) such as those found in figure 3.9 of the text.
Also, print the ESTAB in separate file (name.st) similar to
the ESTAB at the top of page 143 in the text.

- Object File Example ![alt text](/testingFiles/objectfile_ex.png)
- (SIC/XE) ESTAB Example![alt text](/testingFiles/ESTAB_ex.png)

## Design Choices

When designing this linker-editor, we decided to use outputstreams as well as some other supporting libraries to achieve
the desired result when outputing ESTAB and object code files
