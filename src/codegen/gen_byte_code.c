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

    int if_cnt;
};

#define INFO_SCOPE(n)  ((n)->scope)
#define INFO_CONSTANTS_TABLE(n) ((n)->constants_table)
#define INFO_IFCNT(n)  ((n)->scope)

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
    printf("\t%s\n", instruction);
}

void printOp1(char* instruction, int arg) {
    printf("\t%s %d\n", instruction, arg);
}

node* GBCvardec(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCvardec");

    VARDEC_EXPRS(arg_node) = TRAVdo(VARDEC_EXPRS(arg_node), arg_info);

    node* entry = VARDEC_SYMBOLTABLEENTRY(arg_node);
    int index = SYMBOLTABLEENTRY_INDEX(entry);

    printOp1(ISTORE, index);

    DBUG_RETURN(arg_node);
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

node* GBCifelsestmt(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCifelsestmt");

    IFELSESTMT_EXPR(arg_node) = TRAVdo(IFELSESTMT_EXPR(arg_node), arg_info);

    int id = INFO_IFCNT(arg_info);
    node* else_block = IFELSESTMT_ELSEBLOCK(arg_node);
    node* if_block = IFELSESTMT_IFBLOCK(arg_node);

    if (else_block) {
        printf("%s %s_%d\n", BRANCH_F, "ELSE", id);
        if_block = TRAVdo(if_block, arg_info);
        printf("%s %s_%d\n", JMP, "ENDIF", id);    
        printf("%s_%d:\n", "ELSE", id);
        else_block = TRAVdo(else_block, arg_info);
        printf("%s_%d:\n", "ENDIF", id);
    }
    else {
        printf("%s %s_%d\n", BRANCH_F, "ENDIF", id);
        if_block = TRAVdo(if_block, arg_info);
        printf("%s_%d:\n", "ENDIF", id);
    }

    INFO_IFCNT(arg_info) += 1;

    DBUG_RETURN(arg_node);
}

node* GBCbinop(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCbinop");

    BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);
    BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);

    // Integer Operations
    if (BINOP_OP(arg_node) == BO_add) {
        printOp0(IADD);
    }

    if (BINOP_OP(arg_node) == BO_sub) {
        printOp0(ISUB);
    }

    if (BINOP_OP(arg_node) == BO_mul) {
        printOp0(IMUL);
    }

    if (BINOP_OP(arg_node) == BO_div) {
        printOp0(IDIV);
    }

    if (BINOP_OP(arg_node) == BO_mod) {
        printOp0(IREM);
    }

    // Integer Conditionals
    if (BINOP_OP(arg_node) == BO_lt) {
        printOp0(ILT);
    }

    if (BINOP_OP(arg_node) == BO_le) {
        printOp0(ILE);
    }

    if (BINOP_OP(arg_node) == BO_gt) {
        printOp0(IGT);
    }

    if (BINOP_OP(arg_node) == BO_ge) {
        printOp0(IGE);
    }

    if (BINOP_OP(arg_node) == BO_eq) {
        printOp0(IEQ);
    }

    if (BINOP_OP(arg_node) == BO_ne) {
        printOp0(INE);
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
