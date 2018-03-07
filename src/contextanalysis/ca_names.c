/*****************************************************************************
 *
 * Module: sum_ints
 *
 * Prefix: SI
 *
 * Description:
 *
 * This module implements a demo traversal of the abstract syntax tree that
 * sums up all integer constants and prints the result at the end of the traversal.
 *
 *****************************************************************************/


#include "ca_names.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"


/*
 * INFO structure
 */

struct INFO {
    node* table_stack[32];
    int index;
};


/*
 * INFO macros
 */

#define INFO_STACK(n)  ((n)->table_stack)
#define INFO_INDEX(n)  ((n)->index)


/*
 * INFO functions
 */

static info *MakeInfo(void)
{
    info *result;

    DBUG_ENTER( "MakeInfo");

    result = (info *)MEMmalloc(sizeof(info));

    INFO_INDEX(result) = 0;

    DBUG_RETURN( result);
}

static info *FreeInfo( info *info)
{
    DBUG_ENTER ("FreeInfo");

    info = MEMfree( info);

    DBUG_RETURN( info);
}


// Function if a var/func name is in a symbol table


/*
 * Traversal functions
 */

node *CANvardec(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CANvardec");

    //INFO_SUM( arg_info) += NUM_VALUE(arg_node);

    DBUG_RETURN( arg_node);
}


/*
 * Traversal start function
 */

node *CANdoNames( node *syntaxtree)
{
    info *arg_info;

    DBUG_ENTER("SIdoSumIns");

    arg_info = MakeInfo();

    TRAVpush( TR_si);
    syntaxtree = TRAVdo( syntaxtree, arg_info);
    TRAVpop();

    CTInote( "Sum of integer constants: %d", INFO_SUM( arg_info));

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}
