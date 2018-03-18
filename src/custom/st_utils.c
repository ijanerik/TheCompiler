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
bool addSymbolTableEntry(node* symbol_table, char* name, cctype type, bool is_array) {

    while (SYMBOLTABLE_NEXT(symbol_table) != NULL) {
        symbol_table = SYMBOLTABLE_NEXT(symbol_table);
    }

    // Add a new entry to the symbol table
    SYMBOLTABLE_NEXT(symbol_table) = TBmakeSymboltable(NULL, NULL);
    SYMBOLTABLE_SYMBOLTABLEENTRY(SYMBOLTABLE_NEXT(symbol_table)) = TBmakeSymboltableentry(name, type, is_array);

    return 1;
}
/*  
    input:  node* symbol_table, char* name;
    output: node* symbol_entry
*/
bool searchSymbolTable(node* symbol_table, char* name, node* symbol_entry) {
    node* entry;
    while (symbol_table != NULL)
    {
        entry = SYMBOLTABLE_SYMBOLTABLEENTRY(symbol_table);
        if (entry!= NULL && strcmp(SYMBOLTABLEENTRY_NAME(entry), name) == 0) {
            symbol_entry = entry;
            return true;
        }
        symbol_table = SYMBOLTABLE_NEXT(symbol_table);
    } 
    
    return false;
}


// @todo info get table boundary checking
bool searchSymbolTables(symboltables* tables, char* name, node* symbol_entry, int* scope) {
    for (int i = 0; i <= SYMBOLTABLES_INDEX(tables); i++) {
        if (searchSymbolTable(SYMBOLTABLES_GET_TABLE(tables, i), name, symbol_entry)){
            *scope = i; 
            return true;
        }
    }
    return false;
    
}