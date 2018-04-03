#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"

node* BOCbinop(node* arg_node, info *arg_info);
node* BOCdoCasts(node *syntaxtree);