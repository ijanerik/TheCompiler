// @todo Not sure about cctype here
bool addSymbolTableEntry(node* symbol_table, char* name, cctype type, bool is_array) {
    if (NODE_TYPE (symbol_table) != N_symboltable) {
        // @todo terminate
        return 0;
    }

    while (SYMBOLTABLE_NEXT(symbol_table) != NULL) {
        symbol_table = SYMBOLTABLE_NEXT(symbol_table);
    }
    // @todo Not sure if this works, node_basic.c as guideline

    // Allocate a new symboltable node
    /*
    node* table = MakeEmptyNode ();
    NODE_TYPE (table) = N_symboltable;
    table->sons.N_symboltable = MEMmalloc (sizeof (struct SONS_N_SYMBOLTABLE));

    // Allocate a new symboltableentry node
    node* entry = MakeEmptyNode ();
    NODE_TYPE (entry) = N_symboltableentry;
    table->sons.N_symboltableentry = MEMmalloc (sizeof (struct SONS_N_SYMBOLTABLEENTRY));

    // Add a new entry to the symbol table
    SYMBOLTABLE_NEXT(symbol_table) = table;
    SYMBOLTABLE_SYMBOLTABLEENTRY(table) = entry;
    SYMBOLTABLEENTRY_NAME(entry) = name;
    SYMBOLTABLEENTRY_TYPE(entry) = type;
     */

    return 1;
}

bool searchSymbolTable(node* symbol_table, char* name, node** symbol_entry) {
    /* @todo Work your magic jan erik */
    if (symbol_entry != NULL) {
        // *symbol_entry = ... ;
    }

    return 0;
}


// @todo info get table boundary checking
bool searchSymbolTables(info* arg_info, char* name, node** symbol_entry) {
    for (int i = 0; i <= INFO_INDEX(arg_info); i++) {
        if (searchSymbolTable(INFO_GET_TABLE(arg_info, i), name, symbol_entry)){
            return 1;
        }
    }
    return 0;
}