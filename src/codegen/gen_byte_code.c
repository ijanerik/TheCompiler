#include "gen_byte_code.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"

#include "isa.h"
#include "constants_table.h"
#include "util.h"

FILE *f;

struct INFO {
    node* constants_table;
    int scope;

    int if_cnt;
    int while_cnt;
    int dowhile_cnt;
    int for_cnt;

    cctype ret_type;

    bool is_returned;
};

#define INFO_SCOPE(n)  ((n)->scope)
#define INFO_CONSTANTS_TABLE(n) ((n)->constants_table)
#define INFO_IFCNT(n)  ((n)->if_cnt)
#define INFO_WHILECNT(n)  ((n)->while_cnt)
#define INFO_DOWHILECNT(n)  ((n)->dowhile_cnt)
#define INFO_FORCNT(n)  ((n)->for_cnt)

#define INFO_RETTYPE(n)  ((n)->ret_type)
#define INFO_IS_RETURNED(n)  ((n)->is_returned)

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
    INFO_RETTYPE(result) = T_unknown;

    DBUG_RETURN( result);
}

static info *FreeInfo( info *info)
{
    DBUG_ENTER ("FreeInfo");

    info = MEMfree( info);

    DBUG_RETURN( info);
}

char* boolToString(bool value) {
    if (value) {
        return "true";
    }
    else {
        return "false";
    }
}

void printOp0(char* instruction) {
    fprintf(f,"\t%s\n", instruction);
}

void printOp1(char* instruction, int arg) {
    fprintf(f,"\t%s %d\n", instruction, arg);
}

void printOp2(char* instruction, int arg1, int arg2) {
    fprintf(f,"\t%s %d %d\n", instruction, arg1, arg2);
}

void printBranch(char* instruction, char* label, int id) {
    fprintf(f,"\t%s %s_%d\n", instruction, label, id);
}

void printLabel(char* label, int id) {
    fprintf(f,"\t\t%s_%d:\n", label, id);
}

void printFunction(char* name) {
    fprintf(f,"%s:\n", name);
}

void printJSR(char* instruction, int num, char* name) {
    fprintf(f,"\t%s %d %s\n", instruction, num, name);
}

void printFunExport(char* instruction, char* name, char* type, char* label) {
    fprintf(f,"%s \"%s\" %s %s\n", instruction, name, type, label);    
}

void printFunImport(char* instruction, char* name, char* type) {
    fprintf(f,"%s \"%s\" %s\n", instruction, name, type);    
}

void printVarImport(char* instruction, char* name, char* type) {
    fprintf(f,"%s \"%s\" %s\n", instruction, name, type);    
}

void printVarExport(char* instruction, char* name, int index) {
    fprintf(f,"%s \"%s\" %d\n", instruction, name, index);    
}

void printGlobal(char* instruction, char* type) {
    fprintf(f,"%s %s\n", instruction, type);    
}


node* GBCprogram(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCprogram");

    if (PROGRAM_CONSTANTSTABLE(arg_node)) {
        PROGRAM_CONSTANTSTABLE(arg_node) = TRAVdo(PROGRAM_CONSTANTSTABLE(arg_node), arg_info);
    }
    printf("\n");
    PROGRAM_DECLARATIONS(arg_node) = TRAVdo(PROGRAM_DECLARATIONS(arg_node), arg_info);
    
    DBUG_RETURN(arg_node);

}

node* GBCconstantstable(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCglobaldef");
    
    node* table = arg_node;

    while(table) {
        cctype type = CONSTANTSTABLE_TYPE(table);
        char* _type = cctypeToString(type);
        if (type == T_int) {
            fprintf(f,"%s %s %d\n", CONST_TABLE, _type, CONSTANTSTABLE_INT(table));
        }
        if (type == T_bool) {
            fprintf(f,"%s %s %s\n", CONST_TABLE, _type, boolToString(CONSTANTSTABLE_BOOL(table)));
        }
        if (type == T_float) {
            fprintf(f,"%s %s %f\n", CONST_TABLE, _type, CONSTANTSTABLE_FLOAT(table));
        }
        table = CONSTANTSTABLE_NEXT(table);
    }
    DBUG_RETURN(arg_node);
} 

node* GBCmonop(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCmonop");

    MONOP_RIGHT(arg_node) = TRAVdo(MONOP_RIGHT(arg_node), arg_info);
    
    printf("IN MONOP");
    cctype type = MONOP_TYPE(arg_node);
    monop op = MONOP_OP(arg_node);

    if (op == MO_not) {
        printOp0(BNOT);
    }
    if (op == MO_neg) {
        if (type == T_int) {
            printOp0(INEG);
        }
        if (type == T_float) {
            printOp0(FNEG);
        }
    }

    DBUG_RETURN(arg_node);
}

