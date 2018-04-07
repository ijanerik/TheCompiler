
/**
 * @file print.c
 *
 * Functions needed by print traversal.
 *
 */

/**
 * @defgroup print Print Functions.
 *
 * Functions needed by print traversal.
 *
 * @{
 */


#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"


/*
 * INFO structure
 */
struct INFO {
    bool firsterror;
    int indents;
};

#define INFO_FIRSTERROR(n) ((n)->firsterror)
#define INFO_INDENTS(n) ((n)->indents)

static info *MakeInfo() {
    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_FIRSTERROR(result) = FALSE;
    INFO_INDENTS(result) = 0;

    return result;
}


static info *FreeInfo(info *info) {
    info = MEMfree(info);

    return info;
}

static void printIndents(info *info) {
    for(int i = 0; i < INFO_INDENTS(info); i++) {
        printf("\t");
    }
}

static void typeToString(cctype type, char* string) {
    switch (type) {
        case T_float:
            string = "float";
            break;
        case T_int:
            string = "int";
            break;
        case T_bool:
            string = "bool";
            break;
        default:
            string = "unknown";
    }
}




/** <!--******************************************************************-->
 *
 * @fn PRTstmts
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node*
PRTstmts(node*arg_node, info *arg_info) {
    DBUG_ENTER("PRTstmts");

    if(STMTS_STMT(arg_node) != NULL) {
        STMTS_STMT(arg_node) = TRAVdo(STMTS_STMT(arg_node), arg_info);
    }

    if(STMTS_NEXT(arg_node) != NULL) {
        STMTS_NEXT(arg_node) = TRAVopt(STMTS_NEXT(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTassign
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node*
PRTassign(node*arg_node, info *arg_info) {
    DBUG_ENTER("PRTassign");

    printIndents(arg_info);

    ASSIGN_LET(arg_node) = TRAVdo(ASSIGN_LET(arg_node), arg_info);

    if (ASSIGN_INDEX(arg_node) != NULL) {
        printf(" [");
         ASSIGN_INDEX(arg_node) = TRAVdo(ASSIGN_INDEX(arg_node), arg_info);
        printf("] ");
    }
    
    printf(" = ");

    ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), arg_info);

    printf(";\n");

    DBUG_RETURN(arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTbinop
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node* 
PRTbinop(node* arg_node, info *arg_info) {
    char *tmp;

    DBUG_ENTER("PRTbinop");

    printf("(");

    BINOP_LEFT(arg_node) = TRAVdo(BINOP_LEFT(arg_node), arg_info);

    switch (BINOP_OP(arg_node)) {
        case BO_add:
            tmp = "+";
            break;
        case BO_sub:
            tmp = "-";
            break;
        case BO_mul:
            tmp = "*";
            break;
        case BO_div:
            tmp = "/";
            break;
        case BO_mod:
            tmp = "%";
            break;
        case BO_lt:
            tmp = "<";
            break;
        case BO_le:
            tmp = "<=";
            break;
        case BO_gt:
            tmp = ">";
            break;
        case BO_ge:
            tmp = ">=";
            break;
        case BO_eq:
            tmp = "==";
            break;
        case BO_ne:
            tmp = "!=";
            break;
        case BO_or:
            tmp = "||";
            break;
        case BO_and:
            tmp = "&&";
            break;
        case BO_unknown:
            DBUG_ASSERT(0, "unknown binop detected!");
    }

    printf(" %s ", tmp);

    BINOP_RIGHT(arg_node) = TRAVdo(BINOP_RIGHT(arg_node), arg_info);

    printf(")");

    DBUG_RETURN(arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTfloat
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Float node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node* 
PRTfloat(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTfloat");

    printf("%f", FLOAT_VALUE(arg_node));
    if (FLOAT_CONSTANTSTABLE(arg_node)) {
        printf("/*%f*/", CONSTANTSTABLE_FLOAT(FLOAT_CONSTANTSTABLE(arg_node)));
    }

    DBUG_RETURN(arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTnum
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Num node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node* 
PRTnum(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTnum");

    printf("%i", NUM_VALUE(arg_node));
    if (NUM_CONSTANTSTABLE(arg_node)) {
        printf("/*%d, %p*/", CONSTANTSTABLE_INT(NUM_CONSTANTSTABLE(arg_node)),
                             NUM_CONSTANTSTABLE(arg_node));
    }

    DBUG_RETURN(arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTboolean
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Boolean node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node* 
PRTbool(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTbool");

    if (BOOL_VALUE(arg_node)) {
        printf("true");
    } else {
        printf("false");
    }
    
    if (BOOL_CONSTANTSTABLE(arg_node)) {
        printf("/*%d*/", CONSTANTSTABLE_BOOL(BOOL_CONSTANTSTABLE(arg_node)));
    }

    DBUG_RETURN(arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTvar
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node* 
PRTident(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTident");

    printf("%s", IDENT_NAME(arg_node));

    DBUG_RETURN(arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn PRTerror
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node* 
PRTerror(node* arg_node, info *arg_info) {
    bool first_error;

    DBUG_ENTER("PRTerror");

    if (NODE_ERROR(arg_node) != NULL) {
        NODE_ERROR(arg_node) = TRAVdo(NODE_ERROR(arg_node), arg_info);
    }

    first_error = INFO_FIRSTERROR(arg_info);

    if ((global.outfile != NULL)
        && (ERROR_ANYPHASE(arg_node) == global.compiler_anyphase)) {

        if (first_error) {
            printf("\n/******* BEGIN TREE CORRUPTION ********\n");
            INFO_FIRSTERROR(arg_info) = FALSE;
        }

        printf("%s\n", ERROR_MESSAGE(arg_node));

        if (ERROR_NEXT(arg_node) != NULL) {
            TRAVopt(ERROR_NEXT(arg_node), arg_info);
        }

        if (first_error) {
            printf("********  END TREE CORRUPTION  *******/\n");
            INFO_FIRSTERROR(arg_info) = TRUE;
        }
    }

    DBUG_RETURN(arg_node);
}


/** <!-- ****************************************************************** -->
 * @brief Prints the given syntaxtree
 * 
 * @param syntaxtree a node structure
 * 
 * @return the unchanged nodestructure
 ******************************************************************************/

node
*PRTdoPrint(node* syntaxtree) {
    info *info;

    DBUG_ENTER("PRTdoPrint");

    DBUG_ASSERT((syntaxtree != NULL), "PRTdoPrint called with empty syntaxtree");

    printf("\n\n------------------------------\n\n");

    info = MakeInfo();

    TRAVpush(TR_prt);

    syntaxtree = TRAVdo(syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    printf("\n------------------------------\n\n");

    DBUG_RETURN(syntaxtree);
}


/* OWN NODES */
node* PRTcastexpr(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTcastexpr");
    printf("(");
    switch (CASTEXPR_TYPE(arg_node)) {
        case T_int:
            printf("(int)");
            break;
        case T_float:
            printf("(float)");
            break;
        case T_bool:
            printf("(bool)");
            break;
        default:
            printf("(unknown)");
            break;
    }

    CASTEXPR_EXPR(arg_node) = TRAVdo(CASTEXPR_EXPR(arg_node), arg_info);
    printf(")");

    DBUG_RETURN(arg_node);
}

node* PRTmonop(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTmonop");

    printf("(");
    switch (MONOP_OP(arg_node)) {
        case MO_not:
            printf("!");
            break;
        case MO_neg:
            printf("-");
            break;
        default:
            printf("mo_unknown");
            break;
    }

    MONOP_RIGHT(arg_node) = TRAVdo(MONOP_RIGHT(arg_node), arg_info);
    printf(")");

    DBUG_RETURN(arg_node);
}

node* PRTexprs(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTexprs");
    EXPRS_EXPR(arg_node) = TRAVdo(EXPRS_EXPR(arg_node), arg_info);
    if (EXPRS_NEXT(arg_node) != NULL) {
        printf(", ");
        EXPRS_NEXT(arg_node) = TRAVdo(EXPRS_NEXT(arg_node), arg_info);
    }
    DBUG_RETURN(arg_node);
}

node* PRTreturnstmt(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTreturnstmt");
    printIndents(arg_info);
    printf("return ");
    if (RETURNSTMT_EXPR(arg_node)) {
        RETURNSTMT_EXPR(arg_node) = TRAVdo(RETURNSTMT_EXPR(arg_node), arg_info);
    }
    printf(";\n");

    DBUG_RETURN(arg_node);
}

node* PRTforstmt(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTforstmt");

    printIndents(arg_info);

    printf("for (int ");
    FORSTMT_ASSIGNVAR(arg_node) = TRAVdo(FORSTMT_ASSIGNVAR(arg_node), arg_info);
    printf(" = ");
    FORSTMT_ASSIGNEXPR(arg_node) = TRAVdo(FORSTMT_ASSIGNEXPR(arg_node), arg_info);
    printf(", ");
    FORSTMT_COMPAREEXPR(arg_node) = TRAVdo(FORSTMT_COMPAREEXPR(arg_node), arg_info);
    if(FORSTMT_UPDATEEXPR(arg_node) != NULL) {
        printf(", ");
        FORSTMT_UPDATEEXPR(arg_node) = TRAVdo(FORSTMT_UPDATEEXPR(arg_node), arg_info);
    }
    printf(") ");
    FORSTMT_BLOCK(arg_node) = TRAVdo(FORSTMT_BLOCK(arg_node), arg_info);
    //printf("for (%s = %s, %s, %s) {\n %s;\n }", "ASSIGN_VAR", "ASSIGN_EXPR",
    //       "COMPARE_EXPR", "UPDATE_EXPR", "BLOCK");
    DBUG_RETURN(arg_node);
}

node* PRTdowhilestmt(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTdowhilestm");

    printIndents(arg_info);
    printf("do ");
    DOWHILESTMT_BLOCK(arg_node) = TRAVdo(DOWHILESTMT_BLOCK(arg_node), arg_info);

    printIndents(arg_info);
    printf("while (");
    DOWHILESTMT_EXPR(arg_node) = TRAVdo(DOWHILESTMT_EXPR(arg_node), arg_info);
    printf(");");
    
    DBUG_RETURN(arg_node);    

    DBUG_RETURN(arg_node);
}

node* PRTwhilestmt(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTwhilestmt");

    printIndents(arg_info);

    printf("while (");
    WHILESTMT_EXPR(arg_node) = TRAVdo(WHILESTMT_EXPR(arg_node), arg_info);
    printf(")");
    WHILESTMT_BLOCK(arg_node) = TRAVdo(WHILESTMT_BLOCK(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node* PRTifelsestmt(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTifelsestmt");

    printIndents(arg_info);
    printf("if(");
    IFELSESTMT_EXPR(arg_node) = TRAVdo(IFELSESTMT_EXPR(arg_node), arg_info);
    printf(") ");

    IFELSESTMT_IFBLOCK(arg_node) = TRAVdo(IFELSESTMT_IFBLOCK(arg_node), arg_info);

    if(IFELSESTMT_ELSEBLOCK(arg_node) != NULL) {
        printIndents(arg_info);
        printf("else ");
        IFELSESTMT_ELSEBLOCK(arg_node) = TRAVdo(IFELSESTMT_ELSEBLOCK(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

node* PRTfuncall(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTfuncall");
    
    FUNCALL_IDENT(arg_node) = TRAVdo(FUNCALL_IDENT(arg_node), arg_info);
    printf("(");
    if(FUNCALL_ARGS(arg_node) != NULL) {
        FUNCALL_ARGS(arg_node) = TRAVdo(FUNCALL_ARGS(arg_node), arg_info);
    }
    printf(")");

    DBUG_RETURN(arg_node);
}

node* PRTblock(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTblock");

    printf("{\n");
    INFO_INDENTS(arg_info)++;

    // Print symbol table
    if(BLOCK_SYMBOLTABLE(arg_node) != NULL) {
        printIndents(arg_info);
        printf("/** SYMBOL TABLE **\n");
        BLOCK_SYMBOLTABLE(arg_node) = TRAVdo(BLOCK_SYMBOLTABLE(arg_node), arg_info);
        printIndents(arg_info);
        printf(" */\n");
    }


    BLOCK_STMTS(arg_node) = TRAVdo(BLOCK_STMTS(arg_node), arg_info);
    INFO_INDENTS(arg_info)--;
    printIndents(arg_info);
    printf("}\n");

    DBUG_RETURN(arg_node);
}

node* PRTvardec(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTvardec");

    printIndents(arg_info);
    bool is_array = VARDEC_ARRAYLENGTH(arg_node) != NULL;

    switch(VARDEC_TYPE(arg_node)) {
        case T_int:
            printf("int");
            break;
        case T_float:
            printf("float");
            break;
        case T_bool:
            printf("bool");
            break;
        default:
            printf("unknown");
            break; 
    }
    printf(" ");

    if (is_array) {
        printf("[");
        VARDEC_ARRAYLENGTH(arg_node) = TRAVdo(VARDEC_ARRAYLENGTH(arg_node), arg_info);
        printf("] ");
    }

    VARDEC_IDENT(arg_node) = TRAVdo(VARDEC_IDENT(arg_node), arg_info);
    
    if (VARDEC_EXPRS(arg_node) != NULL) {
        printf(" = ");
        
        if (EXPRS_EXPR(VARDEC_EXPRS(arg_node)) != NULL) {
            VARDEC_EXPRS(arg_node) = TRAVdo(EXPRS_EXPR(VARDEC_EXPRS(arg_node)), arg_info);
        }
        else if (EXPRS_NEXT(VARDEC_EXPRS(arg_node)) != NULL) {
            
            if (is_array) { printf("["); }
            VARDEC_EXPRS(arg_node) = TRAVdo(EXPRS_NEXT(
                                               VARDEC_EXPRS(arg_node))
                                               , arg_info);
            if (is_array) { printf("]"); }
        }
    }

    printf(";\n");
    DBUG_RETURN(arg_node);
}

node* PRTvarcall(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTvarcall");
    VARCALL_IDENT(arg_node) = TRAVdo(VARCALL_IDENT(arg_node), arg_info);
    DBUG_RETURN(arg_node);
}

node* PRTvardecs(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTvardecs");

    if (VARDECS_VARDEC(arg_node) != NULL) {
        VARDECS_VARDEC(arg_node) = TRAVdo(VARDECS_VARDEC(arg_node), arg_info);
    }
    
    if (VARDECS_NEXT(arg_node) != NULL) {
        VARDECS_NEXT(arg_node) = TRAVdo(VARDECS_NEXT(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

node* PRTfundefs(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTfundefs");

    if(FUNDEFS_FUNDEF(arg_node) != NULL) {
        FUNDEFS_FUNDEF(arg_node) = TRAVdo(FUNDEFS_FUNDEF(arg_node), arg_info);
    }

    if (FUNDEFS_NEXT(arg_node) != NULL) {
        FUNDEFS_NEXT(arg_node) = TRAVdo(FUNDEFS_NEXT(arg_node), arg_info);
    }
    DBUG_RETURN(arg_node);
}

node* PRTfunbody(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTfunbody");
    printIndents(arg_info);
    printf(" {\n");
    INFO_INDENTS(arg_info)++;

    // Var declarations
    if (FUNBODY_VARDECS(arg_node) != NULL) {
        FUNBODY_VARDECS(arg_node) = TRAVdo(FUNBODY_VARDECS(arg_node), arg_info);
    }

    if (FUNBODY_FUNDEFS(arg_node) != NULL) {
        FUNBODY_FUNDEFS(arg_node) = TRAVdo(FUNBODY_FUNDEFS(arg_node), arg_info);
    }

    if (FUNBODY_STMTS(arg_node) != NULL) {
        FUNBODY_STMTS(arg_node) = TRAVdo(FUNBODY_STMTS(arg_node), arg_info);
    }

    INFO_INDENTS(arg_info)--;

    printIndents(arg_info);
    printf("}\n");
    DBUG_RETURN(arg_node);
}

node* PRTparam(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTparam");

    switch (PARAM_TYPE(arg_node)) {
        case T_int:
            printf("int");
            break;
        case T_float:
            printf("float");
            break;
        case T_bool:
            printf("bool");
            break;
        default:
            printf("unknown");
            break; 
    }
    printf(" ");
    if (PARAM_ARRAYLENGTH(arg_node) != NULL) {
        printf("[");
        PARAM_ARRAYLENGTH(arg_node) = TRAVdo(PARAM_ARRAYLENGTH(arg_node), arg_info);
        printf("] ");
    }

    PARAM_IDENT(arg_node) = TRAVdo(PARAM_IDENT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node* PRTparams(node* arg_node, info *arg_info) {
    DBUG_ENTER("");

    PARAMS_PARAM(arg_node) = TRAVdo(PARAMS_PARAM(arg_node), arg_info);

    if (PARAMS_NEXT(arg_node) != NULL) {
        printf(", ");
        PARAMS_NEXT(arg_node) = TRAVdo(PARAMS_NEXT(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

/**
 * Global definition
 * @param arg_node
 * @param arg_info
 * @return
 */
node* PRTglobaldef(node* arg_node, info *arg_info) {
    char* tmp;

    DBUG_ENTER("PRTglobaldef");

    bool is_array = GLOBALDEF_ARRAYLENGTH(arg_node) != NULL;

    if (GLOBALDEF_EXPORT(arg_node) == TRUE) {
        printf("export ");
    }


    switch (GLOBALDEF_TYPE(arg_node)) {
        case T_float:
            tmp = "float";
            break;
        case T_int:
            tmp = "int";
            break;
        case T_bool:
            tmp = "bool";
            break;
        default:
            DBUG_ASSERT(0, "unknown/incorrect returntype detected!");
    }
    printf("%s ", tmp);

    if (is_array) {
        printf("[");
        GLOBALDEF_ARRAYLENGTH(arg_node) = TRAVdo(GLOBALDEF_ARRAYLENGTH(arg_node), arg_info);
        printf("] ");
    }

    GLOBALDEF_IDENT(arg_node) = TRAVdo(GLOBALDEF_IDENT(arg_node), arg_info);

    if (GLOBALDEF_EXPRS(arg_node) != NULL) {
        printf(" = ");
        
        if (EXPRS_EXPR(GLOBALDEF_EXPRS(arg_node)) != NULL) {
            GLOBALDEF_EXPRS(arg_node) = TRAVdo(EXPRS_EXPR(GLOBALDEF_EXPRS(arg_node)), arg_info);
        }
        else if (EXPRS_NEXT(GLOBALDEF_EXPRS(arg_node)) != NULL) {
            
            if (is_array) { printf("["); }
            GLOBALDEF_EXPRS(arg_node) = TRAVdo(EXPRS_NEXT(
                                               GLOBALDEF_EXPRS(arg_node))
                                               , arg_info);
            if (is_array) { printf("]"); }
        }
    }

    printf(";");
    DBUG_RETURN(arg_node);
}


node* PRTglobaldec(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTglobaldec");

    printf("extern ");
    switch (GLOBALDEC_TYPE(arg_node)) {
        case T_int:
            printf("int");
            break;
        case T_float:
            printf("float");
            break;
        case T_bool:
            printf("bool");
            break;
        default:
            printf("unknown");
            break; 
    }
    printf(" ");
    if (GLOBALDEC_ARRAYLENGTH(arg_node) != NULL) {
        printf("[");
        GLOBALDEC_ARRAYLENGTH(arg_node) = TRAVdo(GLOBALDEC_ARRAYLENGTH(arg_node), arg_info);
        printf("] ");
    }
    GLOBALDEC_IDENT(arg_node) = TRAVdo(GLOBALDEC_IDENT(arg_node), arg_info);

    printf(";\n");
    DBUG_RETURN(arg_node);
}

node* PRTfunheader(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTfunheader");

    switch (FUNHEADER_RETTYPE(arg_node)) {
        case T_float:
            printf("float");
            break;
        case T_int:
            printf("int");
            break;
        case T_bool:
            printf("bool");
            break;
        case T_void:
            printf("void");
            break;
        default:
            printf("unknown");
    }
    printf(" ");

    FUNHEADER_IDENT(arg_node) = TRAVdo(FUNHEADER_IDENT(arg_node), arg_info);
    
    printf("(");
    if (FUNHEADER_PARAMS(arg_node) != NULL) {
        FUNHEADER_PARAMS(arg_node) = TRAVdo(FUNHEADER_PARAMS(arg_node), arg_info);
    } 
    printf(")");

    DBUG_RETURN(arg_node);
}

// todo add attribute
node* PRTfundef(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTfundef");

    printIndents(arg_info);
    
    // Print symbol table
    printf("\n/** FUNCTION SYMBOL TABLE **\n * ID\tTYPE\tISARRAY\tINDEX\tSCOPE\n");
    if(FUNDEF_SYMBOLTABLE(arg_node) != NULL) {
        FUNDEF_SYMBOLTABLE(arg_node) = TRAVdo(FUNDEF_SYMBOLTABLE(arg_node), arg_info);
    }
    printIndents(arg_info);
    printf(" */\n");

    if (FUNDEF_FUNBODY(arg_node) == NULL && FUNDEF_EXPORT(arg_node) == FALSE) {
        printIndents(arg_info);
        printf("extern ");
        FUNDEF_FUNHEADER(arg_node) = TRAVdo(FUNDEF_FUNHEADER(arg_node), arg_info);
        printf(";\n");
        DBUG_RETURN(arg_node);
    } else {
        printIndents(arg_info);
        if (FUNDEF_EXPORT(arg_node)) {
            printf("export ");
        }
        if (FUNDEF_FUNHEADER(arg_node) != NULL) {
            FUNDEF_FUNHEADER(arg_node) = TRAVdo(FUNDEF_FUNHEADER(arg_node), arg_info);
        }

        if (FUNDEF_FUNBODY(arg_node) != NULL) {
            FUNDEF_FUNBODY(arg_node) = TRAVdo(FUNDEF_FUNBODY(arg_node), arg_info);
        }
    }

    DBUG_RETURN(arg_node);
}

node* PRTdeclarations(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTdeclarations");


    DECLARATIONS_DECLARATION(arg_node) = TRAVdo(DECLARATIONS_DECLARATION(arg_node), arg_info);
    printf("\n");
    if(DECLARATIONS_NEXT(arg_node) != NULL) {
        DECLARATIONS_NEXT(arg_node) = TRAVdo(DECLARATIONS_NEXT(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

node* PRTprogram(node* arg_node, info *arg_info) {
    DBUG_ENTER("");

    printIndents(arg_info);
    printf("/** CONSTANTS TABLE\n");
    if(PROGRAM_CONSTANTSTABLE(arg_node) != NULL) {
        PROGRAM_CONSTANTSTABLE(arg_node) = TRAVdo(PROGRAM_CONSTANTSTABLE(arg_node), arg_info);
    }
    printf("\n");

    printf("/** GLOBAL SYMBOL TABLE **\n * ID\tTYPE\tISARRAY\tINDEX\tSCOPE\n");
    if(PROGRAM_SYMBOLTABLE(arg_node) != NULL) {
        PROGRAM_SYMBOLTABLE(arg_node) = TRAVdo(PROGRAM_SYMBOLTABLE(arg_node), arg_info);
    }
    printIndents(arg_info);
    printf(" */\n");
    
    if(PROGRAM_DECLARATIONS(arg_node) != NULL) {
        PROGRAM_DECLARATIONS(arg_node) = TRAVdo(PROGRAM_DECLARATIONS(arg_node), arg_info);
    }

    

    DBUG_RETURN(arg_node);
}

node* PRTsymboltable(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTsymboltable");

    if(SYMBOLTABLE_SYMBOLTABLEENTRY(arg_node) != NULL) {
        SYMBOLTABLE_SYMBOLTABLEENTRY(arg_node) = TRAVdo(SYMBOLTABLE_SYMBOLTABLEENTRY(arg_node), arg_info);
    }

    if(SYMBOLTABLE_NEXT(arg_node) != NULL) {
        SYMBOLTABLE_NEXT(arg_node) = TRAVdo(SYMBOLTABLE_NEXT(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

node* PRTsymboltableentry(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTsymboltableentry");

    printIndents(arg_info);

    
    char* type;
    switch (SYMBOLTABLEENTRY_TYPE(arg_node)) {
        case T_float:
            type = "float";
            break;
        case T_int:
            type = "int";
            break;
        case T_bool:
            type = "bool";
            break;
        default:
            DBUG_ASSERT(0, "unknown/incorrect returntype detected!");
    }
    
    printf(" * %s\t%s\t%d\t%d\t%d\n", SYMBOLTABLEENTRY_NAME(arg_node),
                               type,
                               SYMBOLTABLEENTRY_ISARRAY(arg_node),
                               SYMBOLTABLEENTRY_INDEX(arg_node),
                               SYMBOLTABLEENTRY_SCOPE(arg_node));

    DBUG_RETURN(arg_node);
}

node* PRTarrayindex(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTarrayindex");

    ARRAYINDEX_IDENT(arg_node) = TRAVdo(ARRAYINDEX_IDENT(arg_node), arg_info);
    printf("[");
    ARRAYINDEX_INDEX(arg_node) = TRAVdo(ARRAYINDEX_INDEX(arg_node), arg_info);
    printf("]");

    DBUG_RETURN(arg_node);
}

node* PRTcondexpr(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTcondexpr");
    
    printf("(");
    CONDEXPR_EXPR(arg_node) = TRAVdo(CONDEXPR_EXPR(arg_node), arg_info);
    printf(" ? ");
    CONDEXPR_TRUE(arg_node) = TRAVdo(CONDEXPR_TRUE(arg_node), arg_info);
    printf(" : ");
    CONDEXPR_FALSE(arg_node) = TRAVdo(CONDEXPR_FALSE(arg_node), arg_info);
    printf(")");

    DBUG_RETURN(arg_node);
}

node* PRTconstantstable(node* arg_node, info *arg_info) {
    DBUG_ENTER("PRTconstantstable");
    
    int index = CONSTANTSTABLE_INDEX(arg_node);
    cctype type = CONSTANTSTABLE_TYPE(arg_node);

    if (type == T_float) {
        printf(" * %d\tfloat\t%f\n", index, CONSTANTSTABLE_FLOAT(arg_node));
    }
    if (type == T_bool) {
        printf(" * %d\tbool\t%d\n", index, CONSTANTSTABLE_BOOL(arg_node));
    }
    if (type == T_int) {
        printf(" * %d\tint\t%d\n", index, CONSTANTSTABLE_INT(arg_node));
    }

    if (CONSTANTSTABLE_NEXT(arg_node)) {
        CONSTANTSTABLE_NEXT(arg_node) = TRAVdo(CONSTANTSTABLE_NEXT(arg_node), arg_info);
    }
    DBUG_RETURN(arg_node);
}



/**
 * @}
 */
