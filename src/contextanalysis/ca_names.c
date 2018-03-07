/*****************************************************************************
 *
 * Module: ca_names
 *
 * Prefix: CAN
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

#define ERROR_RE_DEC_VAR "Variable is already declared"
#define ERROR_UN_DEC_VAR "Variable is used but not yet declared"

/*
 * INFO macros
 */

// #define INFO_STACK(n)  ((n)->table_stack)
#define INFO_INDEX(n)  ((n)->index)
#define INFO_GET_TABLE(n, i) ((n)->table_stack[i])
#define INFO_GLOBAL_TABLE(n) ((n)->table_stack)
#define INFO_CURRENT_TABLE(n) ((n)->table_stack[(n)->index])


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

void throwError(char *msg) {
    printf("ERROR: %s\n", msg);
}

// @todo Not sure about cctype here
bool addSymbolTableEntry(node* symbol_table, char* name, cctype type) {
    if (NODE_TYPE (symbol_table) != N_symboltable) {
        // @todo terminate 
        return false;
    }
    while (SYMBOLTABLE_NEXT(symbol_table) != NULL) {
        symbol_table = SYMBOLTABLE_NEXT(symbol_table);
    }
    // @todo Not sure if this works, node_basic.c as guideline

    // Allocate a new symboltable node
    node* table = MakeEmptyNode ();
    NODE_TYPE (table) = N_symboltable;
    table->sons.N_symboltable = MEMmalloc (sizeof (struct SONS_N_SYMBOLTABLE));
    
    // Allocate a new symboltableentry node
    node* entry = MakeEmptyNode ();
    NODE_TYPE (entry) = N_symboltableentry;
    table->sons.N_symboltableentry = MEMmalloc (sizeof (struct SONS_N_SYMBOLTABLEENTRY));
    
    // Add a new entry to the symbol table
    SYMBOLTABLE_NEXT(symbol_table) = table;
    SYMBOLTABLE_SYMBOLTABLEENTRY(table) = entry;
    SYMBOLTABLEENTRY_NAME(entry) = name;
    SYMBOLTABLEENTRY_TYPE(entry) = type;

    return true;
}

bool searchSymbolTable(node* symbol_table, char* name, node* symbol_entry) {
    /* @todo Work your magic jan erik */
    if (symbol_entry != NULL) {
        // symbol_entry = ... ;
    }
}


// @todo info get table boundary checking
bool searchSymbolTables(info* arg_info, char* name, node* symbol_entry) {
    for (int i = 0; i <= INFO_INDEX(arg_info); i++) {
        if (searchSymbolTable(INFO_GET_TABLE(arg_info, i), name, symbol_entry)){
            return true;
        }
    }
    return false;
}

/*
 * Traversal functions
 */

node *CANvardec(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CANvardec");
    node* ident = VARDEC_IDENT(arg_node);
    
    if (!searchSymbolTables(arg_info, IDENT_NAME(ident), NULL)) {
        addSymbolTableEntry(INFO_CURRENT_TABLE(arg_info),
                            VARDEC_IDENT(arg_node), VARDEC_TYPE(arg_node));
    }
    else {
        throwError(ERROR_RE_DEC_VAR);
    }
    
    DBUG_RETURN( arg_node);
}

node *CANglobaldec(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CANglobaldec");
    node* ident = GLOBALDEC_IDENT(arg_node);
    node* entry;

    if (!searchSymbolTable(INFO_GLOBAL_TABLE(arg_info), IDENT_NAME(ident), entry)) {
        addSymbolTableEntry(INFO_GLOBAL_TABLE(arg_info),
                            IDENT_NAME(ident), GLOBALDEC_TYPE(arg_node));
    }
    else {
        throwError(ERROR_RE_DEC_VAR);
    }
    
    DBUG_RETURN( arg_node);
}

node *CANglobaldef(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CANglobaldef");
    node* ident = GLOBALDEF_IDENT(arg_node);
    node* entry;

    if (searchSymbolTable(INFO_GLOBAL_TABLE(arg_info), IDENT_NAME(ident), entry)) {
        addSymbolTableEntry(INFO_GLOBAL_TABLE(arg_info),
                            IDENT_NAME(ident), GLOBALDEF_TYPE(arg_node));
    }
    else {
        // @todo not sure if this is the correct error
        throwError(ERROR_RE_DEC_VAR);
    }
    
    DBUG_RETURN( arg_node);
}

node *CANassign(node *arg_node, info *arg_info) {
    DBUG_ENTER("CANassign");
    node* ident = ASSIGN_LET(arg_node);
    node* entry;

    if (searchSymbolTables(arg_info, IDENT_NAME(ident), entry) {
        IDENT_DECL(ident) = entry;
    } else {
        throwErro(ERROR_UN_DEC_VAR);
    }

    DBUG_RETURN( arg_node);
}

node *CANexpr(node *arg_node, info *arg_info) {
    DBUG_ENTER("CANexpr");
    // Select ident from expression nodeset
    node* ident = EXPR_IDENT(arg_node);
    // Check if the expression is a ident
    if (ident == NULL) {
        DBUG_RETURN( arg_node);
    }

    node* entry;
    if (searchSymbolTables(arg_info, IDENT_NAME(ident), entry) {
        IDENT_DECL(ident) = entry;
    } else {
        throwErro(ERROR_UN_DEC_VAR);
    }

    DBUG_RETURN( arg_node);
}

// // @todo add VarCall node to AST
// node *CANvarcall(node *arg_node, info *arg_info) {
//     DBUG_ENTER("CANvarcall");
//     node* ident = VARCALL_IDENT(arg_node);
//     node* entry;

//     if (searchSymbolTables(arg_info, IDENT_NAME(ident), entry) {
//         IDENT_DECL(ident) = entry;
//     } else {
//         throwErro(ERROR_UN_DEC_VAR);
//     }

//     DBUG_RETURN( arg_node);
// }

/*
 * Traversal start function
 */

node *CANdoNames( node *syntaxtree)
{
    info *arg_info;

    DBUG_ENTER("CANdoNames");

    arg_info = MakeInfo();

    TRAVpush(TR_can);
    syntaxtree = TRAVdo( syntaxtree, arg_info);
    TRAVpop();

    CTInote( "Sum of integer constants: %d", INFO_SUM( arg_info));

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}