node* GBCfuncall(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCfuncall");

    node* fundef = FUNCALL_SYMBOLTABLEENTRY(arg_node);
    int scope = FUNDEF_SCOPE(fundef);
    int current_scope = INFO_SCOPE(arg_info);
    int delta_scope = current_scope - scope;     

    if (delta_scope  == 1) {
        printOp0(ISRL);
    }
    else if (delta_scope > 0) {
        printOp1(ISRN, delta_scope);
    } 
    else {
        printOp0(ISR);
    }

    if (FUNCALL_ARGS(arg_node) != NULL) {
        FUNCALL_ARGS(arg_node) = TRAVdo(FUNCALL_ARGS(arg_node), arg_info);
    }

    int n_args = 0;
    node* args = FUNCALL_ARGS(arg_node);
    while(args) {
        n_args += 1;
        args = EXPRS_NEXT(args);
    }

    char* name = IDENT_NAME(FUNHEADER_IDENT(FUNDEF_FUNHEADER(fundef)));
    printJSR(JSR, n_args, name);

    DBUG_RETURN(arg_node);
}

node* GBCglobaldef(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCglobaldef");

    if(GLOBALDEF_EXPRS(arg_node)) {
        GLOBALDEF_EXPRS(arg_node) = TRAVdo(GLOBALDEF_EXPRS(arg_node), arg_info);
        node* entry = GLOBALDEF_SYMBOLTABLEENTRY(arg_node);
        int index = SYMBOLTABLEENTRY_INDEX(entry);
        cctype type = SYMBOLTABLEENTRY_TYPE(entry);
        
        if (type == T_int) {
            printOp1(ISTORE, index);
        }
        if (type == T_float) {
            printOp1(FSTORE, index);
        }
        if (type == T_bool) {
            printOp1(BSTORE, index);
        }
    }

    DBUG_RETURN(arg_node);
} 

node* GBCfundef(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCfundef");
    
    node* table = FUNDEF_SYMBOLTABLE(arg_node);
    node* funheader = FUNDEF_FUNHEADER(arg_node);


    int var_cnt = 0;
    if(table != NULL) {
        // Next to fix one off error
        table = SYMBOLTABLE_NEXT(table);
        while (table) {
            var_cnt += 1;
            table = SYMBOLTABLE_NEXT(table);
        }
    }
    
    node* ident = FUNHEADER_IDENT(funheader);
    cctype type = FUNHEADER_RETTYPE(funheader);
    char* name = IDENT_NAME(ident);

    if (FUNDEF_EXPORT(arg_node) == TRUE) {
        printFunExport(EXPORT_FUN, name, cctypeToString(type), name);
    }

    if(FUNDEF_FUNBODY(arg_node) == NULL) {
        printFunImport(IMPORT_FUN, name, cctypeToString(type));
    }
    else {
        printFunction(name);
    }
    
    //printf("var_cnt: %d\n", var_cnt);

    if (var_cnt > 0) {
        printOp1(ESR, var_cnt);
    }

    // node* params = FUNHEADER_PARAMS(funheader);
    // int params_cnt = 0;
    // while(params) {
    //     params_cnt += 1;
    //     params = PARAMS_NEXT(params);
    // }

    // printf("params_cnt: %d\n", params_cnt);

    FUNDEF_FUNHEADER(arg_node) = TRAVdo(FUNDEF_FUNHEADER(arg_node), arg_info);



    if (FUNDEF_FUNBODY(arg_node)) {
        INFO_IS_RETURNED(arg_info) = 0;
        INFO_SCOPE(arg_info) += 1;
        cctype temp = INFO_RETTYPE(arg_info);
        INFO_RETTYPE(arg_info) = FUNHEADER_RETTYPE(funheader);
        FUNDEF_FUNBODY(arg_node) = TRAVdo(FUNDEF_FUNBODY(arg_node), arg_info);
        INFO_RETTYPE(arg_info) = temp;
        INFO_SCOPE(arg_info) -= 1;

        if(INFO_IS_RETURNED(arg_info) == 0) {
            printOp0(RETURN);
        }
    }

    fprintf(f,"\n");
    DBUG_RETURN(arg_node);
}

