#include "ca_functions.h"

struct INFO {
    node* table_stack[32];
    int index;
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

info* MakeTables(void)
{
    info *tables;

    DBUG_ENTER( "MakeInfo");

    tables = (info *)MEMmalloc(sizeof(info));

    TABLES_INDEX(tables) = 0;

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

    printf("PROGRAM Stack: %i\n", TABLES_INDEX(tables));

    TABLES_ADD_TABLE(tables, arg_node);
    PROGRAM_DECLARATIONS(arg_node) = TRAVdo(PROGRAM_DECLARATIONS(arg_node), tables);
    TABLES_REMOVE_TABLE(tables);

    DBUG_RETURN( arg_node);
}

node *CAFfundef(node *arg_node, info *tables)
{
    DBUG_ENTER("CAFfundef");

    printf("FUNCTION DEF: -------- \n");
    TABLES_ADD_TABLE(tables, arg_node);
    printf("New Stack: %i\n", TABLES_INDEX(tables));

    FUNDEF_FUNHEADER(arg_node) = TRAVdo(FUNDEF_FUNHEADER(arg_node), tables);
    if(FUNDEF_FUNBODY(arg_node) != NULL) {
        FUNDEF_FUNBODY(arg_node) = TRAVdo(FUNDEF_FUNBODY(arg_node), tables);
    }
    TABLES_REMOVE_TABLE(tables);

    DBUG_RETURN( arg_node);
}

node *CAFfuncall(node *arg_node, info *tables)
{
    DBUG_ENTER("CAVfuncall");

    printf("FUNCTION CALL: \n");
    printf("Current Stack: %i\n", TABLES_INDEX(tables));

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

    TRAVpush(TR_caf);
    syntaxtree = TRAVdo( syntaxtree, tables);
    TRAVpop();

    tables = FreeTables(tables);

    DBUG_RETURN( syntaxtree);
}