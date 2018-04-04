#include "gen_global_table.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"


struct INFO {
  int index [32];
  int scope;

  node* constants_table_head;
  node* constants_table_tail;

  int constants_index;
};

#define INFO_NEW_INDEX(n)  ((n)->index[(n)->scope]++)
#define INFO_INC_SCOPE(n)  ((n)->scope++)
#define INFO_DEC_SCOPE(n)  ((n)->scope--)

#define INFO_CONSTANTS_TABLE_HEAD(n) ((n)->constants_table_head)
#define INFO_CONSTANTS_TABLE_TAIL(n) ((n)->constants_table_tail)
#define INFO_NEW_CONSTANTS_INDEX(n)  ((n)->constants_index++)
#define INFO_CONSTANTS_INDEX(n)  ((n)->constants_index)

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

static info *MakeInfo(void)
{
  info *result;

  DBUG_ENTER( "MakeInfo");

  result = (info *)MEMmalloc(sizeof(info));

  result->scope = 0;

  for (int i = 0; i < 32; i++) {
      result->index[i] = 0;
  }
  result->constants_index = 0;

  DBUG_RETURN( result);
}

static info *FreeInfo( info *info)
{
  DBUG_ENTER ("FreeInfo");

  info = MEMfree( info);

  DBUG_RETURN( info);
}

