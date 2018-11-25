//
// Created by Ancient on 25. 11. 2018.
//

#ifndef IFJ_PROJECT_EXPRESSION_H
#define IFJ_PROJECT_EXPRESSION_H
typedef enum prec_table_symbols{
    P_PLUS,   // +
    P_MINUS,  // -
    P_MUL,    // *
    P_DIV,    // /
    P_EQ,     // ==
    P_NOT_EQ, // !=
    P_LESS_EQ,// <=
    P_MORE_EQ,// >=
    P_LESS,   // <
    P_MORE,   // >
    P_LEFT_PAR,// (
    P_RIGHT_PAR,// )
    P_ID,     // ID
    P_INT_NUM,// int
    P_FLOAT_NUM,// float
    P_STRING,// string
    P_DOLLAR,// $
    P_STOP,
    P_NON_TERM// non terminal

}Prec_table_symbols_enum;

typedef enum expr_rules{
               // E->
    E_plus_E, //    E + E
    E_minus_E, //   E - E
    E_mul_E,   //   E * E
    E_div_E,    //  E / E
    lbr_E_rbr,  //  E = (E)
    E_eq_E,     //  E == E
    E_not_eq_E, //  E != E
    E_less_E,   //  E < E
    E_more_E,   //  E > E
    E_less_eq_E,//  E <= E
    E_more_eq_E,//  E >= E
    operand,    //  i
    no_rule     //  no suitable rule


}Expr_rules_enum;
#endif //IFJ_PROJECT_EXPRESSION_H
