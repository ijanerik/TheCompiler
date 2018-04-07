/*****************************************************************************
 *
 * Module: ca_variables
 *
 * Prefix: CAV
 *
 * Description:
 *
 *****************************************************************************/


#include "ca_variables.h"
#include "errors.h"

/*
 * Traversal functions
 */
 //@todo remove all symboltables
node *CAVprogram(node *arg_node, symboltables *tables)
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

node *CAVvardec(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CAVvardec");
    node* ident = VARDEC_IDENT(arg_node);
    int scope = SYMBOLTABLES_INDEX(tables);
    node* entry = searchSymbolTables(tables, IDENT_NAME(ident), &scope);
    
    //printf("VAR DEC: %s \n", IDENT_NAME(ident));
    //printf("Stack: %i\n", SYMBOLTABLES_INDEX(tables));

    bool is_array = VARDEC_ARRAYLENGTH(arg_node) != NULL;

    if(VARDEC_EXPRS(arg_node) != NULL) {
        VARDEC_EXPRS(arg_node) = TRAVdo(VARDEC_EXPRS(arg_node), tables);
    }

    if(is_array) {
        VARDEC_ARRAYLENGTH(arg_node) = TRAVdo(VARDEC_ARRAYLENGTH(arg_node), tables);
    }

    
    if (entry == NULL) {
        entry = addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables),
                                    IDENT_NAME(VARDEC_IDENT(arg_node)),
                                    VARDEC_TYPE(arg_node), is_array,
                                    SYMBOLTABLES_INDEX(tables));
        VARDEC_SYMBOLTABLEENTRY(arg_node) = entry;
    }
    else if(scope < SYMBOLTABLES_INDEX(tables)) {
        CTIwarn(WARNING_SHADOW_VAR, IDENT_NAME(ident));
    }
    else {
        CTIerror(ERROR_REDEC_VAR, IDENT_NAME(ident));
    }
    DBUG_RETURN( arg_node);
}

node *CAVglobaldec(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CAVglobaldec");
    
    node* ident = GLOBALDEC_IDENT(arg_node);
    node* entry = searchSymbolTables(tables, IDENT_NAME(ident), NULL);
    bool is_array = GLOBALDEC_ARRAYLENGTH(arg_node) != NULL;

    if (entry == NULL) {
        entry = addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables),
                                    IDENT_NAME(ident),
                                    GLOBALDEC_TYPE(arg_node), is_array,
                                    SYMBOLTABLES_INDEX(tables));
        GLOBALDEC_SYMBOLTABLEENTRY(arg_node) = entry;
    }
    else {
        CTIerror(ERROR_REDEC_VAR, IDENT_NAME(ident));
    }

    if(is_array) {
        GLOBALDEC_ARRAYLENGTH(arg_node) = TRAVdo(GLOBALDEC_ARRAYLENGTH(arg_node), tables);
    }

    DBUG_RETURN( arg_node);
}

node *CAVglobaldef(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CAVglobaldef");

    node* ident = GLOBALDEF_IDENT(arg_node);
    node* entry = searchSymbolTables(tables, IDENT_NAME(ident), NULL);
    bool is_array = GLOBALDEF_ARRAYLENGTH(arg_node) != NULL;
    
    //printf("Stack: %i\n", SYMBOLTABLES_INDEX(tables));

    if(is_array) {
        GLOBALDEF_ARRAYLENGTH(arg_node) = TRAVdo(GLOBALDEF_ARRAYLENGTH(arg_node), tables);
    }

    if(GLOBALDEF_EXPRS(arg_node) != NULL) {
        GLOBALDEF_EXPRS(arg_node) = TRAVdo(GLOBALDEF_EXPRS(arg_node), tables);
    }

    if (entry == NULL) {
        entry = addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables),
                                    IDENT_NAME(ident),
                                    GLOBALDEF_TYPE(arg_node), is_array,
                                    SYMBOLTABLES_INDEX(tables));
        GLOBALDEF_SYMBOLTABLEENTRY(arg_node) = entry;
    }
    else {
        CTIerror(ERROR_REDEC_VAR, IDENT_NAME(ident));
    }

    DBUG_RETURN( arg_node);
}

