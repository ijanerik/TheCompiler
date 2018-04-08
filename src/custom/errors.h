#define ERROR_TYPE_GLOBDEF  "line: %d\tVariable %s of type %s does not match inferred type %s"
#define ERROR_TYPE_BINOP    "line: %d\tType %s does not match type %s in the expression"
#define ERROR_TYPE_CAST     "line: %d\tType %s cannot be cast to %s"

#define ERROR_TYPE_MONOP    "line: %d\tOperator does not support this type"

#define ERROR_REDEC_VAR "line: %d\tVariable %s is already declared"
#define ERROR_UNDEC_VAR "line: %d\tVariable %s is not declared"
#define WARNING_SHADOW_VAR "line: %d\tVariable %s is shadowed"

#define ERROR_PAREM_TYPE "line: %d\tArgument of type %s does not match parameter of type %s"
#define ERROR_RET_TYPE "line: %d\tType %s of returned value does not match type %s of its function"

#define ERROR_OP_NOT_SUP "line: %d\tOperator not supported for type %s."

#define ERROR_INCORRECT_FUNCTION_TABLE "Incorrect function table given"
#define ERROR_REDEC_FUNC "line: %d\tFunction \"%s\" is already declared"
#define ERROR_UNDEC_FUNC "line: %d\tFunction \"%s\" is not declared"