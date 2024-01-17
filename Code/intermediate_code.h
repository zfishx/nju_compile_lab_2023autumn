#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_tab.h"


typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct CodeList_* CodeList;
typedef struct ArgList_* ArgList;
typedef struct Variable_* Variable;


struct Operand_{
    enum{
        EM_VAR = 1, // 变量（var）
        EM_CONSTANT = 2, // 常量（#1）
        EM_ADDRESS = 3, // 地址（*var）
        EM_LABEL = 4, // 标签(LABEL label1:)
        EM_ARR = 5, // 数组（arr[2]）
        EM_STRUCT = 6, // 结构体（struct Point p）
        EM_TEMP = 7, // 临时变量（t1）
        EM_FUNC = 8,// 函数名（func）
        EM_GETADDR = 9// 取地址（&p）
    } kind;
    union{ 
        int varno; // 变量定义的序号
        int labelno; // 标签序号
        int val; // 操作数的值
        int tempno; // 临时变量序号（唯一性）
        char* funcname; // 函数名
    } u;
    char* symbolname;//符号的名字
    Type type; // 计算数组、结构体占用size
    int para; // 标识函数参数
};

struct InterCode_{
    enum{
        IC_ASSIGN = 1,
        IC_LABEL = 2,
        IC_PLUS = 3, 
        IC_SUB = 4,
        IC_MUL = 5,
        IC_DIV = 6,
        IC_DEC = 7,
        IC_FUNC = 8,
        IC_CALL = 9, 
        IC_PARAM = 10, 
        IC_ARG = 11,
        IC_GOTO = 12,
        IC_IFGOTO = 13,
        IC_READ = 14,
        IC_WRITE = 15, 
        IC_RETURN = 16,
        IC_GETADDR = 17,
        IC_GETVALUE = 18
    } kind; 
    union{
        char* func; // 函数名
        struct{Operand right, left; } assign; //二地址代码
        struct{Operand result, op1, op2; } three_op; //三地址代码
        struct{Operand op; int size;} dec; // 数组、结构体的定义
        struct{Operand op; } one_op; // 单目运算符，适用于IC_LABEL,IC_FUNC,IC_PARAM,IC_ARG,IC_GOTO,IC_READ,IC_WRITE,IC_RETURN
        struct{Operand op1, op2, label; char* relop;} ifgoto; // 条件跳转
    }u;
};

struct CodeList_{
    InterCode code; // 中间代码列表实现方式
    CodeList prev, next;
};
struct ArgList_{ // 参数列表实现方式
    Operand args;
    ArgList next;
};
struct Variable_{ // 变量的实现方式
    //char* name;
    Operand op;
    Variable next;
};
int var_num,label_num; // 新变量/新标签编号
CodeList code_head, code_tail; // 双链表的首部和尾部
Variable var_head,var_tail; // 变量表的首部和尾部

void add_codelist(InterCode code); // 添加中间代码

Operand new_temp();
Operand new_label();
Operand new_var();
Operand new_constant(int val);

Operand find_op(int varno);

Operand get_address(Operand op);
Operand get_value(Operand op);
Operand get_value_newic(Operand op);

void copy_array(Operand arr1, Operand arr2);

Operand copy_operand(Operand op);

void print_operand(Operand op, FILE* f);
void print_code(InterCode cur, FILE* f);
void print_IR(FILE* filename);

void Intercode(Node* root,FILE* f);
void translate_ExtDef(Node* ExtDef);
Operand translate_VarDec(Node* VarDec);
void translate_FunDec(Node* FunDec);
void translate_CompSt(Node* CompSt);
void translate_StmtList(Node* StmtList);
void translate_Stmt(Node* Stmt);
void translate_DefList(Node* DefList);
void translate_Def(Node* Def);
void translate_DecList(Node* DecList);
void translate_Dec(Node* Dec);
Operand translate_Exp(Node* Exp, Operand place);
void translate_Cond(Node* Exp, Operand label_true, Operand label_false);
void translate_Args(Node* Args, ArgList arg_list,int write_flag);