#include "constants_table.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"

node* findConstant(cctype type, void* value, node* table) {
    while (table != NULL) {
        bool b = CONSTANTSTABLE_BOOL(table);
        int i = CONSTANTSTABLE_INT(table);
        float f = CONSTANTSTABLE_FLOAT(table);
        int index = CONSTANTSTABLE_INDEX(table);
        
        if ((type == T_bool && *((bool*)value) == b) ||
            (type == T_int && *((int*)value) == i) || 
            (type == T_float && *((float*)value) == f)) {
                return table;
        }
        
        table = CONSTANTSTABLE_NEXT(table);
    }
    return NULL;
}