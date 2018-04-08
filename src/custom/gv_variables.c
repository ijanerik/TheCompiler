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
#include "str.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"


/*
 * INFO structure
 */

struct INFO {
    node* initStatements;
    node* latestStatement;

    node* declarations;
    node* latestDec;

    node* inits;
    node* latestInit;
};


/*
 * INFO macros
 */

#define INFO_STATEMENTS(n)  ((n)->initStatements)
#define INFO_LATEST(n) ((n)->latestStatement)
#define INFO_LATEST_DEC(n) ((n)->latestDec)
#define INFO_DECLARATIONS(n) ((n)->declarations)
#define INFO_INITS(n) ((n)->inits)
#define INFO_LATEST_INIT(n) ((n)->latestInit)


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

    INFO_DECLARATIONS(result) = NULL;
    INFO_LATEST_DEC( result) = NULL;

    INFO_INITS(result) = NULL;
    INFO_LATEST_INIT(result) = NULL;

    DBUG_RETURN( result);
}

static info *FreeInfo( info *info)
{
    DBUG_ENTER ("FreeInfo");

    info = MEMfree( info);

    DBUG_RETURN( info);
}


void updateNext(node* lastDec, node* next) {
    if(NODE_TYPE(lastDec) == N_program) {
        PROGRAM_DECLARATIONS(lastDec) = next;
    } else {
        DECLARATIONS_NEXT(lastDec) = next;
    }
}

/*
 * Traversal functions
 * Cleaning the initiation
 */
node *GVVdeclarations (node *arg_node, info *arg_info)
{
    DBUG_ENTER("GVVdeclarations");
    node* next = DECLARATIONS_NEXT(arg_node);
    DECLARATIONS_NEXT(arg_node) = NULL;

    node* declaration = DECLARATIONS_DECLARATION(arg_node);

    if(NODE_TYPE(declaration) == N_globaldef || NODE_TYPE(declaration) == N_globaldec) {
        updateNext(INFO_LATEST_DEC(arg_info), NULL);

        if(NODE_TYPE(declaration) == N_globaldef) {
            // @TODO only works for single expressions, so no array functionality
            node* ident = TBmakeIdent(STRcpy(IDENT_NAME(GLOBALDEF_IDENT(declaration))));
            node* exprs = GLOBALDEF_EXPRS(declaration);
            if(exprs != NULL) {
                GLOBALDEF_EXPRS(declaration) = NULL;
                node *assignVar = TBmakeAssign(ident, exprs, NULL);
                node *stmts = TBmakeStmts(assignVar, NULL);
                if (INFO_STATEMENTS(arg_info) == NULL) {
                    INFO_STATEMENTS(arg_info) = stmts;
                } else {
                    STMTS_NEXT(INFO_LATEST(arg_info)) = stmts;
                }

                INFO_LATEST(arg_info) = stmts;
            }
        }

        if(INFO_INITS(arg_info) == NULL) {
            INFO_INITS(arg_info) = arg_node;
        } else {
            DECLARATIONS_NEXT(INFO_LATEST_INIT(arg_info)) = arg_node;
        }
        INFO_LATEST_INIT(arg_info) = arg_node;

    } else {
        if(INFO_DECLARATIONS(arg_info) == NULL) {
            INFO_DECLARATIONS(arg_info) = arg_node;
        } else {
            DECLARATIONS_NEXT(INFO_LATEST_DEC(arg_info)) = arg_node;
        }
        INFO_LATEST_DEC(arg_info) = arg_node;
    }

    if(next != NULL) {
        TRAVdo(next, arg_info);
    }

    DBUG_RETURN( arg_node);
}

node* makeInitFun(node* stmts) {
    return TBmakeFundef(0, TBmakeFunheader(T_void, TBmakeIdent(STRcpy("__init")), NULL), TBmakeFunbody(NULL, NULL, stmts), NULL);
}

node *GVVprogram (node *arg_node, info *arg_info)
{
    DBUG_ENTER("GVVprogram");

    INFO_LATEST_DEC(arg_info) = arg_node;

    node* next = PROGRAM_DECLARATIONS(arg_node);
    PROGRAM_DECLARATIONS(arg_node) = NULL;

    if(next != NULL) {
        TRAVdo(next, arg_info);

        node* initFunc = makeInitFun(INFO_STATEMENTS(arg_info));
        node* declarations = TBmakeDeclarations(initFunc, INFO_DECLARATIONS(arg_info));
        if(INFO_LATEST_INIT(arg_info) != NULL) {
            DECLARATIONS_NEXT(INFO_LATEST_INIT(arg_info)) = declarations;
            PROGRAM_DECLARATIONS(arg_node) = INFO_INITS(arg_info);
        } else {
            PROGRAM_DECLARATIONS(arg_node) = declarations;
        }

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
