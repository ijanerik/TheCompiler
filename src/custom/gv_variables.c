/*****************************************************************************
 *
 * Module: gv_variables
 *
 * Prefix: GVV
 *
 * Description:
 *
 * Update variable initialization for global variables
 *
 *****************************************************************************/


#include "gv_variables.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "copy_node.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"


/*
 * INFO structure
 */

struct INFO {
    node* initStatements;
    node* latest;
    node* latestDec;
};


/*
 * INFO macros
 */

#define INFO_STATEMENTS(n)  ((n)->initStatements)
#define INFO_LATEST(n) ((n)->latest)
#define INFO_LATEST_DEC(n) ((n)->latestDec)


/*
 * INFO functions
 */

static info *MakeInfo(void)
{
    info *result;

    DBUG_ENTER( "MakeInfo");

    result = (info *)MEMmalloc(sizeof(info));

    INFO_STATEMENTS( result) = NULL;
    INFO_LATEST( result) = NULL;
    INFO_LATEST_DEC( result) = NULL;

    DBUG_RETURN( result);
}

static info *FreeInfo( info *info)
{
    DBUG_ENTER ("FreeInfo");

    info = MEMfree( info);

    DBUG_RETURN( info);
}


/*
 * Traversal functions
 * Cleaning the initiation
 */
node *GVVdeclarations (node *arg_node, info *arg_info)
{
    DBUG_ENTER("GVVdeclarations");

    if(NODE_TYPE(INFO_LATEST_DEC(arg_info)) == N_program) {
        PROGRAM_DECLARATIONS(INFO_LATEST_DEC(arg_info)) = arg_node;
    } else {
        DECLARATIONS_NEXT(INFO_LATEST_DEC(arg_info)) = arg_node;
    }

    node* declaration = DECLARATIONS_DECLARATION(arg_node);

    if(NODE_TYPE(declaration) == N_globaldef || NODE_TYPE(declaration) == N_globaldec) {
        if(NODE_TYPE(declaration) == N_globaldef) {
            // @TODO only works for single expressions, so no array functionality
            node* ident = COPYident(GLOBALDEF_IDENT(declaration), NULL);
            node* exprs = COPYexprs(GLOBALDEF_EXPRS(declaration), NULL);
            node* assignVar = TBmakeAssign(ident, exprs, NULL);
            node* stmts = TBmakeStmts(assignVar, NULL);
            if(INFO_STATEMENTS(arg_info) == NULL) {
                INFO_STATEMENTS(arg_info) = stmts;
            } else {
                STMTS_NEXT(INFO_LATEST(arg_info)) = stmts;
            }

            INFO_LATEST(arg_info) = stmts;
        }
    } else {
        INFO_LATEST_DEC(arg_info) = arg_node;
    }

    if(DECLARATIONS_NEXT(arg_node) != NULL) {
        DECLARATIONS_NEXT(arg_node) = TRAVdo(DECLARATIONS_NEXT(arg_node), arg_info);
    } else {
        if(NODE_TYPE(INFO_LATEST_DEC(arg_info)) == N_program) {
            PROGRAM_DECLARATIONS(INFO_LATEST_DEC(arg_info)) = NULL;
        } else {
            DECLARATIONS_NEXT(INFO_LATEST_DEC(arg_info)) = NULL;
        }
    }

    DBUG_RETURN( arg_node);
}

node* makeInitFun(node* stmts) {
    return TBmakeFundef(0, TBmakeFunheader(T_void, TBmakeIdent("__init"), NULL), TBmakeFunbody(NULL, NULL, stmts), NULL);
}

node *GVVprogram (node *arg_node, info *arg_info)
{
    DBUG_ENTER("GVVprogram");

    INFO_LATEST_DEC(arg_info) = arg_node;

    if(PROGRAM_DECLARATIONS(arg_node) != NULL) {
        PROGRAM_DECLARATIONS(arg_node) = TRAVdo(PROGRAM_DECLARATIONS(arg_node), arg_info);

        node* initFunc = makeInitFun(INFO_STATEMENTS(arg_info));
        node* declarations = TBmakeDeclarations(initFunc, PROGRAM_DECLARATIONS(arg_node));
        PROGRAM_DECLARATIONS(arg_node) = declarations;
    }

    DBUG_RETURN( arg_node);
}


/*
 * Traversal start function
 */

node *GVdoVariableToFunction( node *syntaxtree)
{
    info *arg_info;

    DBUG_ENTER("GVdoVariableToFunction");

    arg_info = MakeInfo();

    TRAVpush( TR_gvv);
    syntaxtree = TRAVdo( syntaxtree, arg_info);
    TRAVpop();

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}