node* GBCreturnstmt(node* arg_node, info* arg_info) {

    DBUG_ENTER("GBCreturnstmt");

    if (RETURNSTMT_EXPR(arg_node)) {
        RETURNSTMT_EXPR(arg_node) = TRAVdo(RETURNSTMT_EXPR(arg_node), arg_info);
    }
    cctype type = INFO_RETTYPE(arg_info);
    
    if (type == T_int) { 
        printOp0(IRETURN);
    }
    if (type == T_bool) { 
        printOp0(BRETURN);
    }
    if (type == T_float) { 
        printOp0(FRETURN);
    }
    if (type == T_void) {
        printOp0(RETURN);
    }

    INFO_IS_RETURNED(arg_info) = 1;

    DBUG_RETURN(arg_node);
}

node* GBCvardec(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCvardec");

    if(VARDEC_EXPRS(arg_node)) {
        VARDEC_EXPRS(arg_node) = TRAVdo(VARDEC_EXPRS(arg_node), arg_info);
        node* entry = VARDEC_SYMBOLTABLEENTRY(arg_node);
        int index = SYMBOLTABLEENTRY_INDEX(entry);
        cctype type = SYMBOLTABLEENTRY_TYPE(entry);
        
        if (type == T_int) {
            printOp1(ISTORE, index);
        }
        if (type == T_float) {
            printOp1(FSTORE, index);
        }
        if (type == T_bool) {
            printOp1(BSTORE, index);
        }
    }

    DBUG_RETURN(arg_node);
}

node* GBCcastexpr(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCcastexpr");

    cctype type = CASTEXPR_TYPE(arg_node);
    cctype expr_type = CASTEXPR_EXPRTYPE(arg_node);

    if (CASTEXPR_CONDEXPR(arg_node)) {
        CASTEXPR_CONDEXPR(arg_node) = TRAVdo(CASTEXPR_CONDEXPR(arg_node), arg_info);
    }
    else {
        CASTEXPR_EXPR(arg_node) = TRAVdo(CASTEXPR_EXPR(arg_node), arg_info);
    }

    
    if (type == T_int && expr_type == T_float) {
        printOp0(F2I);
    }
    if (type == T_float && expr_type == T_int) {
        printOp0(I2F);
    } 

    DBUG_RETURN(arg_node);
}

node* GBCcondexpr(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCcondexpr");
    
    CONDEXPR_EXPR(arg_node) = TRAVdo(CONDEXPR_EXPR(arg_node), arg_info);

    int id = INFO_IFCNT(arg_info);
    printBranch(BRANCH_F, "ELSE", id);
    CONDEXPR_TRUE(arg_node) = TRAVdo(CONDEXPR_TRUE(arg_node), arg_info);
    printBranch(JMP, "ENDIF", id);    
    printLabel("ELSE", id);
    CONDEXPR_FALSE(arg_node) = TRAVdo(CONDEXPR_FALSE(arg_node), arg_info);
    printLabel("ENDIF", id);
    
    INFO_IFCNT(arg_info) += 1;

    DBUG_RETURN(arg_node);
}


