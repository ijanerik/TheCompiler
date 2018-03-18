#include "st_utils.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"

node *CAVprogram(node *arg_node, symboltables *tables);
node *CAVvardec(node *arg_node, symboltables *tables);
node *CAVfundef(node *arg_node, symboltables *tables);
node* CAVfunbody(node *arg_node, symboltables *tables);
node *CAVglobaldec(node *arg_node, symboltables *tables);
node *CAVglobaldef(node *arg_node, symboltables *tables);
node *CAVvarcall(node *arg_node, symboltables *tables);
node *CAVparam(node *arg_node, symboltables *tables);
node *CAVarrayindex(node *arg_node, symboltables *tables);
node *CAVfuncall(node *arg_node, symboltables *tables);
node *CAVassign(node *arg_node, symboltables *tables);
node *CAVforstmt(node *arg_node, symboltables *tables);
node *CAVdoVariables(node *syntaxtree);
