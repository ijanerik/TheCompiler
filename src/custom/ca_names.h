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

extern node *CANdoNames( node *syntaxtree);

extern node *CANvardec(node *arg_node, info *arg_info);
extern node *CANglobaldec(node *arg_node, info *arg_info);
extern node *CANglobaldef(node *arg_node, info *arg_info);
extern node *CANfundef(node *arg_node, info *arg_info);
extern node *CANassign(node *arg_node, info *arg_info);
extern node *CANvarcall(node *arg_node, info *arg_info);
extern node *CANfuncall(node *arg_node, info *arg_info);
extern node *CANfuncall(node *arg_node, info *arg_info);
extern node *CANparam(node *arg_node, info *arg_info);
extern node *CANforstmt(node *arg_node, info *arg_info);

#endif




