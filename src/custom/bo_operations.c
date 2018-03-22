/*****************************************************************************
 *
 * Module: bo_operations
 *
 * Prefix: BO
 *
 * Description: Boolean operations
 *
 *****************************************************************************/

#include "bo_operations.h"
#include "errors.h"

node* BObinop(node* arg_node, info *arg_info)
{
    DBUG_ENTER("BObinop");

    binop op = BINOP_OP(arg_node);
    
    if (op != BO_and && op != BO_or) {
        DBUG_RETURN(arg_node);
    }

    BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);
    BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);
    
    node* expr = op == BO_and ? TBmakeCondexpr(BINOP_RIGHT(arg_node),
                                               TBmakeBool(FALSE),
                                               BINOP_LEFT(arg_node))
                              : TBmakeCondexpr(TBmakeBool(TRUE),
                                               BINOP_RIGHT(arg_node),
                                               BINOP_LEFT(arg_node));

    MEMfree(arg_node);
    arg_node = expr;

    DBUG_RETURN(arg_node);
}

node* BOdoOperations( node *syntaxtree)
{
    DBUG_ENTER("BodoOperations");

    TRAVpush(TR_bo);
    syntaxtree = TRAVdo( syntaxtree, NULL);
    TRAVpop();

    DBUG_RETURN( syntaxtree);
}