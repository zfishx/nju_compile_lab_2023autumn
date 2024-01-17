#include "objectcode.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define debug 1

Register r[32]; //定义寄存器
char* regName[] = {"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", 
                   "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 
                   "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
                   "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

VarStructure varlist_head = NULL; //定义变量表头
VarStructure varlist_tail = NULL; //定义变量表尾

int local_offset = 0; //记录偏移量
int arg_count = 0;

void generate_asm(FILE* f){
    fprintf(f,".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n");
    // 打印read函数定义
    fprintf(f,"\nread:\n");
    fprintf(f,"\tli $v0, 4\n");
    fprintf(f,"\tla $a0, _prompt\n");
    fprintf(f,"\tsyscall\n");
    fprintf(f,"\tli $v0, 5\n");
    fprintf(f,"\tsyscall\n");
    fprintf(f,"\tjr $ra\n");
    // 打印write函数定义
    fprintf(f,"\nwrite:\n");
    fprintf(f,"\tli $v0, 1\n");
    fprintf(f,"\tsyscall\n");
    fprintf(f,"\tli $v0, 4\n");
    fprintf(f,"\tla $a0, _ret\n");
    fprintf(f,"\tsyscall\n");
    fprintf(f,"\tmove $v0, $0\n");
    fprintf(f,"\tjr $ra\n\n");
    //寄存器
    for(int i=0;i<32;i++){
        Register x;
        x.name = regName[i];
        x.var = NULL;
        x.used = 0;
        r[i] = x;
    }
    CodeList cur = code_head;
    while(cur != NULL){
        /* if(cur->code->kind == IC_ARG){
            cur = generate_IR_arg(cur,f);
        } */
        if(cur->code->kind == IC_FUNC){
            cur = generate_IR_func(cur,f);
        }
        else{
            generate_IR_asm(cur->code,f);
            cur = cur->next;
        }
    }
}

void generate_IR_asm(InterCode ic, FILE* f){
    switch(ic->kind){
        //赋值操作：x := y
        case IC_ASSIGN:
            generate_IR_assign(ic,f);
            break;
        //二元运算：x := y op z
        case IC_PLUS:
            generate_IR_add(ic,f);
            break;
        case IC_SUB:
            generate_IR_sub(ic,f);
            break;
        case IC_MUL:
            generate_IR_mul(ic,f);
            break;
        case IC_DIV:
            generate_IR_div(ic,f);
            break;
        //LABEL x: 
        case IC_LABEL:
            generate_IR_label(ic,f);
            break;
        //GOTO x
        case IC_GOTO:
            generate_IR_goto(ic,f);
            break;
        //IF x relop y GOTO z
        case IC_IFGOTO:
            generate_IR_ifgoto(ic,f);
            break;
        //RETURN x
        case IC_RETURN:
            generate_IR_return(ic,f);
            break;
        //DEC x size
        case IC_DEC:
            generate_IR_dec(ic,f);
            break;
        //ARG x
        case IC_ARG:
            generate_IR_arg(ic,f);
            //assert(0);
            break;
        //x := CALL f
        case IC_CALL:
            generate_IR_call(ic,f);
            //assert(0);
            break;
        //FUNCTION f :
        case IC_FUNC:
            generate_IR_func(ic,f);
            break;
        //PARAM x
        case IC_PARAM:
            //generate_IR_param(ic,f);
            assert(0);
            break;
        //READ x
        case IC_READ:
            generate_IR_read(ic,f);
            break;
        //WRITE x
        case IC_WRITE:
            generate_IR_write(ic,f);
            break;
        //x := &y
        case IC_GETADDR:
            generate_IR_getaddr(ic,f);
            break;
        //x := *y
        case IC_GETVALUE:
            generate_IR_getvalue(ic,f);
            break;
        default:
            break;
    }
}

//获取一个operand的名字
char* getopname(Operand op){
    char* name = (char*)malloc(20); // 为结果字符串分配足够的内存
    if(name == NULL) {
        // 处理内存分配失败的情况
        assert(0);
        return NULL;
    }
    switch(op->kind){
        case EM_CONSTANT:
            sprintf(name, "#%d", op->u.val);
            break;
        case EM_VAR:
            sprintf(name, "v%d", op->u.varno);
            break;
        case EM_TEMP:
            sprintf(name, "t%d", op->u.varno);
            break;
        case EM_LABEL:
            sprintf(name, "label%d", op->u.varno);
            break;
        case EM_FUNC:
            strcpy(name, op->u.funcname);
            break;
        case EM_ARR:
            sprintf(name, "a%d", op->u.varno);
            break;
        case EM_ADDRESS:
            sprintf(name, "*t%d",op->u.varno);
            break;
        case EM_GETADDR:
            sprintf(name, "&a%d",op->u.varno);
            break;
        default:
            assert(0);
    }
    return name;
}
//创建一个新的变量
VarStructure newVar(char* name,int reg,int offset){
    VarStructure x = (VarStructure)malloc(sizeof(struct VarStructure_));
    x->name = name;
    x->reg = reg;
    x->offset = offset;
    x->next = NULL;
    return x;
}
//获取变量
VarStructure getVar(Operand op){
    //printf("lalalalalalal\n");
    VarStructure cur = varlist_head;
    while(cur != NULL){
        char* opname = getopname(op);
        //printf("opname = %s\n",opname);
        if(strcmp(cur->name,opname) == 0){
            //printf("opname = %s\n",opname);
            return cur;
        } 
        cur = cur->next;
    }
    //assert(0);
    return NULL;
}
//把变量加入变量表
void addvar2list(VarStructure var){
    if(varlist_head == NULL){
        varlist_head = var;
        varlist_tail = var;
    }
    else{
        VarStructure cur = varlist_head;
        while(cur != NULL){
            if(strcmp(cur->name,var->name) == 0){
                var->next = cur->next;
                printf("var->name = %s;var->offset = %d\n",var->name,var->offset);
                cur = var;
                return;
            }
            cur = cur->next;
        }
        varlist_tail->next = var;
        varlist_tail = var;
    }
}
//把operand转换成变量,并把变量加入变量表
void op2var(Operand op){
    if(op->kind == EM_CONSTANT) return;
    VarStructure var = getVar(op);
    if(var == NULL){
        local_offset += 4;
        char* name = getopname(op);
        var = newVar(name,-1,-local_offset);
        addvar2list(var);
        printf("var->name = %s;var->offset = %d\n",var->name,var->offset);
    }
}
//在函数开始时，初始化变量表
void init_var(InterCode ic,FILE* f){
    assert(ic!=NULL);
    switch(ic->kind){
        case IC_ASSIGN:
            op2var(ic->u.assign.left);
            op2var(ic->u.assign.right);
            break;
        case IC_PLUS:
            op2var(ic->u.three_op.result);
            op2var(ic->u.three_op.op1);
            op2var(ic->u.three_op.op2);
            break;
        case IC_SUB:
            op2var(ic->u.three_op.result);
            op2var(ic->u.three_op.op1);
            op2var(ic->u.three_op.op2);
            break;
        case IC_MUL:
            op2var(ic->u.three_op.result);
            op2var(ic->u.three_op.op1);
            op2var(ic->u.three_op.op2);
            break;
        case IC_DIV:
            op2var(ic->u.three_op.result);
            op2var(ic->u.three_op.op1);
            op2var(ic->u.three_op.op2);
            break;
        case IC_LABEL:
            break;
        case IC_GOTO:
            break;
        case IC_IFGOTO:
            op2var(ic->u.ifgoto.op1);
            op2var(ic->u.ifgoto.op2);
            break;
        case IC_RETURN:
            op2var(ic->u.one_op.op);
            break;
        case IC_DEC:
            local_offset += ic->u.dec.size * 4;
            if(debug) printf("ic->u.dec.size = %d\n", ic->u.dec.size);
            VarStructure var = newVar(getopname(ic->u.dec.op),-1,-local_offset);
            addvar2list(var);
            break;
        case IC_ARG:
            op2var(ic->u.one_op.op);
            break;
        case IC_CALL:
            op2var(ic->u.assign.left);
            //op2var(ic->u.assign.right);
            break;
        case IC_FUNC:
            break;
        case IC_PARAM:
            break;
        case IC_READ:
            op2var(ic->u.one_op.op);
            break;
        case IC_WRITE:
            op2var(ic->u.one_op.op);
            break;
        case IC_GETADDR:
            op2var(ic->u.assign.left);
            op2var(ic->u.assign.right);
            break;
        case IC_GETVALUE:
            op2var(ic->u.assign.left);
            op2var(ic->u.assign.right);
            break;
        default:
            break;
    
    }

}

//获取寄存器的编号
int getReg(Operand op,FILE* f){
    //使用调用者保存的寄存器t0-t7
    for(int i=8;i<16;i++){
        if(r[i].used == 0){
            r[i].used = 1;
            if(op->kind == EM_CONSTANT) fprintf(f,"\tli %s, %d\n",r[i].name,op->u.val);
            else if(op->kind == EM_GETADDR){
                op->kind = EM_ARR;
                VarStructure var1 = getVar(op);
                op->kind = EM_GETADDR;
                VarStructure var2 = getVar(op);
                fprintf(f, "\taddi %s, $fp, %d\n", r[i].name, var1->offset);
                var2->reg = i;
                r[i].var = var2;
            } 
            else if(op->kind == EM_ADDRESS){
                //*x
                op->kind = EM_TEMP;
                VarStructure var1 = getVar(op);//x
                op->kind = EM_ADDRESS;
                VarStructure var2 = getVar(op);//*x
                fprintf(f, "\tlw %s, %d($fp)\n", r[i].name, var1->offset);//把x存储在寄存器r[i]中
                fprintf(f, "\tlw %s, 0(%s)\n", r[i].name,r[i].name);//从*x处加载数据到寄存器r[i]中
                var2->reg = i;
                r[i].var = var2;
            }
            else{
                VarStructure var = getVar(op);
                assert(op!=NULL);
                var->reg = i;
                fprintf(f, "\tlw %s, %d($fp)\n", r[i].name, var->offset);
                r[i].var = var;
            }
            return i;
        }
    }
    assert(0);
    return -1;
}
//释放寄存器
void freeReg(int x,FILE* f,int flag){
    if(flag) fprintf(f,"\tsw %s, %d($fp)\n",r[x].name,r[x].var->offset);
    r[x].used = 0;
    r[x].var = NULL;
}

void generate_IR_assign(InterCode ic,FILE* f){
    //获取赋值语句左右操作数left和right;
    Operand left = ic->u.assign.left;
    Operand right = ic->u.assign.right;
    //if(left->kind == EM_ADDRESS && right->kind != EM_ADDRESS){
    if(left->kind == EM_ADDRESS){
        left->kind = EM_TEMP;
        VarStructure x = getVar(left);
        left->kind = EM_ADDRESS;
        assert(r[16].used == 0);//s0没有被占用
        fprintf(f, "\tlw $s0, %d($fp)\n", x->offset);//把x存储在寄存器$s0中
        int y = getReg(right,f);
        fprintf(f, "\tsw %s, 0($s0)\n", r[y].name);//将寄存器r[y]中的数据存储到*x处
        freeReg(y,f,0);
    }
    else{
        int x = getReg(left,f);
        int y = getReg(right,f);
        fprintf(f,"\tmove %s, %s\n",r[x].name,r[y].name);
        freeReg(x,f,1);
        freeReg(y,f,0);
    }
}

void generate_IR_add(InterCode ic,FILE* f){
    Operand result = ic->u.three_op.result;
    Operand op1 = ic->u.three_op.op1;
    Operand op2 = ic->u.three_op.op2;
    int x = getReg(result,f);
    int y = getReg(op1,f);
    int z = getReg(op2,f);
    fprintf(f,"\tadd %s, %s, %s\n",r[x].name,r[y].name,r[z].name);
    freeReg(x,f,1);
    freeReg(y,f,0);
    freeReg(z,f,0);
}

void generate_IR_sub(InterCode ic,FILE* f){
    Operand result = ic->u.three_op.result;
    Operand op1 = ic->u.three_op.op1;
    Operand op2 = ic->u.three_op.op2;
    int x = getReg(result,f);
    int y = getReg(op1,f);
    int z = getReg(op2,f);
    fprintf(f,"\tsub %s, %s, %s\n",r[x].name,r[y].name,r[z].name);
    freeReg(x,f,1);
    freeReg(y,f,0);
    freeReg(z,f,0);
}

void generate_IR_mul(InterCode ic,FILE* f){
    Operand result = ic->u.three_op.result;
    Operand op1 = ic->u.three_op.op1;
    Operand op2 = ic->u.three_op.op2;
    int x = getReg(result,f);
    int y = getReg(op1,f);
    int z = getReg(op2,f);
    fprintf(f,"\tmul %s, %s, %s\n",r[x].name,r[y].name,r[z].name);
    freeReg(x,f,1);
    freeReg(y,f,0);
    freeReg(z,f,0);
}

void generate_IR_div(InterCode ic,FILE* f){
    Operand result = ic->u.three_op.result;
    Operand op1 = ic->u.three_op.op1;
    Operand op2 = ic->u.three_op.op2;
    int x = getReg(result,f);
    int y = getReg(op1,f);
    int z = getReg(op2,f);
    fprintf(f,"\tdiv %s, %s, %s\n",r[x].name,r[y].name,r[z].name);
    freeReg(x,f,1);
    freeReg(y,f,0);
    freeReg(z,f,0);
}

void generate_IR_label(InterCode ic,FILE* f){
    fprintf(f,"label%d:\n",ic->u.one_op.op->u.varno);
}

void generate_IR_goto(InterCode ic,FILE* f){
    fprintf(f,"\tj label%d\n",ic->u.one_op.op->u.varno);
}

void generate_IR_ifgoto(InterCode ic,FILE* f){
    Operand op1 = ic->u.ifgoto.op1;
    Operand op2 = ic->u.ifgoto.op2;
    Operand label = ic->u.ifgoto.label;
    char* relop = ic->u.ifgoto.relop;
    int r_op1 = getReg(op1,f);
    int r_op2 = getReg(op2,f);
    if(strcmp(relop,"==") == 0) fprintf(f,"\tbeq %s, %s, label%d\n",r[r_op1].name,r[r_op2].name,label->u.varno);
    else if(strcmp(relop,"!=") == 0) fprintf(f,"\tbne %s, %s, label%d\n",r[r_op1].name,r[r_op2].name,label->u.varno);
    else if(strcmp(relop,">") == 0) fprintf(f,"\tbgt %s, %s, label%d\n",r[r_op1].name,r[r_op2].name,label->u.varno);
    else if(strcmp(relop,"<") == 0) fprintf(f,"\tblt %s, %s, label%d\n",r[r_op1].name,r[r_op2].name,label->u.varno);
    else if(strcmp(relop,">=") == 0) fprintf(f,"\tbge %s, %s, label%d\n",r[r_op1].name,r[r_op2].name,label->u.varno);
    else if(strcmp(relop,"<=") == 0) fprintf(f,"\tble %s, %s, label%d\n",r[r_op1].name,r[r_op2].name,label->u.varno);
    freeReg(r_op1,f,0);
    freeReg(r_op2,f,0);
}

void generate_IR_return(InterCode ic,FILE* f){
    Operand op = ic->u.one_op.op;
    int r_op = getReg(op,f);
    fprintf(f,"\tmove $v0, %s\n",r[r_op].name);
    freeReg(r_op,f,0);
    fprintf(f, "\taddi $sp, $fp, 8\n"); // 恢复$sp的值
    fprintf(f, "\tlw $ra, 0($fp)\n"); // 恢复$ra的值
    fprintf(f, "\tlw $fp, 4($fp)\n"); // 恢复$fp的值   
    fprintf(f,"\tjr $ra\n");
    
}

void generate_IR_dec(InterCode ic,FILE* f){

}

void generate_IR_arg(InterCode ic,FILE* f){
    //int arg_count = 0;
    int reg = getReg(ic->u.one_op.op,f);
    //fprintf(f,"\tlw $s0, %d($fp)\n",arg_count*4);
    fprintf(f,"\tsubu $sp, $sp, 4\n");
    fprintf(f,"\tsw %s, 0($sp)\n",r[reg].name); //把所有的实参都压入栈中，没有像指导手册上把前三个放入寄存器中
    freeReg(reg,f,0);
    arg_count++;
    return ;
}

void generate_IR_call(InterCode ic,FILE* f){
    assert(ic->kind == IC_CALL);
    //fprintf(f, "\tsubu $sp, $sp, 4\n");
    //fprintf(f, "\tsw $ra, 0($sp)\n"); // 保存$ra寄存器
    fprintf(f, "\tjal %s\n", ic->u.assign.right->u.funcname); // 调用函数
    //fprintf(f, "\tlw $ra, 0($sp)\n"); // 恢复$ra的值
    //fprintf(f, "\taddi $sp, $sp, 4\n"); // 回收$ra占用的栈空间
    //fprintf(f, "\taddi $sp, $sp, 4\n"); // 回收$fp占用的栈空间
    fprintf(f, "\taddi $sp, $sp, %d\n",arg_count*4); //回收实参占用的栈空间;
    arg_count = 0;
    int return_reg = getReg(ic->u.assign.left,f);//为返回值分配一个寄存器x
    fprintf(f, "\tmove %s, $v0\n", r[return_reg].name); //将返回值保存;
    freeReg(return_reg,f,1);
}

CodeList generate_IR_func(CodeList c,FILE* f){
    InterCode ic = c->code;
    fprintf(f,"%s:\n",ic->u.func);
    fprintf(f,"\taddi $sp, $sp, -8\n");
    fprintf(f,"\tsw $ra, 0($sp)\n");
    fprintf(f,"\tsw $fp, 4($sp)\n");
    fprintf(f,"\tmove $fp, $sp\n");
    c = c->next;
    local_offset = 0;
    CodeList temp = c;
    int index = 0;//记录当前参数是第几个参数
    while(c->code->kind == IC_PARAM){
        //generate_IR_param(c->code,f,index);
        c = c->next;
        index++;
    }
    int count = index;
    index = 0;
    c = temp;
    printf("count = %d\n",count);
    while(c->code->kind == IC_PARAM){
        generate_IR_param(c->code,f,index,count);
        c = c->next;
        index++;
    }
    CodeList res = c;
    while(c!=NULL && c->code->kind != IC_FUNC){
        init_var(c->code,f);
        c = c->next;
    }
    fprintf(f,"\tsubu $sp, $sp, %d\n",local_offset);//为局部变量分配空间
    return res;
}

void generate_IR_param(InterCode ic,FILE* f,int index,int count){
    assert(ic->kind == IC_PARAM);
    char* name = getopname(ic->u.one_op.op);
    VarStructure var = newVar(name, -1 , 8 + index*4);
    addvar2list(var);
    //fprintf(f,"\taddi $sp, $sp, -4\n");
    //fprintf(f,"\tsw $fp, 0($sp)\n");
}

void generate_IR_read(InterCode ic,FILE* f){
    Operand op = ic->u.one_op.op;
    int reg = getReg(op,f);
    fprintf(f,"\taddi $sp, $sp, -4\n");
    fprintf(f,"\tsw $ra, 0($sp)\n");
    fprintf(f,"\tjal read\n");
    fprintf(f,"\tlw $ra, 0($sp)\n");
    fprintf(f,"\taddi $sp, $sp, 4\n");
    fprintf(f,"\tmove %s, $v0\n",r[reg].name);
    freeReg(reg,f,1);
}

void generate_IR_write(InterCode ic,FILE* f){
    Operand op = ic->u.one_op.op;
    int reg = getReg(op,f);
    fprintf(f,"\tmove $a0, %s\n",r[reg].name);
    fprintf(f,"\taddi $sp, $sp, -4\n");
    fprintf(f,"\tsw $ra, 0($sp)\n");
    fprintf(f,"\tjal write\n");
    fprintf(f,"\tlw $ra, 0($sp)\n");
    fprintf(f,"\taddi $sp, $sp, 4\n");
    freeReg(reg,f,0);
}

void generate_IR_getaddr(InterCode ic,FILE* f){
}

void generate_IR_getvalue(InterCode ic,FILE* f){
}
