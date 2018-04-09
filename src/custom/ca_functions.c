#include "ca_functions.h"
#include "str.h"
#include "errors.h"

node* root_node;

struct INFO {
    node* table_stack[32];
    int index;

    int no_return;
};

/*
 * TABLE functions
 */
#define TABLES_INDEX(n)  ((n)->index)
#define TABLES_GET_TABLE(n, i) ((n)->table_stack[i])
#define TABLES_GLOBAL_TABLE(n) ((n)->table_stack[0])
#define TABLES_TABLE_STACK(n) ((n)->table_stack)
#define TABLES_CURRENT_TABLE(n) ((n)->table_stack[(n)->index])
#define TABLES_ADD_TABLE(n, symboltable) ((n)->table_stack[++(n)->index] = symboltable)
#define TABLES_REMOVE_TABLE(n) ((n)->table_stack[(n)->index--] = NULL)

#define TABLES_NO_RETURN(n) ((n)->no_return)

bool is_equal(node* funDef, node* fun) {
    if(NODE_TYPE(fun) == N_funcall) {
        return STReq(IDENT_NAME(FUNHEADER_IDENT(FUNDEF_FUNHEADER(funDef))), IDENT_NAME(FUNCALL_IDENT(fun)));
    } 
    if(NODE_TYPE(fun) == N_fundef) {
        return fun == funDef ;
    } 
    return 0;
}

/**
 * Look through a single table for function declarations.
 * This table can be a fundef of a program.
 * @param table
 * @param funCall
 * @param times
 * @return
 */
node* searchFunctionTable(node* fun, int* times) {
    node* declarations;
    node* foundNode = NULL;
    
    if(NODE_TYPE(root_node) == N_program) {
        declarations = PROGRAM_DECLARATIONS(root_node);
    } 

    while(declarations != NULL) {
        node* declaration = DECLARATIONS_DECLARATION(declarations);
        
        if (NODE_TYPE(declaration) == N_fundef &&
            is_equal(declaration, fun)) {
            foundNode = declaration;
            *times += 1; 
        }

        declarations = DECLARATIONS_NEXT(declarations);
    }

    return foundNode;    
}

/**
 * Look through the whole stack of tables.
 * This table can be a program or a table, so inline functions are possible
 * @param tables
 * @param funCall
 * @param totalTimes
 * @return
 */
// node* searchFunctionTables(info *tables, node* funCall, int *totalTimes) {
//     DBUG_ENTER("searchFunctionTables");
//     node* returnNode = NULL;
//     for(int i = TABLES_INDEX(tables); i >= 0; i--) {
//         int times = 0;
//         node *foundNode = searchFunctionTable(TABLES_GET_TABLE(tables, i), funCall, &times);
//         if (foundNode != NULL) {
//             returnNode = foundNode;
//             break;
//         }

//         if(totalTimes != NULL) {
//             *totalTimes = *totalTimes + times;
//         }
//     }

//     DBUG_RETURN(returnNode);
//     return returnNode;
// }

info* MakeTables(void)
{
    info *tables;

    DBUG_ENTER( "MakeInfo");

    tables = (info *)MEMmalloc(sizeof(info));

    TABLES_INDEX(tables) = -1;

    DBUG_RETURN( tables);
}

info* FreeTables( info *tables)
{
    DBUG_ENTER ("SymboltablesInfo");

    tables = MEMfree( tables);

    DBUG_RETURN( tables);
}


node *CAFprogram(node *arg_node, info *tables)
{
    DBUG_ENTER("CANprogram");

    if(PROGRAM_SYMBOLTABLE(arg_node) != NULL) {
        PROGRAM_SYMBOLTABLE(arg_node) = TRAVdo(PROGRAM_SYMBOLTABLE(arg_node), tables);
    }

    // Add program to the stack and continue traversal
    TABLES_ADD_TABLE(tables, arg_node);
    PROGRAM_DECLARATIONS(arg_node) = TRAVdo(PROGRAM_DECLARATIONS(arg_node), tables);
    TABLES_REMOVE_TABLE(tables);

    DBUG_RETURN( arg_node);
}

// node* CAFfundec(node *arg_node, info *tables) {
//     DBUG_ENTER("CAFfundef");

//     int times = 0;
//     searchFunctionTable(TABLES_CURRENT_TABLE(tables), arg_node, &times);

//     if(times > 1) {
//         CTIerror(ERROR_REDEC_FUNC, arg_node->lineno + 1, IDENT_NAME(FUNHEADER_IDENT(FUNDEF_FUNHEADER(arg_node))));
//     }

//     FUNDEC_FUNHEADER(arg_node) = TRAVdo(FUNDEC_FUNHEADER(arg_node), tables);


//     DBUG_RETURN( arg_node);
// }