node* GBCassign(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCassign");

    ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

    node* entry = ASSIGN_SYMBOLTABLEENTRY(arg_node);
    int index = SYMBOLTABLEENTRY_INDEX(entry);
    cctype type = SYMBOLTABLEENTRY_TYPE(entry);
    int scope = SYMBOLTABLEENTRY_SCOPE(entry);

    if (scope == 0) {
        if (type == T_int) {
            printOp1(ISTOREG, index);
        }
        if (type == T_float) {
            printOp1(FSTOREG, index);
        }
        if (type == T_bool) {
            printOp1(BSTOREG, index);
        }
    }
    else {
        if (type == T_int) {
            printOp1(ISTORE, index);
        }
        if (type == T_float) {
            printOp1(FSTORE, index);
        }
        if (type == T_bool) {
            printOp1(BSTORE, index);
        }
    }
    

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

node* GBCfloat(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCfloat");

    float value = FLOAT_VALUE(arg_node);
    node* table = findConstant(T_float, (void*)&value,
                               INFO_CONSTANTS_TABLE(arg_info));
    if (table) {
        int index = CONSTANTSTABLE_INDEX(table);
        printOp1(FLOADC, index);
    }

    DBUG_RETURN(arg_node);
}

node* GBCbool(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCbool");

    bool value = BOOL_VALUE(arg_node);
    node* table = findConstant(T_bool, (void*)&value,
                               INFO_CONSTANTS_TABLE(arg_info));
    if (table) {
        int index = CONSTANTSTABLE_INDEX(table);
        printOp1(BLOADC, index);
    }

    DBUG_RETURN(arg_node);
}

node* GBCvarcall(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCvarcall");

    node* entry = VARCALL_SYMBOLTABLEENTRY(arg_node);
    cctype type = SYMBOLTABLEENTRY_TYPE(entry);
    int index = SYMBOLTABLEENTRY_INDEX(entry);
    int scope = SYMBOLTABLEENTRY_SCOPE(entry);
    int current_scope = INFO_SCOPE(arg_info);

    if (scope == 0) {
        if (type == T_int) {
            printOp1(ILOADG, index);
        }
        if (type == T_float) {
            printOp1(FLOADG, index);
        }
        if (type == T_bool) {
            printOp1(BLOADG, index);
        }
    }
    else if (current_scope > scope) {
        if (type == T_int) {
            printOp2(ILOADN, current_scope - scope, index);
        }
        if (type == T_float) {
            printOp2(FLOADN, current_scope - scope, index);
        }
        if (type == T_bool) {
            printOp2(BLOADN, current_scope - scope, index);
        }
        
    } else{
        if (type == T_int) {
            printOp1(ILOAD, index);
        }
        if (type == T_float) {
            printOp1(FLOAD, index);
        }
        if (type == T_bool) {
            printOp1(BLOAD, index);
        }
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

    if(FORSTMT_ASSIGNEXPR(arg_node) != NULL) {
        FORSTMT_ASSIGNEXPR(arg_node) = TRAVdo(FORSTMT_ASSIGNEXPR(arg_node), arg_info);
    }
    node* entry = FORSTMT_SYMBOLTABLEENTRY(arg_node);
    printOp1(ISTORE, SYMBOLTABLEENTRY_INDEX(entry));

    printLabel(FOR_LABEL, id);
    printOp1(ILOAD, SYMBOLTABLEENTRY_INDEX(entry));

    if(FORSTMT_COMPAREEXPR(arg_node) != NULL) {
        FORSTMT_COMPAREEXPR(arg_node) = TRAVdo(FORSTMT_COMPAREEXPR(arg_node), arg_info);
    }

    printOp0(ILT);
    printBranch(BRANCH_F, END_FOR_LABEL, id);

    if(FORSTMT_BLOCK(arg_node) != NULL) {
        FORSTMT_BLOCK(arg_node) = TRAVdo(FORSTMT_BLOCK(arg_node), arg_info);
    }

    if(FORSTMT_UPDATEEXPR(arg_node) != NULL) {
        FORSTMT_UPDATEEXPR(arg_node) = TRAVdo(FORSTMT_UPDATEEXPR(arg_node), arg_info);
    }


    printOp1(ILOAD, SYMBOLTABLEENTRY_INDEX(entry));
    printOp0(IADD);
    printOp1(ISTORE, SYMBOLTABLEENTRY_INDEX(entry));
    printBranch(JMP, FOR_LABEL, id);
    printLabel(END_FOR_LABEL, id);

    DBUG_RETURN(arg_node);
}

node* GBCbinop(node* arg_node, info* arg_info) {
    DBUG_ENTER("GBCbinop");

    if ((BINOP_OP(arg_node) == BO_and || BINOP_OP(arg_node) == BO_or) &&
         BINOP_CONDEXPR(arg_node)) {
        BINOP_CONDEXPR(arg_node) = TRAVdo(BINOP_CONDEXPR(arg_node), arg_info);
        DBUG_RETURN(arg_node);
    }
   
    BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);
    BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);
    
   

    /* ARTITHMETIC INSTRUCTIONS */
    if (BINOP_OP(arg_node) == BO_add) {
        if (BINOP_TYPE(arg_node) == T_float) {
            printOp0(FADD);
        }
        if (BINOP_TYPE(arg_node) == T_int) {
            printOp0(IADD);
        }
        
    }

    if (BINOP_OP(arg_node) == BO_sub) {
        if (BINOP_TYPE(arg_node) == T_int) {
            printOp0(ISUB);
        }
        if (BINOP_TYPE(arg_node) == T_float) {
            printOp0(FSUB);
        }
    }

    if (BINOP_OP(arg_node) == BO_mul) {
        if (BINOP_TYPE(arg_node) == T_int) {
            printOp0(IMUL);
        }
        if (BINOP_TYPE(arg_node) == T_float) {
            printOp0(FMUL);
        }
    }

    if (BINOP_OP(arg_node) == BO_div) {
        if (BINOP_TYPE(arg_node) == T_int) {
            printOp0(IDIV);
        }
        if (BINOP_TYPE(arg_node) == T_float) {
            printOp0(FDIV);
        }
    }

    if (BINOP_OP(arg_node) == BO_mod) {
        printOp0(IREM);
    }

    /* CONDITIONALS */
    if (BINOP_OP(arg_node) == BO_lt) {
        if (BINOP_TYPE(arg_node) == T_int) {
            printOp0(ILT);
        }
        if (BINOP_TYPE(arg_node) == T_float) {
            printOp0(FLT);
        }
    }

    if (BINOP_OP(arg_node) == BO_le) {
        if (BINOP_TYPE(arg_node) == T_int) {
            printOp0(ILE);
        }
        if (BINOP_TYPE(arg_node) == T_float) {
            printOp0(FLE);
        }
    }

    if (BINOP_OP(arg_node) == BO_gt) {
        if (BINOP_TYPE(arg_node) == T_int) {
            printOp0(IGT);
        }
        if (BINOP_TYPE(arg_node) == T_float) {
            printOp0(FGT);
        }
    }

    if (BINOP_OP(arg_node) == BO_ge) {
        if (BINOP_TYPE(arg_node) == T_int) {
            printOp0(IGE);
        }
        if (BINOP_TYPE(arg_node) == T_float) {
            printOp0(FGE);
        }
    }

    if (BINOP_OP(arg_node) == BO_eq) {
        if (BINOP_TYPE(arg_node) == T_int) {
            printOp0(IEQ);
        }
        if (BINOP_TYPE(arg_node) == T_float) {
            printOp0(FEQ);
        }
        if (BINOP_TYPE(arg_node) == T_bool) {
            printOp0(BEQ);
        }
    }

    if (BINOP_OP(arg_node) == BO_ne) {
        if (BINOP_TYPE(arg_node) == T_int) {
            printOp0(INE);
        }
        if (BINOP_TYPE(arg_node) == T_float) {
            printOp0(FNE);
        }
        if (BINOP_TYPE(arg_node) == T_bool) {
            printOp0(BNE);
        }
    }

    DBUG_RETURN(arg_node);    
}

