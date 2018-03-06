
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
};

#define INFO_FIRSTERROR(n) ((n)->firsterror)

static info *MakeInfo() {
    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_FIRSTERROR(result) = FALSE;

    return result;
}


static info *FreeInfo(info *info) {
    info = MEMfree(info);

    return info;
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

node *
PRTstmts(node *arg_node, info *arg_info) {
    DBUG_ENTER("PRTstmts");

    STMTS_STMT(arg_node) = TRAVdo(STMTS_STMT(arg_node), arg_info);

    STMTS_NEXT(arg_node) = TRAVopt(STMTS_NEXT(arg_node), arg_info);

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

node *
PRTassign(node *arg_node, info *arg_info) {
    DBUG_ENTER("PRTassign");

    if (ASSIGN_LET(arg_node) != NULL) {
        ASSIGN_LET(arg_node) = TRAVdo(ASSIGN_LET(arg_node), arg_info);
        printf(" = ");
    }

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

node *
PRTbinop(node *arg_node, info *arg_info) {
    char *tmp;

    DBUG_ENTER("PRTbinop");

    printf("( ");

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

node *
PRTfloat(node *arg_node, info *arg_info) {
    DBUG_ENTER("PRTfloat");

    printf("%f", FLOAT_VALUE(arg_node));

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

node *
PRTnum(node *arg_node, info *arg_info) {
    DBUG_ENTER("PRTnum");

    printf("%i", NUM_VALUE(arg_node));

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

node *
PRTbool(node *arg_node, info *arg_info) {
    DBUG_ENTER("PRTbool");

    if (BOOL_VALUE(arg_node)) {
        printf("true");
    } else {
        printf("false");
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

node *
PRTident(node *arg_node, info *arg_info) {
    DBUG_ENTER("PRTident");

    printf("%s", IDENT_NAME(arg_node));

    DBUG_RETURN(arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTsymboltableentry
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *PRTsymboltableentry(node *arg_node, info *arg_info) {
    DBUG_ENTER("PRTsymboltableentry");

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

node *
PRTerror(node *arg_node, info *arg_info) {
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
*PRTdoPrint(node *syntaxtree) {
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
// @todo: Needs to handle expr
node *PRTcastexpr(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    switch (CASTEXPR_TYPE(arg_node)) {
        case T_void:
            printf("(void)");
            break;
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
    printf(CASTEXPR_EXPR(arg_node));
    DBUG_RETURN(arg_node);
}

// @todo: Needs to handle expr
node *PRTmonop(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

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
    DBUG_RETURN(arg_node);
}

node *PRTexprs(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

// @todo: Needs to handle expr
node *PRTreturnstmt(node *arg_node, info *arg_info) {
    DBUG_ENTER("");
    printf("return %s\n;", "Expr");
    DBUG_RETURN(arg_node);
}

// @todo: Needs to handle expr
node *PRTforstmt(node *arg_node, info *arg_info) {
    DBUG_ENTER("");
    printf("FOR STMT");
    //printf("for (%s = %s, %s, %s) {\n %s;\n }", "ASSIGN_VAR", "ASSIGN_EXPR",
    //       "COMPARE_EXPR", "UPDATE_EXPR", "BLOCK");
    DBUG_RETURN(arg_node);
}

node *PRTdowhilestmt(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTwhilestmt(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTifelsestmt(node *arg_node, info *arg_info) {
    DBUG_ENTER("PRTifelsestmt");

    printf("if(");
    IFELSESTMT_EXPR(arg_node) = TRAVdo(IFELSESTMT_EXPR(arg_node), arg_info);
    printf(") ");

    IFELSESTMT_IFBLOCK(arg_node) = TRAVdo(IFELSESTMT_IFBLOCK(arg_node), arg_info);

    if(IFELSESTMT_ELSEBLOCK(arg_node) != NULL) {
        printf("else ");
        IFELSESTMT_ELSEBLOCK(arg_node) = TRAVdo(IFELSESTMT_ELSEBLOCK(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

node *PRTfuncall(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTblock(node *arg_node, info *arg_info) {
    DBUG_ENTER("PRTblock");

    printf("{\n");
    BLOCK_STMTS(arg_node) = TRAVdo(BLOCK_STMTS(arg_node), arg_info);
    printf("}\n");

    DBUG_RETURN(arg_node);
}

node *PRTvardec(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTvardecs(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTfundefs(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTfunbody(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTparam(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTparams(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

/**
 * Global definition
 * @param arg_node
 * @param arg_info
 * @return
 */
node *PRTglobaldef(node *arg_node, info *arg_info) {
    char* tmp;

    DBUG_ENTER("PRTglobaldef");
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

    GLOBALDEF_IDENT(arg_node) = TRAVdo(GLOBALDEF_IDENT(arg_node), arg_info);


    if (GLOBALDEF_EXPR(arg_node) != NULL) {
        printf(" = ");
        GLOBALDEF_EXPR(arg_node) = TRAVdo(GLOBALDEF_EXPR(arg_node), arg_info);
    }

    printf(";");
    DBUG_RETURN(arg_node);
}


node *PRTglobaldec(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTfunheader(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTfundef(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}

node *PRTdeclarations(node *arg_node, info *arg_info) {
    DBUG_ENTER("PRTdeclarations");


    DECLARATIONS_DECLARATION(arg_node) = TRAVdo(DECLARATIONS_DECLARATION(arg_node), arg_info);
    printf("\n");
    if(DECLARATIONS_NEXT(arg_node) != NULL) {
        DECLARATIONS_NEXT(arg_node) = TRAVdo(DECLARATIONS_NEXT(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

node *PRTprogram(node *arg_node, info *arg_info) {
    DBUG_ENTER("");

    DBUG_RETURN(arg_node);
}



/**
 * @}
 */
