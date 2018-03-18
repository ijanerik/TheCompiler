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

void throwError(char *msg) {
    printf("ERROR: %s\n", msg);
}

#define ERROR_RE_DEC_VAR "Variable %s is already declared"
#define ERROR_UN_DEC_VAR "Variable %s is not declared"


/*
 * Traversal functions
 */
 //@todo remove all symboltables
node *CANprogram(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANprogram");

    printf("PROGRAM Stack: %i\n", SYMBOLTABLES_INDEX(tables));

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

    printf("VAR DEC: %s \n", IDENT_NAME(ident));
    printf("Stack: %i\n", SYMBOLTABLES_INDEX(tables));

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
    else {
        CTIerror(ERROR_RE_DEC_VAR, IDENT_NAME(ident));
    }
    DBUG_RETURN( arg_node);
}

node *CANglobaldec(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANglobaldec");
    node* ident = GLOBALDEC_IDENT(arg_node);
    //node* entry;
    bool is_array = GLOBALDEC_ARRAYLENGTH(arg_node) != NULL;

    printf("GLOBAL DEC: %s", IDENT_NAME(ident));
    if(is_array) {
        printf("[]");
    }
    printf(" \n");
    printf("Stack: %i\n", SYMBOLTABLES_INDEX(tables));

    if(is_array) {
        GLOBALDEC_ARRAYLENGTH(arg_node) = TRAVdo(GLOBALDEC_ARRAYLENGTH(arg_node), tables);
    }


    // Check symbol table
    /*
    if (!searchSymbolTable(SYMBOLTABLES_GLOBAL_TABLE(tables), IDENT_NAME(ident), &entry, is_array)) {
        addSymbolTableEntry(SYMBOLTABLES_GLOBAL_TABLE(tables),
                            IDENT_NAME(ident), GLOBALDEC_TYPE(arg_node));
    }
    else {
        throwError(ERROR_RE_DEC_VAR);
        throwError(ERROR_RE_DEC_VAR);
    }
     */

    DBUG_RETURN( arg_node);
}

node *CANglobaldef(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANglobaldef");
    node* ident = GLOBALDEF_IDENT(arg_node);
    
    
    //node* entry;

    printf("GLOBAL DEF: %s", IDENT_NAME(ident));
    if(GLOBALDEF_ARRAYLENGTH(arg_node) != NULL) {
        printf("[]");
    }
    printf("\n");
    printf("Stack: %i\n", SYMBOLTABLES_INDEX(tables));

    if(GLOBALDEF_ARRAYLENGTH(arg_node) != NULL) {
        GLOBALDEF_ARRAYLENGTH(arg_node) = TRAVdo(GLOBALDEF_ARRAYLENGTH(arg_node), tables);
    }

    if(GLOBALDEF_EXPRS(arg_node) != NULL) {
        GLOBALDEF_EXPRS(arg_node) = TRAVdo(GLOBALDEF_EXPRS(arg_node), tables);
    }

    addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables), IDENT_NAME(GLOBALDEF_IDENT(arg_node)), GLOBALDEF_TYPE(arg_node), FALSE);


    /*
    if (searchSymbolTable(SYMBOLTABLES_GLOBAL_TABLE(tables), IDENT_NAME(ident), &entry)) {
        addSymbolTableEntry(SYMBOLTABLES_GLOBAL_TABLE(tables),
                            IDENT_NAME(ident), GLOBALDEF_TYPE(arg_node));
    }
    else {
        // @todo not sure if this is the correct error
        throwError(ERROR_RE_DEC_VAR);
    }

     */
    DBUG_RETURN( arg_node);
}

node *CANfundef(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANfundef");
    node* ident = FUNHEADER_IDENT(FUNDEF_FUNHEADER(arg_node));
    //node* entry;

    if(FUNDEF_SYMBOLTABLE(arg_node) == NULL) {
       FUNDEF_SYMBOLTABLE(arg_node) = TBmakeSymboltable(NULL, NULL);
    }
    SYMBOLTABLES_ADD_TABLE(tables, FUNDEF_SYMBOLTABLE(arg_node));

    printf("FUNCTION DEF: %s -------- \n", IDENT_NAME(ident));
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
    printf("FUNCTION BODY\n");

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

    printf("VARIABLE CALL: %s \n", IDENT_NAME(ident));
    printf("Stack: %i\n", SYMBOLTABLES_INDEX(tables));

    int scope = SYMBOLTABLES_INDEX(tables);
    if (!searchSymbolTables(tables, IDENT_NAME(ident), NULL, &scope)) {
        CTIerror(ERROR_UN_DEC_VAR, IDENT_NAME(ident));
    }
    DBUG_RETURN( arg_node);
}

