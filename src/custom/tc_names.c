/*****************************************************************************
 *
 * Module: tc_names
 *
 * Prefix: TC
 *
 * Description:
 *
 * This module implements type checking
 *
 *****************************************************************************/


#include "tc_names.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "free.h"
#include "str.h"

struct INFO {
  cctype type;
};

#define INFO_SET_TYPE(n, type)  ((n)->type = type)
#define INFO_GET_TYPE(n)  ((n)->type)

static info *MakeInfo(void)
{
  info *result;

  DBUG_ENTER( "MakeInfo");

  result = (info *)MEMmalloc(sizeof(info));

  INFO_SET_TYPE(info, cctype.T_unknown);

  DBUG_RETURN( result);
}

static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree( info);

  DBUG_RETURN( info);
}

cctype type_inference(node* expr, info *arg_info)
{
    TRAVdo(expr, arg_info);
    return INFO_GET_TYPE(arg_info);
}

node *TCglobaldec (node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCglobaldec");
    if GLOBALDEC_TYPE(arg_node) != type_inference(GLOBALDEC_EXPR(arg_node)){
        //@todo throw error
    }
        
    DBUG_RETURN( info);
}

node* TCbool(node *arg_node, info *arg_info)
{
    INFO_SET_TYPE(arg_info, cctype.T_bool);
}

node* TCfloat(node *arg_node, info *arg_info)
{
    INFO_SET_TYPE(arg_info, cctype.T_float);
}

node* TCnum(node *arg_node, info *arg_info)
{
    INFO_SET_TYPE(arg_info, cctype.T_int);
}

node* TCbinop(node *arg_node, info *arg_info) 
{
    TRAVdo(BINOP_LEFT(arg_node), arg_info);
    cctype t1 = INFO_GET_TYPE(arg_info);

    TRAVdo(BINOP_RIGHT(arg_node), arg_info);
    cctype t2 = INFO_GET_TYPE(arg_info);

    if (t1 != t2) {
        //@todo produce error
    }
    else {
        INFO_SET_TYPE(arg_info, t1);
    }

}

main() {
    info *arg_info;

    DBUG_ENTER("type_inference");

    arg_info = MakeInfo();

    //@todo something here
    TRAVpush(TC_si);
    syntaxtree = TRAVdo( syntaxtree, arg_info);
    TRAVpop();

    arg_info = FreeInfo( arg_info);

}