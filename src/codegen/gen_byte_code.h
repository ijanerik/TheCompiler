#ifndef _GEN_BYTE_CODE_H_
#define _GEN_BYTE_CODE_H_
#include "types.h"

extern node *GBCdoGenByteCode( node *syntaxtree);
extern node* GBCassign(node* arg_node, info* arg_info);
extern node* GBCnum(node* arg_node, info* arg_info);
extern node* GBCvarcall(node* arg_node, info* arg_info);
extern node* GBCbinop(node* arg_node, info* arg_info);
extern node* GBCvardec(node* arg_node, info* arg_info);
extern node* GBCifelsestmt(node* arg_node, info* arg_info);
extern node* GBCwhilestmt(node* arg_node, info* arg_info);
extern node* GBCdowhilestmt(node* arg_node, info* arg_info);
extern node* GBCforstmt(node* arg_node, info* arg_info);
extern node* GBCglobaldef(node* arg_node, info* arg_info);
extern node* GBCfundef(node* arg_node, info* arg_info);
extern node* GBCconstantstable(node* arg_node, info* arg_info);
extern node* GBCprogram(node* arg_node, info* arg_info);
extern node* GBCreturnstmt(node* arg_node, info* arg_info);
extern node* GBCfuncall(node* arg_node, info* arg_info);
extern node* GBCfloat(node* arg_node, info* arg_info);
extern node* GBCbool(node* arg_node, info* arg_info);
extern node* GBCcastexpr(node* arg_node, info* arg_info);
extern node* GBCcondexpr(node* arg_node, info* arg_info);
extern node* GBCmonop(node* arg_node, info* arg_info);
#endif
