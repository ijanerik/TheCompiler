#include "st_utils.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"

node *CANprogram(node *arg_node, symboltables *tables);
node *CANvardec(node *arg_node, symboltables *tables);
node *CANfundef(node *arg_node, symboltables *tables);
node* CANfunbody(node *arg_node, symboltables *tables);
node *CANglobaldec(node *arg_node, symboltables *tables);
node *CANglobaldef(node *arg_node, symboltables *tables);
node *CANvarcall(node *arg_node, symboltables *tables);
node *CANparam(node *arg_node, symboltables *tables);
node *CANarrayindex(node *arg_node, symboltables *tables);
node *CANfuncall(node *arg_node, symboltables *tables);
node *CANassign(node *arg_node, symboltables *tables);
node *CANforstmt(node *arg_node, symboltables *tables);
node *CANdoNames(node *syntaxtree);