node *CAFfundef(node *arg_node, info *tables)
{
    DBUG_ENTER("CAFfundef");

    // Search on the same level for duplicates.
    // Overloading is possible, but on the same level gives a duplicate conflict
    
    int times = 0;
    searchFunctionTable(arg_node, &times);

    if(times > 1) {
        CTIerror(ERROR_REDEC_FUNC, arg_node->lineno + 1, IDENT_NAME(FUNHEADER_IDENT(FUNDEF_FUNHEADER(arg_node))));
    }
    
    FUNDEF_SCOPE(arg_node) = TABLES_INDEX(tables);

    // Add to table and traverse through the function body.
    TABLES_ADD_TABLE(tables, arg_node);

    if(FUNDEF_SYMBOLTABLE(arg_node) != NULL) {
        FUNDEF_SYMBOLTABLE(arg_node) = TRAVdo(FUNDEF_SYMBOLTABLE(arg_node), tables);
    }
    
    FUNDEF_FUNHEADER(arg_node) = TRAVdo(FUNDEF_FUNHEADER(arg_node), tables);

    TABLES_NO_RETURN(tables) = 1;
    if(FUNDEF_FUNBODY(arg_node) != NULL) {
        FUNDEF_FUNBODY(arg_node) = TRAVdo(FUNDEF_FUNBODY(arg_node), tables);
    }
    
    if (TABLES_NO_RETURN(tables) == 1){
        
        if (FUNHEADER_RETTYPE(FUNDEF_FUNHEADER(arg_node)) == T_void &&
            FUNDEF_FUNBODY(arg_node)) {
            
            node* retstmt = TBmakeReturnstmt(NULL);
            
            node* stmts = FUNBODY_STMTS(FUNDEF_FUNBODY(arg_node));
            
            if (stmts == NULL) {
                stmts = TBmakeStmts(retstmt, NULL);
                FUNBODY_STMTS(FUNDEF_FUNBODY(arg_node)) = stmts;
            }
            else {
                while (STMTS_NEXT(stmts)) {
                    stmts = STMTS_NEXT(stmts); 
                }
                
                STMTS_NEXT(stmts) = TBmakeStmts(retstmt, NULL);
            }
        }

        if (FUNHEADER_RETTYPE(FUNDEF_FUNHEADER(arg_node)) != T_void &&
        FUNDEF_FUNBODY(arg_node) != NULL) {
            CTIerror("No return statement found for non void function.");
        }
    }

    

    TABLES_REMOVE_TABLE(tables);


    DBUG_RETURN( arg_node);
}

node *CAFreturnstmt(node *arg_node, info *tables)
{
    DBUG_ENTER("CAFreturnstmt");

    TABLES_NO_RETURN(tables) = 0;

    if(RETURNSTMT_EXPR(arg_node) != NULL) {
        TRAVdo(RETURNSTMT_EXPR(arg_node), tables);
    }

    DBUG_RETURN( arg_node);
}

node* CAFblock(node *arg_node, info *tables) {
    DBUG_ENTER("CAFreturnstmt");

    TABLES_NO_RETURN(tables) = 1;
    BLOCK_STMTS(arg_node) = TRAVdo(BLOCK_STMTS(arg_node), tables);

    DBUG_RETURN( arg_node);
}

node* CAFifelsestmt(node *arg_node, info *tables) {
    DBUG_ENTER("CAFifelsestmt");

    IFELSESTMT_IFBLOCK(arg_node) = TRAVdo(IFELSESTMT_IFBLOCK(arg_node), tables);

    if (IFELSESTMT_ELSEBLOCK(arg_node)) {
            IFELSESTMT_ELSEBLOCK(arg_node) = TRAVdo(IFELSESTMT_ELSEBLOCK(arg_node), tables);
    }

    // if (TABLES_NO_RETURN(tables)) {
        
        
            
        
    // }
    
    DBUG_RETURN( arg_node);
}

node *CAFfuncall(node *arg_node, info *tables)
{
    DBUG_ENTER("CAFfuncall");

    // Check if you can find the function in one of the tables.
    // Otherwise give a error.
    int times = 0;
    node* entry = searchFunctionTable(arg_node, &times);
    
    if(entry != NULL) {
        FUNCALL_SYMBOLTABLEENTRY(arg_node) = entry;
    } else {
        CTIerror(ERROR_UNDEC_FUNC, arg_node->lineno + 1, IDENT_NAME(FUNCALL_IDENT(arg_node)));
    }

    if(FUNCALL_ARGS(arg_node) != NULL) {
        FUNCALL_ARGS(arg_node) = TRAVdo(FUNCALL_ARGS(arg_node), tables);
    }

    DBUG_RETURN( arg_node);
}

node *CAFdoFunctions( node *syntaxtree)
{
    info *tables;

    DBUG_ENTER("CAVdoFunctions");

    tables = MakeTables();
    root_node = syntaxtree;
    TRAVpush(TR_caf);
    syntaxtree = TRAVdo( syntaxtree, tables);
    TRAVpop();

    tables = FreeTables(tables);

    DBUG_RETURN( syntaxtree);
}