#include "st_utils.h"

/*
 * SYMBOLTABLE functions
 */

symboltables* MakeSymboltables(void)
{
    symboltables *tables;

    DBUG_ENTER( "MakeInfo");

    tables = (symboltables *)MEMmalloc(sizeof(symboltables));

    SYMBOLTABLES_INDEX(tables) = 0;

    DBUG_RETURN( tables);
}

symboltables* FreeSymboltables( symboltables *tables)
{
    DBUG_ENTER ("SymboltablesInfo");

    tables = MEMfree( tables);

    DBUG_RETURN( tables);
}


// @todo Not sure about cctype here
node* addSymbolTableEntry(node* symbol_table, char* name, cctype type, bool is_array, int scope) {

    while (SYMBOLTABLE_NEXT(symbol_table) != NULL) {
        symbol_table = SYMBOLTABLE_NEXT(symbol_table);
    }

    // Add a new entry to the symbol table
    SYMBOLTABLE_NEXT(symbol_table) = TBmakeSymboltable(NULL, NULL);
    node* entry = TBmakeSymboltableentry(name, type, is_array, -1, scope);
    SYMBOLTABLE_SYMBOLTABLEENTRY(SYMBOLTABLE_NEXT(symbol_table)) = entry;
    return entry;
}

node* searchSymbolTable(node* symbol_table, char* name) {
    node* entry;
    while (symbol_table != NULL)
    {
        entry = SYMBOLTABLE_SYMBOLTABLEENTRY(symbol_table);
        if (entry != NULL && strcmp(SYMBOLTABLEENTRY_NAME(entry), name) == 0) {
            return entry;
        }
        symbol_table = SYMBOLTABLE_NEXT(symbol_table);
    }
    return NULL;
}

// @todo info get table boundary checking
node* searchSymbolTables(symboltables* tables, char* name, int* scope) {
    node* entry;
    for (int i = 0; i <= SYMBOLTABLES_INDEX(tables); i++) {
        entry = searchSymbolTable(SYMBOLTABLES_GET_TABLE(tables, i), name);
        if (entry != NULL){
            if (scope != NULL) {
                *scope = i; 
            }
            return entry;
        }
    }
    return NULL;
}