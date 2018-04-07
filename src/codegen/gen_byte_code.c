#include "gen_byte_code.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"

#include "isa.h"
#include "constants_table.h"


struct INFO {
    node* constants_table;
    int scope;
};

#define INFO_SCOPE(n)  ((n)->scope)
#define INFO_CONSTANTS_TABLE(n) ((n)->constants_table)

static info *MakeInfo(void)
{
    info *result;

    DBUG_ENTER( "MakeInfo");

    result = (info *)MEMmalloc(sizeof(info));

    result->scope = 0;

    DBUG_RETURN( result);
}

static info *FreeInfo( info *info)
{
    DBUG_ENTER ("FreeInfo");

    info = MEMfree( info);

    DBUG_RETURN( info);
}

/*
 * Traversal start function
 */

void printOp0(char* instruction) {
    printf("%s\n", instruction);
}

void printOp1(char* instruction, int arg) {
    printf("%s %d\n", instruction, arg);
}


node* GBCassign(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCassign");

    ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

    node* entry = ASSIGN_SYMBOLTABLEENTRY(arg_node);
    int index = SYMBOLTABLEENTRY_INDEX(entry);

    printOp1(ISTORE, index);

    DBUG_RETURN(arg_node);
}

node* GBCnum(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCnum");

    int value = NUM_VALUE(arg_node);
    node* table = findConstant(T_int, (void*)&value,
                               INFO_CONSTANTS_TABLE(arg_info));
    if (table) {
        int index = CONSTANTSTABLE_INDEX(table);
        printOp1(ILOADC, index);
    }

    DBUG_RETURN(arg_node);
}

node* GBCvarcall(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCvarcall");

    node* entry = VARCALL_SYMBOLTABLEENTRY(arg_node);
    int index = SYMBOLTABLEENTRY_INDEX(entry);

    printOp1(ILOAD, index);

    DBUG_RETURN(arg_node);
}

node* GBCbinop(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCbinop");

    BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);
    BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);

    if (BINOP_OP(arg_node) == BO_add) {
        printOp0(IADD);
    }

    if (BINOP_OP(arg_node) == BO_mul) {
        printOp0(IMUL);
    }

    DBUG_RETURN(arg_node);
}



node *GBCdoGenByteCode( node *syntaxtree)
{
    DBUG_ENTER("GBCdoGenByteCode");

    info *arg_info;

    arg_info = MakeInfo();

    TRAVpush( TR_gbc);
    INFO_CONSTANTS_TABLE(arg_info) = PROGRAM_CONSTANTSTABLE(syntaxtree);
    syntaxtree = TRAVdo( syntaxtree, arg_info);
    TRAVpop();

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}
