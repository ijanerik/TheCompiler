// Check if all the variables are correctly defined
/*
// First traverse trough
// -> declarations (symbol table)
//    -> fundef (symbol table)
         -> vardecs
         -> localfunctions (just like fundef)

         -> funcall
         -> assignVar
         -> varCall
      -> globaldec -> add to table

*/

// Voeg VarRead/VarCall toe aan AST en Expr


#ifndef _CA_NAMES_H_
#define _CA_NAMES_H_
#include "types.h"

extern node *CANbinop (node *arg_node, info *arg_info);
extern node *CANdoNames( node *syntaxtree);

#endif




