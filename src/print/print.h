
/**
 * @file print.h
 *
 * Functions to print node structures
 * 
 */

#ifndef _SAC_PRT_NODE_H_
#define _SAC_PRT_NODE_H_

#include "types.h"

extern node *PRTstmts (node * arg_node, info * arg_info);
extern node *PRTassign (node * arg_node, info * arg_info);
extern node *PRTvar (node * arg_node, info * arg_info);
extern node *PRTvarlet (node * arg_node, info * arg_info);
extern node *PRTbinop (node * arg_node, info * arg_info);
extern node *PRTfloat (node * arg_node, info * arg_info);
extern node *PRTnum (node * arg_node, info * arg_info);
extern node *PRTbool (node * arg_node, info * arg_info);
extern node *PRTsymboltableentry (node * arg_node, info * arg_info);
extern node *PRTerror (node * arg_node, info * arg_info);
extern node *PRTdoPrint( node *syntaxtree);

extern node *PRTident (node * arg_node, info * arg_info);
extern node *PRTvarcall (node * arg_node, info * arg_info);
extern node *PRTcastexpr (node * arg_node, info * arg_info);
extern node *PRTmonop (node * arg_node, info * arg_info);
extern node *PRTexprs (node * arg_node, info * arg_info);
extern node *PRTreturnstmt (node * arg_node, info * arg_info);
extern node *PRTforstmt (node * arg_node, info * arg_info);
extern node *PRTdowhilestmt (node * arg_node, info * arg_info);
extern node *PRTwhilestmt (node * arg_node, info * arg_info);
extern node *PRTifelsestmt (node * arg_node, info * arg_info);
extern node *PRTfuncall (node * arg_node, info * arg_info);
extern node *PRTblock (node * arg_node, info * arg_info);
extern node *PRTvardec (node * arg_node, info * arg_info);
extern node *PRTvardecs (node * arg_node, info * arg_info);
extern node *PRTfundefs (node * arg_node, info * arg_info);
extern node *PRTfunbody (node * arg_node, info * arg_info);
extern node *PRTparam (node * arg_node, info * arg_info);
extern node *PRTparams (node * arg_node, info * arg_info);
extern node *PRTglobaldef (node * arg_node, info * arg_info);
extern node *PRTglobaldec (node * arg_node, info * arg_info);
extern node *PRTfunheader (node * arg_node, info * arg_info);
extern node *PRTfundef (node * arg_node, info * arg_info);
extern node *PRTdeclarations (node * arg_node, info * arg_info);
extern node *PRTprogram (node * arg_node, info * arg_info);

//extern node *PRTsymboltables(node *arg_node, info *arg_info);
extern node *PRTsymboltable(node *arg_node, info *arg_info);
extern node *PRTsymboltableentry(node *arg_node, info *arg_info);
extern node *PRTarrayindex(node *arg_node, info *arg_info);
extern node* PRTcondexpr(node* arg_node, info *arg_info);
#endif /* _SAC_PRT_NODE_H_ */
