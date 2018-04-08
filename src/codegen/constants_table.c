#include "constants_table.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"

node* findConstant(cctype type, void* value, node* table) {
    while (table != NULL) {
        
        cctype table_type = CONSTANTSTABLE_TYPE(table);
        if (type == T_bool && table_type == T_bool) {
            bool table_value = CONSTANTSTABLE_BOOL(table);
            if (*((bool*)value) == table_value) {
                return table;
            }
        }
        if (type == T_int && table_type == T_int) {
            int table_value = CONSTANTSTABLE_INT(table);
            if (*((int*)value) == table_value) {
                return table;
            }
        }

        if(type == T_float && table_type == T_float) {
            float table_value = CONSTANTSTABLE_FLOAT(table);
            if(*((float*)value) == table_value) {
                return table;
            }
        }
        
        table = CONSTANTSTABLE_NEXT(table);
    }
    return NULL;
}

