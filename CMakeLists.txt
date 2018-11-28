cmake_minimum_required(VERSION 3.9)
project(IFJ_project C)

set(CMAKE_C_STANDARD 99)

add_executable(IFJ_project  src/scanner.h src/scanner.c src/errors.h src/symtable.c src/symtable.h src/param_list.c src/param_list.h src/sym_stack.c src/sym_stack.h src/expressions.h src/expressions.c src/parser.c src/parser.h src/postfix_data_managment.c src/postfix_data_managment.h)