node *CAVfundef(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CAVfundef");

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


node* CAVfunbody(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CAVfunbody");

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

node *CAVvarcall(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CAVvarcall");

    node* ident = VARCALL_IDENT(arg_node);
    node* entry = searchSymbolTables(tables, IDENT_NAME(ident), NULL);
    
    if (entry != NULL) {
        VARCALL_SYMBOLTABLEENTRY(arg_node) = entry;
    } else {
        CTIerror(ERROR_UNDEC_VAR, IDENT_NAME(ident));
    }

    DBUG_RETURN( arg_node);
}

node *CAVparam(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CAVparam");

    node* ident = PARAM_IDENT(arg_node);
    node* array_length = PARAM_ARRAYLENGTH(arg_node);
    node* entry = searchSymbolTables(tables, IDENT_NAME(ident), NULL);
    bool is_array = PARAM_ARRAYLENGTH(arg_node) != NULL;

    // @todo arrays as parameters are weird
    if(is_array) {
        node* array_entry = searchSymbolTables(tables, IDENT_NAME(array_length), NULL);
        PARAM_ARRAYLENGTH(arg_node) = TRAVdo(PARAM_ARRAYLENGTH(arg_node), tables);
        if (!array_entry) {
            CTIerror(ERROR_UNDEC_VAR, IDENT_NAME(array_length));
        }
    }
    
    if (entry == NULL) {
        entry = addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables),
                                    IDENT_NAME(ident),
                                    PARAM_TYPE(arg_node), is_array,
                                    SYMBOLTABLES_INDEX(tables));
        PARAM_SYMBOLTABLEENTRY(arg_node) = entry;
    }
    else {
        CTIerror(ERROR_REDEC_VAR, IDENT_NAME(ident));
    }

    DBUG_RETURN( arg_node);
}

node *CAVarrayindex(node *arg_node, symboltables *tables) {
    DBUG_ENTER("CAVarrayindex");

    node* ident = ARRAYINDEX_IDENT(arg_node);
    node* entry = searchSymbolTables(tables, IDENT_NAME(ident), NULL);
   
    if (entry != NULL) {
        ARRAYINDEX_SYMBOLTABLEENTRY(arg_node) = entry;
    }
    else {
        CTIerror(ERROR_UNDEC_VAR, IDENT_NAME(ident));
    }
    ARRAYINDEX_INDEX(arg_node) = TRAVdo(ARRAYINDEX_INDEX(arg_node), tables);

    DBUG_RETURN( arg_node);
}

node *CAVfuncall(node *arg_node, symboltables *tables)
{
    DBUG_ENTER("CAVfuncall");

    if(FUNCALL_ARGS(arg_node) != NULL) {
        FUNCALL_ARGS(arg_node) = TRAVdo(FUNCALL_ARGS(arg_node), tables);
    }

    DBUG_RETURN( arg_node);
}


node *CAVassign(node *arg_node, symboltables *tables) {
    DBUG_ENTER("CAVassign");

    node* ident = ASSIGN_LET(arg_node);
    node* entry = searchSymbolTables(tables, IDENT_NAME(ident), NULL);

    ASSIGN_EXPR(arg_node) = TRAVdo(ASSIGN_EXPR(arg_node), tables);

    if (entry != NULL) {
        ASSIGN_SYMBOLTABLEENTRY(arg_node) = entry;
    } else {
        CTIerror(ERROR_UNDEC_VAR, IDENT_NAME(ident));
    }

    if(ASSIGN_INDEX(arg_node) != NULL) {
        ASSIGN_INDEX(arg_node) = TRAVdo(ASSIGN_INDEX(arg_node), tables);
    }

    DBUG_RETURN( arg_node);
}

node *CAVforstmt(node *arg_node, symboltables *tables) {
    DBUG_ENTER("CAVforstmt");


    DBUG_PRINT ("FORSTMT_CAV", ("Enter forstmt"));
    FORSTMT_ASSIGNEXPR(arg_node) = TRAVdo(FORSTMT_ASSIGNEXPR(arg_node), tables);

    if(BLOCK_SYMBOLTABLE(FORSTMT_BLOCK(arg_node)) == NULL) {
        BLOCK_SYMBOLTABLE(FORSTMT_BLOCK(arg_node)) = TBmakeSymboltable(NULL, NULL);
    }
    SYMBOLTABLES_ADD_TABLE(tables, BLOCK_SYMBOLTABLE(FORSTMT_BLOCK(arg_node)));

    FORSTMT_SYMBOLTABLEENTRY(arg_node) = addSymbolTableEntry(SYMBOLTABLES_CURRENT_TABLE(tables),
                                                            IDENT_NAME(FORSTMT_ASSIGNVAR(arg_node)),
                                                             T_int, 0,
                                                             SYMBOLTABLES_INDEX(tables));


    FORSTMT_COMPAREEXPR(arg_node) = TRAVdo(FORSTMT_COMPAREEXPR(arg_node), tables);

    if(FORSTMT_UPDATEEXPR(arg_node) != NULL) {
        FORSTMT_UPDATEEXPR(arg_node) = TRAVdo(FORSTMT_UPDATEEXPR(arg_node), tables);
    }

    FORSTMT_BLOCK(arg_node) = TRAVdo(FORSTMT_BLOCK(arg_node), tables);

    SYMBOLTABLES_REMOVE_TABLE(tables);

    DBUG_RETURN( arg_node);
}


/*
 * Traversal start function
 */

node *CAVdoVariables( node *syntaxtree)
{
    symboltables *tables;

    DBUG_ENTER("CAVdoVariables");

    tables = MakeSymboltables();

    TRAVpush(TR_cav);
    syntaxtree = TRAVdo( syntaxtree, tables);
    TRAVpop();

    tables = FreeSymboltables(tables);

    DBUG_RETURN( syntaxtree);
}