# IFJ project

This was my school project in course IFJ (Formal Languages and Compilers) in VUT FIT. It's a command line program written in C. 
We had defined syntax and sematics of two languages: IFJ18 (ruby like) and IFJcode18 (assembly language like). 
Our tak was to get any code written in IFJ18 and "translate" it to IFJcode18 and yet keep functionality the same.
Many parts of program is implemented as Finite Automata. 
User provide IFJ18 code to STDIN and IFJcode18 will be written to STDOUT.

Project details are in doc/dokumentace.pdf.

This was team project and my part was Parser (parser.c) and Generating IFJcode18 (code_gen.c).
Credits of other parts goes here Kristian Glos, Jakub Vins, Zhamilya Abikenova.

## Usage
```
make all

./ifj_project < "code written in IFJ18"
```
