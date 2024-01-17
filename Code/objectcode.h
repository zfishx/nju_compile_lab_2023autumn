#include "intermediate_code.h"

extern CodeList code_head;

typedef struct VarStructure_* VarStructure;
typedef struct Register_ Register;

struct VarStructure_{
    char* name; //变量名
    int reg; //变量存放的寄存器编号
    int offset; //变量在栈中的偏移量
    VarStructure next; //下一个变量
};

struct Register_{
    char* name; //寄存器名
    VarStructure var; //寄存器中存放的变量
    int used; //寄存器是否被使用
};

void generate_asm(FILE* f);
void generate_IR_asm(InterCode ic, FILE* f);
char* getopname(Operand op);
VarStructure newVar(char* name,int reg,int offset);
VarStructure getVar(Operand op);
void addvar2list(VarStructure var);
void op2var(Operand op);
void init_var(InterCode ic,FILE* f);
int getReg(Operand op,FILE* f);
void freeReg(int x,FILE* f,int flag);
void generate_IR_assign(InterCode ic,FILE* f);
void generate_IR_add(InterCode ic,FILE* f);
void generate_IR_sub(InterCode ic,FILE* f);
void generate_IR_mul(InterCode ic,FILE* f);
void generate_IR_div(InterCode ic,FILE* f);
void generate_IR_label(InterCode ic,FILE* f);
void generate_IR_goto(InterCode ic,FILE* f);
void generate_IR_ifgoto(InterCode ic,FILE* f);
void generate_IR_return(InterCode ic,FILE* f);
void generate_IR_dec(InterCode ic,FILE* f);
void generate_IR_arg(InterCode ic,FILE* f);
void generate_IR_call(InterCode ic,FILE* f);
CodeList generate_IR_func(CodeList c,FILE* f);
void generate_IR_param(InterCode ic,FILE* f,int index,int count);
void generate_IR_read(InterCode ic,FILE* f);
void generate_IR_write(InterCode ic,FILE* f);
void generate_IR_getaddr(InterCode ic,FILE* f);
void generate_IR_getvalue(InterCode ic,FILE* f);