node *CANparam(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANparam");
    node* ident = PARAM_IDENT(arg_node);
    bool is_array = PARAM_ARRAYLENGTH(arg_node) != NULL;

    printf("PARAM DEF: %s \n", IDENT_NAME(ident));

    if(PARAM_ARRAYLENGTH(arg_node) != NULL) {
        PARAM_ARRAYLENGTH(arg_node) = TRAVdo(PARAM_ARRAYLENGTH(arg_node), tables);
    }
    
    int scope = SYMBOLTABLES_INDEX(tables);
    if (!searchSymbolTables(tables, IDENT_NAME(ident), NULL, &scope)) {
        addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables),
                            IDENT_NAME(ident),
                            PARAM_TYPE(arg_node), is_array);
    }
    else {
        CTIerror(ERROR_RE_DEC_VAR, IDENT_NAME(ident));
    }
    
    DBUG_RETURN( arg_node);
}

node *CANarrayindex(node *arg_node, symboltables *tables) {
    DBUG_ENTER("CANarrayindex");
    node* ident = ARRAYINDEX_IDENT(arg_node);
    //node* entry;

    printf("ARRAY INDEX: %s[] \n", IDENT_NAME(ident));

    if(ARRAYINDEX_INDEX(arg_node) != NULL) {
        ARRAYINDEX_INDEX(arg_node) = TRAVdo(ARRAYINDEX_INDEX(arg_node), tables);
    }

    /*
    if (searchSymbolTable(SYMBOLTABLES_GLOBAL_TABLE(tables), IDENT_NAME(ident), &entry)) {
        addSymbolTableEntry(SYMBOLTABLES_GLOBAL_TABLE(tables),
                            IDENT_NAME(ident), GLOBALDEF_TYPE(arg_node));
    }
    else {
        // @todo not sure if this is the correct error
        throwError(ERROR_RE_DEC_VAR);
    }

     */
    DBUG_RETURN( arg_node);
}

node *CANfuncall(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CANfuncall");
    node* ident = FUNCALL_IDENT(arg_node);
    //node* entry;

    printf("FUNCTION CALL: %s \n", IDENT_NAME(ident));
    if(FUNCALL_ARGS(arg_node) != NULL) {
        FUNCALL_ARGS(arg_node) = TRAVdo(FUNCALL_ARGS(arg_node), tables);
    }

    /*
    if (searchSymbolTable(SYMBOLTABLES_GLOBAL_TABLE(tables), IDENT_NAME(ident), &entry)) {
        addSymbolTableEntry(SYMBOLTABLES_GLOBAL_TABLE(tables),
                            IDENT_NAME(ident), GLOBALDEF_TYPE(arg_node));
    }
    else {
        // @todo not sure if this is the correct error
        throwError(ERROR_RE_DEC_VAR);
    }

     */
    DBUG_RETURN( arg_node);
}


node *CANassign(node *arg_node, symboltables *tables) {
    DBUG_ENTER("CANassign");
    node* ident = ASSIGN_LET(arg_node);
    //node* entry;

    printf("ASSIGN: %s \n", IDENT_NAME(ident));

    ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), tables);

    if(ASSIGN_INDEX(arg_node) != NULL) {
        ASSIGN_INDEX(arg_node) = TRAVdo(ASSIGN_INDEX(arg_node), tables);
    }

    /*
    if (searchSymbolTables(tables, IDENT_NAME(ident), &entry)) {
        IDENT_DECL(ident) = entry;
    } else {
        throwError(ERROR_UN_DEC_VAR);
    }
     */

    DBUG_RETURN( arg_node);
}

node *CANforstmt(node *arg_node, symboltables *tables) {
    DBUG_ENTER("CANforstmt");
    node* ident = FORSTMT_ASSIGNVAR(arg_node);
    //node* entry;

    printf("LOOP WITH ASSIGN VAR: %s \n", IDENT_NAME(ident));
    FORSTMT_ASSIGNEXPR(arg_node) = TRAVdo(FORSTMT_ASSIGNEXPR(arg_node), tables);

    FORSTMT_COMPAREEXPR(arg_node) = TRAVdo(FORSTMT_COMPAREEXPR(arg_node), tables);

    if(FORSTMT_UPDATEEXPR(arg_node) != NULL) {
        FORSTMT_UPDATEEXPR(arg_node) = TRAVdo(FORSTMT_UPDATEEXPR(arg_node), tables);
    }

    FORSTMT_BLOCK(arg_node) = TRAVdo(FORSTMT_BLOCK(arg_node), tables);

    /*
    if (searchSymbolTables(tables, IDENT_NAME(ident), &entry)) {
        IDENT_DECL(ident) = entry;
    } else {
        throwError(ERROR_UN_DEC_VAR);
    }
     */

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