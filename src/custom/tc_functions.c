/*****************************************************************************
 *
 * Module: tc_functions
 *
 * Prefix: TCF
 *
 * Description:
 *
 * This module implements type checking of functions
 *
 *****************************************************************************/

#include "tc_functions.h"
#include "errors.h"
#include "string.h"
#include "util.h"

struct INFO {
    cctype ret_type [32];
    int ret_type_ptr;

    cctype current_type;
};

#define INFO_SET_TYPE(n, type)  ((n)->current_type = type)
#define INFO_GET_TYPE(n)  ((n)->current_type)

#define INFO_SET_RETTYPE(n, type)  ((n)->ret_type[(n)->ret_type_ptr] = type)
#define INFO_GET_RETTYPE(n)  ((n)->ret_type[(n)->ret_type_ptr])
#define INFO_INC_RETPTR(n) ((n)->ret_type_ptr++)
#define INFO_DEC_RETPTR(n) ((n)->ret_type_ptr--)

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


cctype tf_type_inference(node* expr, info *arg_info)
{
    TRAVdo(expr, arg_info);
    return INFO_GET_TYPE(arg_info);
}

node* TCFbool(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCFbool");

    INFO_SET_TYPE(arg_info, T_bool);

    DBUG_RETURN( arg_node);
}

node* TCFfloat(node *arg_node, info *arg_info)
{   
    DBUG_ENTER("TCFfloat");

    INFO_SET_TYPE(arg_info, T_float);

    DBUG_RETURN( arg_node);
}

node* TCFnum(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCFnum");

    INFO_SET_TYPE(arg_info, T_int);

    DBUG_RETURN( arg_node);
}

node *TCFvarcall(node *arg_node, info *arg_info) {
    DBUG_ENTER("TCFvarcall");

    node* entry = VARCALL_SYMBOLTABLEENTRY(arg_node);
    cctype type = SYMBOLTABLEENTRY_TYPE(entry);

    INFO_SET_TYPE(arg_info, type);

    DBUG_RETURN( arg_node);
}

node *TCFfuncall(node *arg_node, info *arg_info) {
    DBUG_ENTER("TCFfuncall");

    node* fundef = FUNCALL_SYMBOLTABLEENTRY(arg_node);
    node* funheader = FUNDEF_FUNHEADER(fundef);
    cctype type = FUNHEADER_RETTYPE(funheader);

    node* params = FUNHEADER_PARAMS(funheader);
    
    if (params == NULL) {
        INFO_SET_TYPE(arg_info, type);
        DBUG_RETURN( arg_node);
    }
    
    
    node* args = FUNCALL_ARGS(arg_node);
    
    while (params && args) {
        node* arg = EXPRS_EXPR(args);
        node* param = PARAMS_PARAM(params);

        cctype param_type = PARAM_TYPE(param);
        cctype arg_type = tf_type_inference(arg, arg_info);

        if (param_type != arg_type) {
            CTIerror(ERROR_PAREM_TYPE, arg_node->lineno + 1, cctypeToString(arg_type),
                                                             cctypeToString(param_type));
        }

        args = EXPRS_NEXT(args);
        params = PARAMS_NEXT(params);
        
    }

    INFO_SET_TYPE(arg_info, type);

    DBUG_RETURN( arg_node);
}

node* TCFbinop(node *arg_node, info *arg_info) 
{
    DBUG_ENTER("TCFbinop");

    BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);
    cctype t1 = INFO_GET_TYPE(arg_info);

    BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);
    cctype t2 = INFO_GET_TYPE(arg_info);

    binop op = BINOP_OP(arg_node);
    if (t1 != t2) {
        CTIerror(ERROR_TYPE_BINOP, arg_node->lineno + 1, cctypeToString(t1),
                                                         cctypeToString(t2));
        INFO_SET_TYPE(arg_info, T_unknown);
        DBUG_RETURN( arg_node);

    }

    if (t1 == T_int) {
        if (op == BO_lt || op == BO_le || op == BO_gt || op == BO_ge || op == BO_ne || op == BO_eq ){
            INFO_SET_TYPE(arg_info, T_bool);
            BINOP_TYPE(arg_node) = T_int;       
        }
        else if(op == BO_add || op == BO_sub || op == BO_mul || op == BO_div || op == BO_mod) {
            INFO_SET_TYPE(arg_info, T_int);
            BINOP_TYPE(arg_node) = T_int;       
        }
        else {
             CTIerror(ERROR_OP_NOT_SUP, arg_node->lineno + 1, "int");
        }
    }

    if (t1 == T_float) {
        if (op == BO_lt || op == BO_le || op == BO_gt || op == BO_ge || op == BO_ne || op == BO_eq ){
            INFO_SET_TYPE(arg_info, T_bool);
            BINOP_TYPE(arg_node) = T_float;       
        }
        else if (op == BO_add || op == BO_sub || op == BO_mul || op == BO_div ) {
            INFO_SET_TYPE(arg_info, T_float);
            BINOP_TYPE(arg_node) = T_float;       
        }
        else {
            CTIerror(ERROR_OP_NOT_SUP, arg_node->lineno + 1, "float");
        }
    }

    if (t1 == T_bool) {
        if (op == BO_and || op == BO_or || op == BO_ne || op == BO_eq ||
            op == BO_add || op == BO_mul) {
            INFO_SET_TYPE(arg_info, T_bool);  
            BINOP_TYPE(arg_node) = T_bool;             
        }
        else {
            CTIerror(ERROR_OP_NOT_SUP, arg_node->lineno + 1, "bool");
        }
    }

    DBUG_RETURN( arg_node);
}

node *TCFcastexpr(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCFcastexpr");

    INFO_SET_TYPE(arg_info, CASTEXPR_TYPE(arg_node));

    DBUG_RETURN( arg_node);
}


node* TCFfundef(node *arg_node, info *arg_info) {

    DBUG_ENTER("TCFfundef");

    node* funheader = FUNDEF_FUNHEADER(arg_node);
    cctype type = FUNHEADER_RETTYPE(funheader);

    INFO_INC_RETPTR(arg_info);
    INFO_SET_RETTYPE(arg_info, type);

    if (FUNDEF_FUNBODY(arg_node)) {
        FUNDEF_FUNBODY(arg_node) = TRAVdo(FUNDEF_FUNBODY(arg_node), arg_info);
    }

    INFO_DEC_RETPTR(arg_info);

    DBUG_RETURN( arg_node);
}

node *TCFreturnstmt(node *arg_node, info *arg_info) {
    DBUG_ENTER("TCFreturnstmt");

    node* expr = RETURNSTMT_EXPR(arg_node);
    
    cctype ret_type = T_void;
    if (expr != NULL) {
        ret_type = tf_type_inference(expr, arg_info);
    }

    cctype fun_type = INFO_GET_RETTYPE(arg_info);
    
    if (ret_type != fun_type) {
        CTIerror(ERROR_RET_TYPE, arg_node->lineno + 1, cctypeToString(ret_type),
                                                       cctypeToString(fun_type));
    }

    DBUG_RETURN( arg_node);
} 

node *TCFdoFunctions( node *syntaxtree)
{
    info *arg_info;

    DBUG_ENTER("TCFdoFunctions");

    arg_info = MakeInfo();

    TRAVpush(TR_tcf);
    syntaxtree = TRAVdo( syntaxtree, arg_info);
    TRAVpop();

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}