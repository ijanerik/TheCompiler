#ifndef _GEN_GLOBAL_TABLE_H_
#define _GEN_GLOBAL_TABLE_H_
#include "types.h"

extern node* GGTdoGenGlobalTable( node *syntaxtree);
extern node* GGTvardec(node* arg_node, info *arg_info);
extern node* GGTglobaldec(node* arg_node, info *arg_info);
extern node* GGTglobaldef(node* arg_node, info *arg_info);
extern node* GGTparam(node* arg_node, info *arg_info);
extern node* GGTfundef(node* arg_node, info *arg_info);

extern node* GGTnum(node* arg_node, info *arg_info);
extern node* GGTfloat(node* arg_node, info *arg_info);
extern node* GGTbool(node* arg_node, info *arg_info);
#endif
