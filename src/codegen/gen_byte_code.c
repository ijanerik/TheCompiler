#include "gen_byte_code.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"

#include "isa.h"
#include "constants_table.h"
//#include "util.h"


struct INFO {
    node* constants_table;
    int scope;

    int if_cnt;
    int while_cnt;
    int dowhile_cnt;
    int for_cnt;
};

#define INFO_SCOPE(n)  ((n)->scope)
#define INFO_CONSTANTS_TABLE(n) ((n)->constants_table)
#define INFO_IFCNT(n)  ((n)->if_cnt)
#define INFO_WHILECNT(n)  ((n)->while_cnt)
#define INFO_DOWHILECNT(n)  ((n)->dowhile_cnt)
#define INFO_FORCNT(n)  ((n)->for_cnt)

static info *MakeInfo(void)
{
    info *result;

    DBUG_ENTER( "MakeInfo");

    result = (info *)MEMmalloc(sizeof(info));

    INFO_SCOPE(result) = 0;
    INFO_WHILECNT(result) = 0;
    INFO_DOWHILECNT(result) = 0;
    INFO_FORCNT(result) = 0;
    INFO_IFCNT(result) = 0;

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

void printOp2(char* instruction, int arg1, int arg2) {
    printf("\t%s %d %d\n", instruction, arg1, arg2);
}

void printBranch(char* instruction, char* label, int id) {
    printf("%s %s_%d\n", instruction, label, id);
}

void printLabel(char* label, int id) {
    printf("%s_%d:\n", label, id);
}



// node* GBCconstantstable(node* arg_node, info* arg_info) {
//     DBUG_ENTER("GBCglobaldef");

//     //char* type = cctypeToString(CONSTANTSTABLE_TYPE(arg_node));
//     //printf("%s %s %d\n", CONST_TABLE, type, CONSTANTSTABLE_INDEX(arg_node));

//     DBUG_RETURN(arg_node);
// } 


node* GBCglobaldef(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCglobaldef");

    if(GLOBALDEF_EXPRS(arg_node)) {
        GLOBALDEF_EXPRS(arg_node) = TRAVdo(GLOBALDEF_EXPRS(arg_node), arg_info);
        node* entry = GLOBALDEF_SYMBOLTABLEENTRY(arg_node);
        int index = SYMBOLTABLEENTRY_INDEX(entry);
        printOp1(ISTORE, index);
    }

    DBUG_RETURN(arg_node);
} 

node* GBCfundef(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCfundef");
    
    FUNDEF_FUNHEADER(arg_node) = TRAVdo(FUNDEF_FUNHEADER(arg_node), arg_info);

    if (FUNDEF_FUNBODY(arg_node)) {
        INFO_SCOPE(arg_info) += 1;
        FUNDEF_FUNBODY(arg_node) = TRAVdo(FUNDEF_FUNBODY(arg_node), arg_info);
        INFO_SCOPE(arg_info) -= 1;
    }

    DBUG_RETURN(arg_node);
}

node* GBCvardec(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCvardec");

    if(VARDEC_EXPRS(arg_node)) {
        VARDEC_EXPRS(arg_node) = TRAVdo(VARDEC_EXPRS(arg_node), arg_info);
        node* entry = VARDEC_SYMBOLTABLEENTRY(arg_node);
        int index = SYMBOLTABLEENTRY_INDEX(entry);
        printOp1(ISTORE, index);
    }

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
    int scope = SYMBOLTABLEENTRY_SCOPE(entry);
    int current_scope = INFO_SCOPE(arg_info);

    if (current_scope > scope) {
        printOp2(ILOADN, current_scope - scope, index);
    } else{
        printOp1(ILOAD, index);
    }
    

    DBUG_RETURN(arg_node);
}

node* GBCifelsestmt(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCifelsestmt");

    IFELSESTMT_EXPR(arg_node) = TRAVdo(IFELSESTMT_EXPR(arg_node), arg_info);

    int id = INFO_IFCNT(arg_info);
    node* else_block = IFELSESTMT_ELSEBLOCK(arg_node);
    node* if_block = IFELSESTMT_IFBLOCK(arg_node);

    if (else_block) {
        printBranch(BRANCH_F, "ELSE", id);
        if_block = TRAVdo(if_block, arg_info);
        printBranch(JMP, "ENDIF", id);    
        printLabel("ELSE", id);
        else_block = TRAVdo(else_block, arg_info);
        printLabel("ENDIF", id);
    }
    else {
        printBranch(BRANCH_F, "ENDIF", id);
        if_block = TRAVdo(if_block, arg_info);
        printLabel("ENDIF", id);
    }

    INFO_IFCNT(arg_info) += 1;

    DBUG_RETURN(arg_node);
}

// node* GBCblock(node* arg_node, info* arg_info) {



//     INFO_SCOPE(arg_info) += 1;
//     if (BLOCK_STMTS(arg_node)) {
//         BLOCK_STMTS(arg_node) = TRAVdo(BLOCK_STMTS(arg_node), arg_info);
//     }
//     INFO_SCOPE(arg_info) -= 1;
// }

node* GBCwhilestmt(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCwhilestmt");

    int id = INFO_WHILECNT(arg_info);
    char* WHILE_LABEL = "WHILE";
    char* END_WHILE_LABEL = "ENDWHILE";

    printLabel(WHILE_LABEL, id);
    WHILESTMT_EXPR(arg_node) = TRAVdo(WHILESTMT_EXPR(arg_node), arg_info);

    printBranch(BRANCH_F, END_WHILE_LABEL, id);

    WHILESTMT_BLOCK(arg_node) = TRAVdo(WHILESTMT_BLOCK(arg_node), arg_info);    

    printBranch(JMP, WHILE_LABEL, id);
    printLabel(END_WHILE_LABEL, id);

    DBUG_RETURN(arg_node);
}



node* GBCdowhilestmt(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCwhilestmt");

    int id = INFO_WHILECNT(arg_info);
    char* DOWHILE_LABEL = "DOWHILE";
    
    printLabel(DOWHILE_LABEL, id);
    
    DOWHILESTMT_BLOCK(arg_node) = TRAVdo(DOWHILESTMT_BLOCK(arg_node), arg_info);  
    DOWHILESTMT_EXPR(arg_node) = TRAVdo(DOWHILESTMT_EXPR(arg_node), arg_info);  

    printBranch(BRANCH_T, DOWHILE_LABEL, id);
    
    DBUG_RETURN(arg_node);
}

node* GBCforstmt(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCforstmt");

    int id = INFO_FORCNT(arg_info);
    char* FOR_LABEL = "FOR";
    char* END_FOR_LABEL = "ENDFOR";

    FORSTMT_ASSIGNEXPR(arg_node) = TRAVdo(FORSTMT_ASSIGNEXPR(arg_node), arg_info);
    node* entry = FORSTMT_SYMBOLTABLEENTRY(arg_node);
    printOp1(ISTORE, SYMBOLTABLEENTRY_INDEX(entry));

    printLabel(FOR_LABEL, id);
    printOp1(ILOAD, SYMBOLTABLEENTRY_INDEX(entry));  
    
    FORSTMT_COMPAREEXPR(arg_node) = TRAVdo(FORSTMT_COMPAREEXPR(arg_node), arg_info);

    printOp0(ILT);
    printBranch(BRANCH_F, END_FOR_LABEL, id);

    FORSTMT_BLOCK(arg_node) = TRAVdo(FORSTMT_BLOCK(arg_node), arg_info);    

    FORSTMT_UPDATEEXPR(arg_node) = TRAVdo(FORSTMT_UPDATEEXPR(arg_node), arg_info);

    printOp1(ILOAD, SYMBOLTABLEENTRY_INDEX(entry));
    printOp0(IADD);
    printOp1(ISTORE, SYMBOLTABLEENTRY_INDEX(entry));
    printBranch(JMP, FOR_LABEL, id);
    printLabel(END_FOR_LABEL, id);

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
