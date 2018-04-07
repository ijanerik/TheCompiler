#ifndef _GEN_TABLES_H_
#define _GEN_TABLES_H_
#include "types.h"

extern node* GTdoGenTables( node *syntaxtree);
extern node* GTvardec(node* arg_node, info *arg_info);
extern node* GTglobaldec(node* arg_node, info *arg_info);
extern node* GTglobaldef(node* arg_node, info *arg_info);
extern node* GTparam(node* arg_node, info *arg_info);
extern node* GTfundef(node* arg_node, info *arg_info);
extern node* GTforstmt(node* arg_node, info *arg_info);

extern node* GTnum(node* arg_node, info *arg_info);
extern node* GTfloat(node* arg_node, info *arg_info);
extern node* GTbool(node* arg_node, info *arg_info);
#endif
