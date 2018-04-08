/* LOAD INSTRUCTIONS */
char* ILOAD = "iload";
char* ILOADN = "iloadn";
char* ILOADC = "iloadc";
char* ISTORE = "istore";

char* FLOAD = "fload";
char* FLOADN = "floadn";
char* FLOADC = "floadc";
char* FSTORE = "fstore";

char* BLOAD = "bload";
char* BLOADN = "bloadn";
char* BLOADC = "bloadc";
char* BSTORE = "bstore";

/* ARITHMETIC INSTRUCTIONS */
char* IADD = "iadd";
char* ISUB = "isub";
char* IMUL = "imul";
char* IDIV = "idiv";
char* IREM = "irem";

char* FADD = "fadd";
char* FSUB = "fsub";
char* FMUL = "fmul";
char* FDIV = "fdiv";


/* CONVERSION INSTRUCTIONS */

char* I2F = "i2f";
char* F2I = "f2i";

/* CONDITIONALS */
char* ILT = "ilt";
char* ILE = "ile"; 
char* IGT = "igt";
char* IGE = "ige";
char* IEQ = "ieq";
char* INE = "ine";

char* FLT = "flt";
char* FLE = "fle"; 
char* FGT = "fgt";
char* FGE = "fge";
char* FEQ = "feq";
char* FNE = "fne";

char* BEQ = "beq";
char* BNE = "bne";

char* BRANCH_F = "branch_f";
char* BRANCH_T = "branch_t";
char* JMP = "jmp";



char* IINC = "iinc";

char* CONST_TABLE = ".const";
char* EXPORT_FUN = ".exportfun";
char* IMPORT_FUN = ".importfun";

char* ESR = "esr";
char* IRETURN = "ireturn";
char* BRETURN = "breturn";
char* FRETURN = "freturn";
char* RETURN = "return";

char* ISRN = "isrn";
char* ISR = "isr";
char* JSR = "jsr";
