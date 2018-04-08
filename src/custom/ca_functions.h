#ifndef _CA_FUNC_H_
#define _CA_FUNC_H_

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"

node *CAFfundef(node *arg_node, info *tables);
node *CAFprogram(node *arg_node, info *tables);
node *CAFfuncall(node *arg_node, info *tables);
node *CAFreturnstmt(node *arg_node, info *tables);
node* CAFblock(node *arg_node, info *tables);
node* CAFifelsestmt(node *arg_node, info *tables);
node *CAFdoFunctions(node *syntaxtree);


#endif