node *GBCdoGenByteCode( node *syntaxtree)
{
    DBUG_ENTER("GBCdoGenByteCode");

    f = fopen("out.cvo", "w");

    info *arg_info;

    arg_info = MakeInfo();
    /* Import variables */
    node* declarations = PROGRAM_DECLARATIONS(syntaxtree);
    while (declarations) {
        node* declaration = DECLARATIONS_DECLARATION(declarations);
        if (NODE_TYPE(declaration) == N_globaldec) {
            char* name = IDENT_NAME(GLOBALDEC_IDENT(declaration));
            char* type = cctypeToString(GLOBALDEC_TYPE(declaration));
            printVarImport(IMPORT_VAR, name, type);

        }
        declarations = DECLARATIONS_NEXT(declarations);
    }

    
    /* Export variables*/
    declarations = PROGRAM_DECLARATIONS(syntaxtree);
    while (declarations) {
        
        node* declaration = DECLARATIONS_DECLARATION(declarations);
        if (NODE_TYPE(declaration) == N_globaldef &&
            GLOBALDEF_EXPORT(declaration) == TRUE) {    
            char* name = IDENT_NAME(GLOBALDEF_IDENT(declaration));
            
            node* entry = GLOBALDEF_SYMBOLTABLEENTRY(declaration);
            int index = SYMBOLTABLEENTRY_INDEX(entry);
            printVarExport(EXPORT_VAR, name, index);

        }
        declarations = DECLARATIONS_NEXT(declarations);
    }

    /* Global variables*/
    declarations = PROGRAM_DECLARATIONS(syntaxtree);
    while (declarations) {
        
        node* declaration = DECLARATIONS_DECLARATION(declarations);
        if (NODE_TYPE(declaration) == N_globaldec) {
            node* entry = GLOBALDEC_SYMBOLTABLEENTRY(declaration);
            char* type = cctypeToString(SYMBOLTABLEENTRY_TYPE(entry));
            printGlobal(GLOBAL_TABLE, type);
        }
        if (NODE_TYPE(declaration) == N_globaldef) {
            node* entry = GLOBALDEF_SYMBOLTABLEENTRY(declaration);
            char* type = cctypeToString(SYMBOLTABLEENTRY_TYPE(entry));
            printGlobal(GLOBAL_TABLE, type);
        }
        declarations = DECLARATIONS_NEXT(declarations);
    }

    TRAVpush( TR_gbc);
    INFO_CONSTANTS_TABLE(arg_info) = PROGRAM_CONSTANTSTABLE(syntaxtree);
    syntaxtree = TRAVdo( syntaxtree, arg_info);
    TRAVpop();

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}
