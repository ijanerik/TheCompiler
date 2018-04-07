#include "util.h"

char* cctypeToString(cctype type) {
    char* str;
    switch (type) {
        case T_float:
            str = "float";
            break;
        case T_int:
            str = "int";
            break;
        case T_bool:
            str = "bool";
            break;
        case T_void:
            str = "void";
            break;
        default:
            str = "unknown";
    }
    return str;
}
