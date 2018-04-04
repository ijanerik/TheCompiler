#include "gen_global_table.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"


struct INFO {
  int index [32];
  int scope;
};

#define INFO_NEW_INDEX(n)  ((n)->index[(n)->scope]++)
#define INFO_INC_SCOPE(n)  ((n)->scope++)
#define INFO_DEC_SCOPE(n)  ((n)->scope--)

static info *MakeInfo(void)
{
  info *result;

  DBUG_ENTER( "MakeInfo");

  result = (info *)MEMmalloc(sizeof(info));

  result->scope = 0;

  for (int i = 0; i < 32; i++) {
      result->index[i] = 0;
  }

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
    FUNDEF_FUNBODY(arg_node) = TRAVdo(FUNDEF_FUNBODY(arg_node), arg_info);
    INFO_DEC_SCOPE(arg_info);

    DBUG_RETURN(arg_node);
}

node *GGTdoGenGlobalTable( node *syntaxtree)
{
    DBUG_ENTER("GGTdoGenGlobalTable");

    info *arg_info;

    arg_info = MakeInfo();

    TRAVpush( TR_ggt);
    printf("YEAH, STARTING TRAV");
    syntaxtree = TRAVdo( syntaxtree, arg_info);
    TRAVpop();

    arg_info = FreeInfo( arg_info);

    DBUG_RETURN( syntaxtree);
}
