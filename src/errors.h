//
// Created by Ancient on 25. 11. 2018.
//

#ifndef IFJ_PROJECT_ERRORS_H
#define IFJ_PROJECT_ERRORS_H

#define COMPILE_OK 0                 //pokial prebehne preklad bez chyby
#define LEX_ERROR 1                  //chyba v lexikalnej analyze
#define SYNTAX_ERROR 2               //chyba v syntaxe
#define SEMANTIC_ERROR 3             //semanticka chyba (nedefinovana fukncia, premenna)
#define COMPATIBILITY_ERROR 4        //chyba typove kompatiblity
#define PARAM_ERROR 5                //zly pocet parametrov u volania funkcie
#define DIFF_ERROR 6                 //ostatne semanticke chyby
#define ZERO_ERROR 9                 //delenie nulou
#define INNER_ERROR 99               //interna chyba ( chyba alokacie pamati..)

#endif //IFJ_PROJECT_ERRORS_H
