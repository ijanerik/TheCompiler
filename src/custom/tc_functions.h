#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"

node* TCFbool(node *arg_node, info *arg_info);
node* TCFfloat(node *arg_node, info *arg_info);
node* TCFnum(node *arg_node, info *arg_info);
node *TCFvarcall(node *arg_node, info *arg_info);
node *TCFfuncall(node *arg_node, info *arg_info);
node* TCFbinop(node *arg_node, info *arg_info);
node *TCFcastexpr(node *arg_node, info *arg_info);
node *TCFreturnstmt(node *arg_node, info *arg_info);
node* TCFfundef(node *arg_node, info *arg_info);
node *TCFdoFunctions( node *syntaxtree);
