/*****************************************************************************
 *
 * Module: tc_variables
 *
 * Prefix: TCV
 *
 * Description:
 *
 * This module implements type checking of variables
 *
 *****************************************************************************/

#include "tc_variables.h"

struct INFO {
  cctype current_type;
};

#define INFO_SET_TYPE(n, type)  ((n)->current_type = type)
#define INFO_GET_TYPE(n)  ((n)->current_type)

#define ERROR_TYPE_GLOBDEF "Variable %s does not match inferred type of the expression"
#define ERROR_TYPE_BINOP "Both sides of the expression do not have the same type"
char* cctypeToString(cctype type) {
    char* str;
    switch (type) {
        case T_float:
            str = "float";
            break;
        case T_int:
            str = "int";
            break;
        case T_bool:
            str = "bool";
            break;
        default:
            str = "unknown";
    }
    return str;
}


static info *MakeInfo(void)
{
    info *result;

    DBUG_ENTER( "MakeInfo");

    result = (info *)MEMmalloc(sizeof(info));

   
    INFO_SET_TYPE(result, T_unknown);

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

node *TCVglobaldef (node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCVglobaldef");

    node* entry = GLOBALDEF_SYMBOLTABLEENTRY(arg_node);
    cctype declared_type = SYMBOLTABLEENTRY_TYPE(entry);
    
    //@todo fix this global def stuff with int i = 1,2,3;
    node* exprs = GLOBALDEF_EXPRS(arg_node);

    if (exprs != NULL ) {
        node* expr = EXPRS_EXPR(exprs);
        cctype inferred_type = type_inference(expr, arg_info);

        if (declared_type != inferred_type){
            CTIerror(ERROR_TYPE_GLOBDEF, SYMBOLTABLEENTRY_NAME(entry));
        }
    } 

    DBUG_RETURN( arg_node);
}

node *TCVvardec(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCVvardec");

    CTIwarn("INDSIDE VARDEC\n");
    
    // node* entry = VARDEC_SYMBOLTABLEENTRY(arg_node);
    // cctype declared_type = SYMBOLTABLEENTRY_TYPE(entry);
    
    // //@todo fix this vardec stuff with int i = 1,2,3;
    // node* exprs = VARDEC_EXPRS(arg_node);

    // if (exprs != NULL ) {
    //     node* expr = EXPRS_EXPR(exprs);
    //     cctype inferred_type = type_inference(expr, arg_info);

    //     if (declared_type != inferred_type){
    //         CTIerror(ERROR_TYPE_GLOBDEF, SYMBOLTABLEENTRY_NAME(entry));
    //     }
    // } 

    DBUG_RETURN( arg_node);
}

node *TCVassign(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCVassign");
    
    node* entry = ASSIGN_SYMBOLTABLEENTRY(arg_node);
    cctype declared_type = SYMBOLTABLEENTRY_TYPE(entry);
    
    //@todo fix this vardec stuff with int i = 1,2,3;
    node* expr = ASSIGN_EXPR(arg_node);

    if (expr != NULL ) {
        cctype inferred_type = type_inference(expr, arg_info);

        if (declared_type != inferred_type){
            CTIerror(ERROR_TYPE_GLOBDEF, SYMBOLTABLEENTRY_NAME(entry));
        }
    } 

    DBUG_RETURN( arg_node);
}

node* TCVbool(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCVbool");

    INFO_SET_TYPE(arg_info, T_bool);

    DBUG_RETURN( arg_node);
}

node* TCVfloat(node *arg_node, info *arg_info)
{   
    DBUG_ENTER("TCVfloat");

    INFO_SET_TYPE(arg_info, T_float);

    DBUG_RETURN( arg_node);
}

node* TCVnum(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCVnum");

    INFO_SET_TYPE(arg_info, T_int);

    DBUG_RETURN( arg_node);
}

node* TCVbinop(node *arg_node, info *arg_info) 
{
    DBUG_ENTER("TCVbinop");

    TRAVdo(BINOP_LEFT(arg_node), arg_info);
    cctype t1 = INFO_GET_TYPE(arg_info);

    TRAVdo(BINOP_RIGHT(arg_node), arg_info);
    cctype t2 = INFO_GET_TYPE(arg_info);

    if (t1 != t2) {
        CTIerror(ERROR_TYPE_BINOP);
    }
    else {
        INFO_SET_TYPE(arg_info, t1);
    }

    DBUG_RETURN( arg_node);
}

// node *TCVfundef(node *arg_node, info *arg_info)
// {
//     DBUG_ENTER("TCVfundef");
//     DBUG_RETURN( arg_node);
// }

// node *TCVfunbody(node *arg_node, info *arg_info)
// {
//     DBUG_ENTER("TCVbody");
//     DBUG_RETURN( arg_node);
// }



node *TCVdoVariables( node *syntaxtree)
{
    info *arg_info;

    DBUG_ENTER("TCVdoVariables");

    arg_info = MakeInfo();

    TRAVpush(TR_tcv);
    syntaxtree = TRAVdo( syntaxtree, arg_info);
    TRAVpop();

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}