#ifndef _ST_OPR_H_
#define _ST_OPR_H_

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

#include "memory.h"
#include "ctinfo.h"
#include <string.h>


/*
 * INFO structure
 */

struct INFO {
    node* table_stack[32];
    int index;
    char* forloop_vars[32];
    int forloopIndex;
};

typedef struct INFO symboltables;



/*
 * SYMBOLTABLES macros
 */

#define SYMBOLTABLES_INDEX(n)  ((n)->index)
#define SYMBOLTABLES_GET_TABLE(n, i) ((n)->table_stack[i])
#define SYMBOLTABLES_GLOBAL_TABLE(n) ((n)->table_stack[0])
#define SYMBOLTABLES_TABLE_STACK(n) ((n)->table_stack)
#define SYMBOLTABLES_CURRENT_TABLE(n) ((n)->table_stack[(n)->index])
#define SYMBOLTABLES_ADD_TABLE(n, symboltable) ((n)->table_stack[++(n)->index] = symboltable)
#define SYMBOLTABLES_REMOVE_TABLE(n) ((n)->table_stack[(n)->index--] = NULL)

#define SYMBOLTABLES_FORLOOPS(n) ((n)->forloop_vars)
#define SYMBOLTABLES_FORLOOP_C(n) ((n)->forloop_vars[(n)->forloopIndex])
#define SYMBOLTABLES_FORLOOP(n, i) ((n)->forloop_vars[i])
#define SYMBOLTABLES_FORLOOP_INDEX(n) ((n)->forloopIndex)

symboltables* MakeSymboltables(void);
symboltables* FreeSymboltables(symboltables *tables);

bool strInArray(char* string, char* stringArr[], int lastIndex);

// @todo Not sure about cctype here
node* addSymbolTableEntry(node* symbol_table, char* name, cctype type, bool is_array, int scope);
node* searchSymbolTable(node* symbol_table, char* name);
node* searchSymbolTables(symboltables* tables, char* name, int* scope);


#define true 1
#define false 0

#endif