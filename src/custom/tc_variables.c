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
#include "errors.h"
#include "string.h"
#include "util.h"

struct INFO {
  cctype current_type;
};

#define INFO_SET_TYPE(n, type)  ((n)->current_type = type)
#define INFO_GET_TYPE(n)  ((n)->current_type)

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

cctype tc_type_inference(node* expr, info *arg_info)
{
    TRAVdo(expr, arg_info);
    return INFO_GET_TYPE(arg_info);
}

node* TCVcastexpr(node* arg_node, info *arg_info)
{
    DBUG_ENTER("TCVcastexpr");

    node* expr = CASTEXPR_EXPR(arg_node);
    cctype type = CASTEXPR_TYPE(arg_node);
    cctype expr_type = tc_type_inference(expr, arg_info);
    CASTEXPR_EXPRTYPE(arg_node) = expr_type;

    if (type == T_bool && expr_type == T_int) {
        node* binop = TBmakeBinop(BO_ge, expr, TBmakeNum(1), NULL);
        BINOP_TYPE(binop) = T_int;
        node* condexpr = TBmakeCondexpr(TBmakeBool(TRUE), TBmakeBool(FALSE),
                                        binop);
        CASTEXPR_CONDEXPR(arg_node) = condexpr;
    }
    else if (type == T_int && expr_type == T_bool) {
        node* binop = TBmakeBinop(BO_eq, expr, TBmakeBool(TRUE), NULL);
        BINOP_TYPE(binop) = T_bool;
        node* condexpr = TBmakeCondexpr(TBmakeNum(1), TBmakeNum(0),
                                        binop);
        CASTEXPR_CONDEXPR(arg_node) = condexpr;
    }
    else if (type == T_float && expr_type == T_bool) {
        node* binop = TBmakeBinop(BO_eq, expr, TBmakeBool(TRUE), NULL);
        BINOP_TYPE(binop) = T_bool;
        node* condexpr = TBmakeCondexpr(TBmakeFloat(1.0), TBmakeFloat(0.0),
                                        binop);
        CASTEXPR_CONDEXPR(arg_node) = condexpr;
    }
    else if (type == T_bool && expr_type == T_float) {
        node* binop = TBmakeBinop(BO_ge, expr, TBmakeFloat(1.0), NULL);
        BINOP_TYPE(binop) = T_float;
        node* condexpr = TBmakeCondexpr(TBmakeBool(TRUE), TBmakeBool(FALSE),
                                        binop);
        CASTEXPR_CONDEXPR(arg_node) = condexpr;
    }

    INFO_SET_TYPE(arg_info, type);

    DBUG_RETURN(arg_node);
}


node *TCVglobaldef (node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCVglobaldef");

    node* entry = GLOBALDEF_SYMBOLTABLEENTRY(arg_node);
    cctype declared_type = SYMBOLTABLEENTRY_TYPE(entry);
    
    node* exprs = GLOBALDEF_EXPRS(arg_node);

    if (exprs != NULL ) {
        node* expr = EXPRS_EXPR(exprs);
        cctype inferred_type = tc_type_inference(expr, arg_info);

        if (declared_type != inferred_type){
            CTIerror(ERROR_TYPE_GLOBDEF, arg_node->lineno + 1,
                                         SYMBOLTABLEENTRY_NAME(entry),
                                         cctypeToString(declared_type),
                                         cctypeToString(inferred_type));
        }
    } 

    DBUG_RETURN( arg_node);
}

node *TCVvardec(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCVvardec");
    
    node* entry = VARDEC_SYMBOLTABLEENTRY(arg_node);
    cctype declared_type = SYMBOLTABLEENTRY_TYPE(entry);

    node* exprs = VARDEC_EXPRS(arg_node);

    if (exprs != NULL ) {
        node* expr = EXPRS_EXPR(exprs);
        cctype inferred_type = tc_type_inference(expr, arg_info);

        if (declared_type != inferred_type){
            CTIerror(ERROR_TYPE_GLOBDEF, arg_node->lineno + 1,
                                         SYMBOLTABLEENTRY_NAME(entry),
                                         cctypeToString(declared_type),
                                         cctypeToString(inferred_type));
        }
    } 

    DBUG_RETURN( arg_node);
}

node *TCVassign(node *arg_node, info *arg_info)
{
    DBUG_ENTER("TCVassign");
    
    node* entry = ASSIGN_SYMBOLTABLEENTRY(arg_node);
    cctype declared_type = SYMBOLTABLEENTRY_TYPE(entry);
    
    node* expr = ASSIGN_EXPR(arg_node);

    if (expr != NULL ) {
        cctype inferred_type = tc_type_inference(expr, arg_info);

        if (declared_type != inferred_type){
            CTIerror(ERROR_TYPE_GLOBDEF, arg_node->lineno + 1,
                                         SYMBOLTABLEENTRY_NAME(entry),
                                         cctypeToString(declared_type),
                                         cctypeToString(inferred_type));
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

node *TCVvarcall(node *arg_node, info *arg_info) {
    DBUG_ENTER("TCVvarcall");

    node* entry = VARCALL_SYMBOLTABLEENTRY(arg_node);
    cctype type = SYMBOLTABLEENTRY_TYPE(entry);

    INFO_SET_TYPE(arg_info, type);

    DBUG_RETURN( arg_node);
}

node *TCVfuncall(node *arg_node, info *arg_info) {
    DBUG_ENTER("TCVfuncall");

    node* fundef = FUNCALL_SYMBOLTABLEENTRY(arg_node);
    node* funheader = FUNDEF_FUNHEADER(fundef);
    cctype type = FUNHEADER_RETTYPE(funheader);

    INFO_SET_TYPE(arg_info, type);

    DBUG_RETURN( arg_node);
}

node* TCVmonop(node *arg_node, info *arg_info) {
    DBUG_ENTER("TCVmonop");

    MONOP_RIGHT(arg_node) = TRAVdo(MONOP_RIGHT(arg_node), arg_info);
    cctype type = INFO_GET_TYPE(arg_info);
    monop op = MONOP_OP(arg_node);
    MONOP_TYPE(arg_node) = type;
    
    if ((op == MO_neg && type == T_bool) ||
        (op == MO_not && (type == T_int || type == T_float))) {
            CTIerror(ERROR_TYPE_MONOP, arg_node->lineno + 1);
    }
    

    DBUG_RETURN( arg_node);
} 

node* TCVbinop(node *arg_node, info *arg_info) 
{
    DBUG_ENTER("TCVbinop");

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