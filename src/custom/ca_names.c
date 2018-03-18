/*****************************************************************************
 *
 * Module: ca_names
 *
 * Prefix: CAN
 *
 * Description:
 *
 *****************************************************************************/


#include "ca_names.h"

#define ERROR_REDEC_VAR "Variable %s is already declared"
#define ERROR_UNDEC_VAR "Variable %s is not declared"
#define WARNING_SHADOW_VAR "Variable %s is shadowed"


/*
 * Traversal functions
 */
 //@todo remove all symboltables
node *CANprogram(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANprogram");

    //printf("PROGRAM Stack: %i\n", SYMBOLTABLES_INDEX(tables));

    if(PROGRAM_SYMBOLTABLE(arg_node) == NULL) {
        PROGRAM_SYMBOLTABLE(arg_node) = TBmakeSymboltable(NULL, NULL);
    }

    SYMBOLTABLES_CURRENT_TABLE(tables) = PROGRAM_SYMBOLTABLE(arg_node);

    PROGRAM_DECLARATIONS(arg_node) = TRAVdo(PROGRAM_DECLARATIONS(arg_node), tables);

    DBUG_RETURN( arg_node);
}

// @todo Traverse other order by calling declarations

node *CANvardec(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANvardec");
    node* ident = VARDEC_IDENT(arg_node);

    //printf("VAR DEC: %s \n", IDENT_NAME(ident));
    //printf("Stack: %i\n", SYMBOLTABLES_INDEX(tables));

    bool is_array = VARDEC_ARRAYLENGTH(arg_node) != NULL;

    if(VARDEC_EXPRS(arg_node) != NULL) {
        VARDEC_EXPRS(arg_node) = TRAVdo(VARDEC_EXPRS(arg_node), tables);
    }

    if(is_array) {
        VARDEC_ARRAYLENGTH(arg_node) = TRAVdo(VARDEC_ARRAYLENGTH(arg_node), tables);
    }

    int scope = SYMBOLTABLES_INDEX(tables);
    if (!searchSymbolTables(tables, IDENT_NAME(ident), NULL, &scope)) {
        addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables),
                            IDENT_NAME(VARDEC_IDENT(arg_node)),
                            VARDEC_TYPE(arg_node), is_array);
    }
    else if(scope < SYMBOLTABLES_INDEX(tables)) {
        CTIwarn(WARNING_SHADOW_VAR, IDENT_NAME(ident));
    }
    else {
        CTIerror(ERROR_REDEC_VAR, IDENT_NAME(ident));
    }
    DBUG_RETURN( arg_node);
}

node *CANglobaldec(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANglobaldec");
    node* ident = GLOBALDEC_IDENT(arg_node);
    //node* entry;
    bool is_array = GLOBALDEC_ARRAYLENGTH(arg_node) != NULL;

    //printf("GLOBAL DEC: %s", IDENT_NAME(ident));
    if(is_array) {
        printf("[]");
    }

    if (!searchSymbolTables(tables, IDENT_NAME(ident), NULL, NULL)) {
        addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables),
                            IDENT_NAME(ident),
                            GLOBALDEC_TYPE(arg_node), is_array);
    }
    else {
        CTIerror(ERROR_REDEC_VAR, IDENT_NAME(ident));
    }

    //printf(" \n");
    //printf("Stack: %i\n", SYMBOLTABLES_INDEX(tables));

    if(is_array) {
        GLOBALDEC_ARRAYLENGTH(arg_node) = TRAVdo(GLOBALDEC_ARRAYLENGTH(arg_node), tables);
    }

    DBUG_RETURN( arg_node);
}

node *CANglobaldef(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANglobaldef");
    node* ident = GLOBALDEF_IDENT(arg_node);
    bool is_array = GLOBALDEF_ARRAYLENGTH(arg_node) != NULL;
    
    //printf("Stack: %i\n", SYMBOLTABLES_INDEX(tables));

    if(is_array) {
        GLOBALDEF_ARRAYLENGTH(arg_node) = TRAVdo(GLOBALDEF_ARRAYLENGTH(arg_node), tables);
    }

    if(GLOBALDEF_EXPRS(arg_node) != NULL) {
        GLOBALDEF_EXPRS(arg_node) = TRAVdo(GLOBALDEF_EXPRS(arg_node), tables);
    }

    if (!searchSymbolTables(tables, IDENT_NAME(ident), NULL, NULL)) {
        addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables),
                            IDENT_NAME(ident),
                            GLOBALDEF_TYPE(arg_node), is_array);
    }
    else {
        CTIerror(ERROR_REDEC_VAR, IDENT_NAME(ident));
    }

    DBUG_RETURN( arg_node);
}

node *CANfundef(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANfundef");
    node* ident = FUNHEADER_IDENT(FUNDEF_FUNHEADER(arg_node));

    if(FUNDEF_SYMBOLTABLE(arg_node) == NULL) {
       FUNDEF_SYMBOLTABLE(arg_node) = TBmakeSymboltable(NULL, NULL);
    }
    SYMBOLTABLES_ADD_TABLE(tables, FUNDEF_SYMBOLTABLE(arg_node));

    //printf("FUNCTION DEF: %s -------- \n", IDENT_NAME(ident));
    FUNDEF_FUNHEADER(arg_node) = TRAVdo(FUNDEF_FUNHEADER(arg_node), tables);
    if(FUNDEF_FUNBODY(arg_node) != NULL) {
        FUNDEF_FUNBODY(arg_node) = TRAVdo(FUNDEF_FUNBODY(arg_node), tables);
    }

    SYMBOLTABLES_REMOVE_TABLE(tables);
    
    DBUG_RETURN( arg_node);
}


