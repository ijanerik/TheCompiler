#ifndef _ST_OPR_H_
#define _ST_OPR_H_

// @todo Not sure about cctype here
bool addSymbolTableEntry(node* symbol_table, char* name, cctype type, bool is_array);
bool searchSymbolTable(node* symbol_table, char* name, node** symbol_entry);


// @todo info get table boundary checking
bool searchSymbolTables(node* table_stack[n], int *index, char* name, node** symbol_entry, int *symboltableIndex);

#endif