node *GGTvardec(node* arg_node, info *arg_info) {
    DBUG_ENTER("GGTvardec");
    node* entry = VARDEC_SYMBOLTABLEENTRY(arg_node);
    SYMBOLTABLEENTRY_INDEX(entry) = INFO_NEW_INDEX(arg_info);
    
    if (VARDEC_EXPRS(arg_node)) {
        VARDEC_EXPRS(arg_node) = TRAVdo(VARDEC_EXPRS(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

node *GGTglobaldec(node* arg_node, info *arg_info) {
    DBUG_ENTER("GGTglobaldec");

    node* entry = GLOBALDEC_SYMBOLTABLEENTRY(arg_node);
    SYMBOLTABLEENTRY_INDEX(entry) = INFO_NEW_INDEX(arg_info);
    
    DBUG_RETURN(arg_node);
}

node *GGTglobaldef(node* arg_node, info *arg_info) {
    DBUG_ENTER("GGTglobaldef");

    node* entry = GLOBALDEF_SYMBOLTABLEENTRY(arg_node);
    SYMBOLTABLEENTRY_INDEX(entry) = INFO_NEW_INDEX(arg_info);

    if (GLOBALDEF_EXPRS(arg_node)) {
        GLOBALDEF_EXPRS(arg_node) = TRAVdo(GLOBALDEF_EXPRS(arg_node), arg_info);
    }
    
    DBUG_RETURN(arg_node);
}

node* GGTparam(node* arg_node, info *arg_info) {
    DBUG_ENTER("GGTparam");

    node* entry = PARAM_SYMBOLTABLEENTRY(arg_node);
    SYMBOLTABLEENTRY_INDEX(entry) = INFO_NEW_INDEX(arg_info);

    DBUG_RETURN(arg_node);
}

node* GGTfundef(node* arg_node, info *arg_info)  {
    DBUG_ENTER("GGTfundef");

    INFO_INC_SCOPE(arg_info);
    FUNDEF_FUNHEADER(arg_node) = TRAVdo(FUNDEF_FUNHEADER(arg_node), arg_info);
    
    if (FUNDEF_FUNBODY(arg_node)) {
        FUNDEF_FUNBODY(arg_node) = TRAVdo(FUNDEF_FUNBODY(arg_node), arg_info);
    }
    INFO_DEC_SCOPE(arg_info);

    DBUG_RETURN(arg_node);
}

node* GGTnum(node* arg_node, info *arg_info) {
    DBUG_ENTER("GGTnum");

    node* table_head = INFO_CONSTANTS_TABLE_HEAD(arg_info);
    node* table_tail = INFO_CONSTANTS_TABLE_TAIL(arg_info);

    int value = NUM_VALUE(arg_node);
    node* table = findConstant(T_int, (void *)&value, table_head);

    if (table == NULL) {
        int new_index = INFO_NEW_CONSTANTS_INDEX(arg_info);
        if (CONSTANTSTABLE_INDEX(table_head) == -1) {
            CONSTANTSTABLE_INDEX(table_head) = new_index;
            CONSTANTSTABLE_TYPE(table_head) = T_int;
            CONSTANTSTABLE_INT(table_head) = value;
            table = table_head;
            //printf("FIRST ONE, index: %d value: %d p:%p\n", new_index, value, table_head);
        }
        else {
            node* new_table = TBmakeConstantstable(T_int, new_index, FALSE,
                                                   value, 0, NULL);
            CONSTANTSTABLE_NEXT(table_tail) = new_table;
            //printf("Added new table for int: %d, index: %d\t%p->%p\n", value, new_index, table_tail,new_table);
            INFO_CONSTANTS_TABLE_TAIL(arg_info) = new_table;
            table = new_table;
        }
    }
    NUM_CONSTANTSTABLE(arg_node) = table;

    DBUG_RETURN(arg_node);
} 

node* GGTfloat(node* arg_node, info *arg_info) {
    DBUG_ENTER("GGTfloat");

    node* table_head = INFO_CONSTANTS_TABLE_HEAD(arg_info);
    node* table_tail = INFO_CONSTANTS_TABLE_TAIL(arg_info);

    float value = FLOAT_VALUE(arg_node);
    node* table = findConstant(T_int, (void *)&value, table_head);
    
    if (table == NULL) {
        int new_index = INFO_NEW_CONSTANTS_INDEX(arg_info);
        if (CONSTANTSTABLE_INDEX(table_head) == -1) {
            CONSTANTSTABLE_INDEX(table_head) = new_index;
            CONSTANTSTABLE_TYPE(table_head) = T_float;
            CONSTANTSTABLE_FLOAT(table_head) = value;
            table = table_head;
        }
        else {
            node* new_table = TBmakeConstantstable(T_float, new_index, FALSE,
                                                   0, value, NULL);
            CONSTANTSTABLE_NEXT(table_tail) = new_table;
            INFO_CONSTANTS_TABLE_TAIL(arg_info) = new_table;
            table = new_table;
            //printf("Added new table for float: %f\n", value);
        }
    }
    FLOAT_CONSTANTSTABLE(arg_node) = table;
    
    DBUG_RETURN(arg_node);    
} 



node* GGTbool(node* arg_node, info *arg_info) {
    DBUG_ENTER("GGTbool");
       
    node* table_head = INFO_CONSTANTS_TABLE_HEAD(arg_info);
    node* table_tail = INFO_CONSTANTS_TABLE_TAIL(arg_info);

    bool value = BOOL_VALUE(arg_node);
    node* table = findConstant(T_int, (void *)&value, table_head);
    
    if (table == NULL) {
        int new_index = INFO_NEW_CONSTANTS_INDEX(arg_info);
        if (CONSTANTSTABLE_INDEX(table_head) == -1) {
            CONSTANTSTABLE_INDEX(table_head) = new_index,
            CONSTANTSTABLE_TYPE(table_head) = T_bool;
            CONSTANTSTABLE_BOOL(table_head) = value;
            table = table_head;
        }
        else {
            node* new_table = TBmakeConstantstable(T_bool, new_index, value,
                                                   0, 0, NULL);
            CONSTANTSTABLE_NEXT(table_tail) = new_table;
            INFO_CONSTANTS_TABLE_TAIL(arg_info) = new_table;
            table = new_table;
            //printf("Added new table for bool: %d, index: %d\n", value, new_index);
        }
    }
    BOOL_CONSTANTSTABLE(arg_node) = table;

    DBUG_RETURN(arg_node); 
} 

node *GGTdoGenGlobalTable( node *syntaxtree)
{
    DBUG_ENTER("GGTdoGenGlobalTable");

    info *arg_info;

    arg_info = MakeInfo();

    if (NODE_TYPE(syntaxtree) == N_program) {
        node* table = TBmakeConstantstable (T_unknown, -1, FALSE, 0, 0, NULL);
        INFO_CONSTANTS_TABLE_HEAD(arg_info) = table;
        INFO_CONSTANTS_TABLE_TAIL(arg_info) = table;
        PROGRAM_CONSTANTSTABLE(syntaxtree) = table;
    }

    TRAVpush( TR_ggt);
    syntaxtree = TRAVdo( syntaxtree, arg_info);
    TRAVpop();

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}