node* CANfunbody(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANfunbody");
    //printf("FUNCTION BODY\n");

    if(FUNBODY_VARDECS(arg_node) != NULL) {
        FUNBODY_VARDECS(arg_node) = TRAVdo(FUNBODY_VARDECS(arg_node), tables);
    }
    if(FUNBODY_FUNDEFS(arg_node) != NULL) {
        FUNBODY_FUNDEFS(arg_node) = TRAVdo(FUNBODY_FUNDEFS(arg_node), tables);
    }
    if(FUNBODY_STMTS(arg_node) != NULL) {
        FUNBODY_STMTS(arg_node) = TRAVdo(FUNBODY_STMTS(arg_node), tables);
    }

    DBUG_RETURN( arg_node);
}

node *CANvarcall(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANvarcall");
    node* ident = VARCALL_IDENT(arg_node);
    //node* entry;

    //printf("VARIABLE CALL: %s \n", IDENT_NAME(ident));
    //printf("Stack: %i\n", SYMBOLTABLES_INDEX(tables));

    if (!searchSymbolTables(tables, IDENT_NAME(ident), NULL, NULL)) {
        CTIerror(ERROR_UNDEC_VAR, IDENT_NAME(ident));
    }
    DBUG_RETURN( arg_node);
}

node *CANparam(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANparam");
    node* ident = PARAM_IDENT(arg_node);
    node* array_length = PARAM_ARRAYLENGTH(arg_node);
    bool is_array = PARAM_ARRAYLENGTH(arg_node) != NULL;

    // printf("PARAM DEF: %s \n", IDENT_NAME(ident));

    if(is_array) {
        PARAM_ARRAYLENGTH(arg_node) = TRAVdo(PARAM_ARRAYLENGTH(arg_node), tables);
        if (!searchSymbolTables(tables, IDENT_NAME(array_length), NULL, NULL)) {
            CTIerror(ERROR_UNDEC_VAR, IDENT_NAME(array_length));
        }
    }
    
    if (!searchSymbolTables(tables, IDENT_NAME(ident), NULL, NULL)) {
        addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables),
                            IDENT_NAME(ident),
                            PARAM_TYPE(arg_node), is_array);
    }
    else {
        CTIerror(ERROR_REDEC_VAR, IDENT_NAME(ident));
    }
    


    DBUG_RETURN( arg_node);
}

node *CANarrayindex(node *arg_node, symboltables *tables) {
    DBUG_ENTER("CANarrayindex");
    node* ident = ARRAYINDEX_IDENT(arg_node);
   
    if (!searchSymbolTables(tables, IDENT_NAME(ident), NULL, NULL)) {
        CTIerror(ERROR_UNDEC_VAR, IDENT_NAME(ident));
    }

    if(ARRAYINDEX_INDEX(arg_node) != NULL) {
        ARRAYINDEX_INDEX(arg_node) = TRAVdo(ARRAYINDEX_INDEX(arg_node), tables);
    }

    DBUG_RETURN( arg_node);
}

node *CANfuncall(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANfuncall");
    node* ident = FUNCALL_IDENT(arg_node);

    if(FUNCALL_ARGS(arg_node) != NULL) {
        FUNCALL_ARGS(arg_node) = TRAVdo(FUNCALL_ARGS(arg_node), tables);
    }

    DBUG_RETURN( arg_node);
}


node *CANassign(node *arg_node, symboltables *tables) {
    DBUG_ENTER("CANassign");
    node* ident = ASSIGN_LET(arg_node);
    //node* entry;

    // printf("ASSIGN: %s \n", IDENT_NAME(ident));

    ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), tables);

    if (!searchSymbolTables(tables, IDENT_NAME(ident), NULL, NULL)) {
        CTIerror(ERROR_UNDEC_VAR, IDENT_NAME(ident));
    }

    if(ASSIGN_INDEX(arg_node) != NULL) {
        ASSIGN_INDEX(arg_node) = TRAVdo(ASSIGN_INDEX(arg_node), tables);
    }


    DBUG_RETURN( arg_node);
}

node *CANforstmt(node *arg_node, symboltables *tables) {
    DBUG_ENTER("CANforstmt");
    node* ident = FORSTMT_ASSIGNVAR(arg_node);

    FORSTMT_ASSIGNEXPR(arg_node) = TRAVdo(FORSTMT_ASSIGNEXPR(arg_node), tables);

    FORSTMT_COMPAREEXPR(arg_node) = TRAVdo(FORSTMT_COMPAREEXPR(arg_node), tables);

    if(FORSTMT_UPDATEEXPR(arg_node) != NULL) {
        FORSTMT_UPDATEEXPR(arg_node) = TRAVdo(FORSTMT_UPDATEEXPR(arg_node), tables);
    }

    FORSTMT_BLOCK(arg_node) = TRAVdo(FORSTMT_BLOCK(arg_node), tables);

    DBUG_RETURN( arg_node);
}

// @todo print symbol table

/*
 * Traversal start function
 */

node *CANdoNames( node *syntaxtree)
{
    symboltables *tables;

    DBUG_ENTER("CANdoNames");

    tables = MakeSymboltables();

    TRAVpush(TR_can);
    syntaxtree = TRAVdo( syntaxtree, tables);
    TRAVpop();

    tables = FreeSymboltables(tables);

    DBUG_RETURN( syntaxtree);
}