# XE LINK-EDITOR

The XE link-editor program shall open SIC/XE assembler listing files and generate executable object file(s) for the XE machine and
ESTAB for the project (note, do not build a link-loader!).

## Input

Input - The user will start the program (the program shall be named "asl") and will
provide the listing file(s) as arguments on the command-line each separated by
spaces, i.e.:
“[cssc0000@edoras ~]$ asl first.lis second.lis third.list"

## Output

Output – The SIC (XE variant) object file(s) such as those found in figure 3.9 of the text.
Also, print the ESTAB in separate file (name.st) similar to
the ESTAB at the top of page 143 in the text.
