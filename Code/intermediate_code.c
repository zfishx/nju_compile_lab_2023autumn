#include "intermediate_code.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define debug 1

int structure_error = 0;

FILE* f2 = NULL;

void print_Operand(Operand op,FILE* f){
    f2 = f;
    if(op == NULL){
        fprintf(f,"Error: Operand is NULL!\n");
        return;
    }
    switch(op->kind){
        case EM_TEMP:
            fprintf(f,"t%d",op->u.varno);
            break;
        case EM_LABEL:
            fprintf(f,"label%d",op->u.varno);
            break;
        case EM_VAR:
            fprintf(f,"v%d",op->u.varno);
            break;
        case EM_CONSTANT:
            fprintf(f,"#%d",op->u.val);
            break;
        case EM_FUNC:
            fprintf(f,"%s",op->u.funcname);
            break;
        case EM_ADDRESS:
            fprintf(f,"*t%d",op->u.varno);
            break;
        case EM_ARR:
            fprintf(f,"a%d",op->u.varno);
            break;
        case EM_GETADDR:
            fprintf(f,"&a%d",op->u.varno);
            break;
        default:
            fprintf(f,"Error: Operand kind error!\n");
            break;
    }
}

void print_code(InterCode cur, FILE* f){
    switch(cur->kind){
        case IC_LABEL:
            fprintf(f,"LABEL ");
            print_Operand(cur->u.one_op.op,f);
            fprintf(f," :");
            fprintf(f,"\n");
            break;
        case IC_FUNC:
            fprintf(f,"FUNCTION %s :",cur->u.func);
            fprintf(f,"\n");
            break;
        case IC_ASSIGN:
            print_Operand(cur->u.assign.left,f);
            fprintf(f," := ");
            print_Operand(cur->u.assign.right,f);
            fprintf(f,"\n");
            break;
        case IC_ADDR:
            print_Operand(cur->u.three_op.result,f);
            fprintf(f," := ");
            print_Operand(cur->u.three_op.op1,f);
            fprintf(f," + ");
            print_Operand(cur->u.three_op.op2,f);
            fprintf(f,"\n");
            break;
        case IC_PLUS:
            print_Operand(cur->u.three_op.result,f);
            fprintf(f," := ");
            print_Operand(cur->u.three_op.op1,f);
            fprintf(f," + ");
            print_Operand(cur->u.three_op.op2,f);
            fprintf(f,"\n");
            break;
        case IC_SUB:
            print_Operand(cur->u.three_op.result,f);
            fprintf(f," := ");
            print_Operand(cur->u.three_op.op1,f);
            fprintf(f," - ");
            print_Operand(cur->u.three_op.op2,f);
            fprintf(f,"\n");
            break;
        case IC_MUL:
            print_Operand(cur->u.three_op.result,f);
            fprintf(f," := ");
            print_Operand(cur->u.three_op.op1,f);
            fprintf(f," * ");
            print_Operand(cur->u.three_op.op2,f);
            fprintf(f,"\n");
            break;
        case IC_DIV:
            print_Operand(cur->u.three_op.result,f);
            fprintf(f," := ");
            print_Operand(cur->u.three_op.op1,f);
            fprintf(f," / ");
            print_Operand(cur->u.three_op.op2,f);
            fprintf(f,"\n");
            break;
        case IC_GETVALUE:
            print_Operand(cur->u.assign.left,f);
            fprintf(f," := *");
            print_Operand(cur->u.assign.right,f);
            fprintf(f,"\n");
            break;
        case IC_GETADDR:
            print_Operand(cur->u.assign.left,f);
            fprintf(f," := &");
            print_Operand(cur->u.assign.right,f);
            fprintf(f,"\n");
            break;
        case IC_GOTO:
            fprintf(f,"GOTO ");
            print_Operand(cur->u.one_op.op,f);
            fprintf(f,"\n");
            break;
        case IC_IFGOTO:
            fprintf(f,"IF ");
            print_Operand(cur->u.ifgoto.op1,f);
            fprintf(f," %s ",cur->u.ifgoto.relop);
            print_Operand(cur->u.ifgoto.op2,f);
            fprintf(f," GOTO ");
            print_Operand(cur->u.ifgoto.label,f);
            fprintf(f,"\n");
            break;
        case IC_RETURN:
            fprintf(f,"RETURN ");
            print_Operand(cur->u.one_op.op,f);
            fprintf(f,"\n");
            break;
        case IC_DEC:
            fprintf(f,"DEC ");
            print_Operand(cur->u.dec.op,f);
            fprintf(f," %d",cur->u.dec.size);
            fprintf(f,"\n");
            break;
        case IC_ARG:
            fprintf(f,"ARG ");
            print_Operand(cur->u.one_op.op,f);
            fprintf(f,"\n");
            break;
        case IC_CALL:
            print_Operand(cur->u.assign.left,f);
            fprintf(f," := CALL ");
            print_Operand(cur->u.assign.right,f);
            fprintf(f,"\n");
            break;
        case IC_PARAM: 
            fprintf(f,"PARAM ");
            print_Operand(cur->u.one_op.op,f);
            fprintf(f,"\n");
            break;
        case IC_READ:
            fprintf(f,"READ ");
            print_Operand(cur->u.one_op.op,f);
            fprintf(f,"\n");
            break;
        case IC_WRITE:
            fprintf(f,"WRITE ");
            print_Operand(cur->u.one_op.op,f);
            fprintf(f,"\n");
            break;
        default:   
            fprintf(f,"%d\n",cur->kind);
            fprintf(f,"Error: Intercode kind error!\n");
            break;
    }        
}

void print_IR(FILE* f){
    if(structure_error == 1){
        return;
    }
    CodeList cur = code_head;
    while(cur != NULL){
        print_code(cur->code,f);       
        cur = cur->next;
    }
}

void add_codelist(InterCode code){
    CodeList c = (CodeList)malloc(sizeof(struct CodeList_));
    c->code = code;
    c->next = NULL;
    if(code_head == NULL){
        code_head = c;
        code_tail = c;
    }
    else{
        code_tail->next = c;
        c->prev = code_tail;
        code_tail = c;
    }
}

Operand new_temp(){
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = EM_TEMP;
    op->u.varno = var_num++;
    Variable var = (Variable)malloc(sizeof(struct Variable_));
    var->op = op;
    var->next = NULL;
    if(var_head == NULL){
        var_head = var;
        var_tail = var;
    }
    else{
        var_tail->next = var;
              var_tail = var;
    } 
    return op;
}

Operand new_label(){
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = EM_LABEL;
    op->u.varno = label_num++;
    return op;
}

Operand new_var(){
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = EM_VAR;
    op->u.varno = var_num++;
    Variable var = (Variable)malloc(sizeof(struct Variable_));
    var->op = op;
    var->next = NULL;
    if(var_head == NULL){
        var_head = var;
        var_tail = var;
    }
    else{
        var_tail->next = var;
        var_tail = var;
    }
    return op;
}

Operand new_constant(int val){
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = EM_CONSTANT;
    op->u.val = val;
    return op;
}

Operand new_array(Type type){
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = EM_ARR;
    op->u.varno = var_num++;
    op->type = type;
    Variable var = (Variable)malloc(sizeof(struct Variable_));
    var->op = op;
    var->next = NULL;
    if(var_head == NULL){
        var_head = var;
        var_tail = var;
    }
    else{
        var_tail->next = var;
        var_tail = var;
    }
    return op;
}

Operand find_op(int varno){
    if(debug) printf("find_op\n");
    if(varno == -1) return NULL;
    Variable cur = var_head;
    while(cur != NULL){
        assert(cur->op != NULL);
        printf("cur->op->u.varno:%d , id:%s , kind:%d\n",cur->op->u.varno,cur->op->symbolname,cur->op->kind);
        if(cur->op->u.varno == varno){
            printf("return op:%d , id:%s , kind:%d\n",cur->op->u.varno,cur->op->symbolname,cur->op->kind);
            printf("return cur->op\n");
            return cur->op;
        }
        cur = cur->next;
    }
    printf("return NULL\n");
    return NULL;
}

Operand get_address(Operand op){
    if(op->kind == EM_ARR){
        Operand temp = new_temp();
        //temp := &op
        temp->kind = EM_GETADDR;
        temp->u.varno = op->u.varno;
        return temp;
    }
    else{
        return op;
    }
}

void copy_array(Operand arr1, Operand arr2){
    //数组中的每个元素赋值
    int leftsize = arr1->type->u.array.size;
    int rightsize = arr2->type->u.array.size;
    int minsize = leftsize < rightsize ? leftsize : rightsize;
    int i = 0;
    Operand t1 = get_address(arr1);
    Operand t2 = get_address(arr2);
    Operand t3 = new_temp();
    Operand t4 = new_temp();
    while(i < minsize){
        //t3 := t1 + t2*4*size
        //arr1[i]
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_PLUS;
        ic->u.three_op.result = t3;
        ic->u.three_op.op1 = t1;
        ic->u.three_op.op2 = new_constant(i*4);
        add_codelist(ic);
        //arr2[i]
        InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
        ic1->kind = IC_PLUS;
        ic1->u.three_op.result = t4;
        ic1->u.three_op.op1 = t2;
        ic1->u.three_op.op2 = new_constant(i*4);
        add_codelist(ic1);
        //val := *t4
        Operand val = new_temp();
        InterCode ic3 = (InterCode)malloc(sizeof(struct InterCode_));
        ic3->kind = IC_GETVALUE;
        ic3->u.assign.left = val;
        ic3->u.assign.right = t4;
        add_codelist(ic3);
        //*t3 := val
        Operand t5 = new_temp();
        t5->kind = EM_ADDRESS;
        t5->u.varno = t3->u.varno;
        InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
        ic2->kind = IC_ASSIGN;
        ic2->u.assign.left = t5;
        ic2->u.assign.right = val;
        add_codelist(ic2);
        i++;
    }
}
Operand copy_operand(Operand op){
    Operand temp = (Operand)malloc(sizeof(struct Operand_));
    temp->kind = op->kind;
    temp->u.varno = op->u.varno;
    temp->type = op->type;
    temp->symbolname = (char*)malloc(sizeof(char)*32);
    strcpy(temp->symbolname,op->symbolname);
    temp->para = op->para;
    return temp;
}

void Intercode(Node* root,FILE* f){
    f2 = f;
    if(debug) printf("Intercode\n");
    //首先判断AST是否为空
    if(root == NULL) return;
    //接着判断AST结构是否正确
    if(strcmp(root->name, "Program") != 0){
        //printf("Error: AST structure error!\n");
        return;
    }
    code_head = NULL;
    code_tail = NULL;
    var_head = NULL;
    var_tail = NULL;
    var_num = 1;
    label_num = 1;
    //遍历AST，一次调用translate_X函数
    Node* node = root->child; // Program -> ExtDefList
    while(node != NULL){
        node = node->child; // ExtDefList -> ExtDef ExtDefList
        translate_ExtDef(node);
        if(structure_error == 1) return;
        node = node->brother;
    }
    return;

}

void translate_ExtDef(Node* ExtDef){
    if(debug) printf("translate_ExtDef\n");
    //ExtDef -> Specifier ExtDecList SEMI
        //| Specifier SEMI
        //| Specifier FunDec CompSt
    //首先判断AST是否为空
    if(ExtDef == NULL) return;
    //接着判断AST结构是否正确
    assert(strcmp(ExtDef->name, "ExtDef") == 0);
    //检查是否为";"
    if(strcmp(ExtDef->child->brother->name, "SEMI") == 0){
        return;
    }
    //检查是否有全局标识符定义 根据假设，没有全局变量
    else if(strcmp(ExtDef->child->brother->name, "ExtDecList") == 0){
        return;
    }
    else if(strcmp(ExtDef->child->brother->name, "FunDec") == 0){
        assert(strcmp(ExtDef->child->brother->brother->name, "CompSt") == 0);
        //ExtDef -> Specifier FunDec CompSt
        translate_FunDec(ExtDef->child->brother);
        if(structure_error == 1) return;
        translate_CompSt(ExtDef->child->brother->brother);
    }
    else{
        printf("Error: AST structure error!\n");
        return;
    }
}

Operand translate_VarDec(Node* VarDec){
    if(debug) printf("translate_VarDec\n");
    //VarDec -> ID
        //| VarDec LB INT RB
    //首先判断AST是否为空
    if(VarDec == NULL) return;
    //接着判断AST结构是否正确
    assert(strcmp(VarDec->name, "VarDec") == 0);
    Operand res = NULL;
    if(VarDec->child->brother == NULL){
        //VarDec -> ID
        FieldList var = find(VarDec->child->val.val_id);
        if(var == NULL){
            printf("Error: undefined variable!\n");
            printf("val_id:%s\n",VarDec->child->val.val_id);
            return NULL;
        }
        if(var->type->kind == STRUCTURE){
            printf("Cannot translate: Code contains variables or parameters of structure type.\n");
            structure_error = 1;
        }
        Operand temp = find_op(var->var_no);
        if(temp != NULL){
            return temp;
        }
        else{
            res = new_var();
            res->symbolname = (char*)malloc(sizeof(char)*32);
            strcpy(res->symbolname,VarDec->child->val.val_id);
            res->type = var->type;
            if(var->var_no == -1) var->var_no = res->u.varno;
            else res->u.varno = var->var_no;
            printf("defind:val_id:%s var_no:%d\n",VarDec->child->val.val_id,var->var_no);
        }
    }
    else{
        //VarDec -> VarDec LB INT RB
        Node* v2 = VarDec->child;
        while(v2!= NULL && strcmp(v2->name, "ID") != 0){
            v2 = v2->child;
        }
        FieldList f = find(v2->val.val_id);
        if(f == NULL){
            printf("Error: undefined variable!\n");
            return NULL;
        }
        res = new_array(f->type);
        //fprintf(f2,"res->type->u.array.size:%d,varno:%d\n",res->type->u.array.size,res->u.varno);
        res->symbolname = (char*)malloc(sizeof(char)*32);
        strcpy(res->symbolname,v2->val.val_id);
        if(f->var_no == -1) f->var_no = res->u.varno;
        else res->u.varno = f->var_no;
        //res = translate_VarDec(VarDec->child);
        //DEC res child->brother->brother->val.val_int
        /* if(VarDec->child->child->brother == NULL){ */
        /*     InterCode ic = (InterCode)malloc(sizeof(struct InterCode_)); */
        /*     ic->kind = IC_DEC; */
        /*     ic->u.dec.op = res; */
        /*     //ic->u.dec.size = VarDec->child->brother->brother->val.val_int * 4; */
        /*     Type temp_type = res->type->u.array.elem; */
        /*     int dec_size = 1; */
        /*     while(temp_type != NULL && temp_type->kind == ARRAY){ */
        /*         //res->type->u.array.size *= temp_type->u.array.size; */
        /*         dec_size *= temp_type->u.array.size; */
        /*         temp_type = temp_type->u.array.elem; */
        /*     } */
        /*     ic->u.dec.size = dec_size * 4; */
        /*     add_codelist(ic); */
        //}
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_DEC;
        ic->u.dec.op = res;
        Type temp_type = res->type->u.array.elem;
        int dec_size = res->type->u.array.size;
        while(temp_type != NULL && temp_type->kind == ARRAY){
            dec_size *= temp_type->u.array.size;
            temp_type = temp_type->u.array.elem;
        }
        ic->u.dec.size = dec_size * 4;
        add_codelist(ic);
    }
    return res;
}

void translate_FunDec(Node* FunDec){
    if(debug) printf("translate_FunDec\n");
    //FunDec -> ID LP VarList RP
        //| ID LP RP
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    ic->kind = IC_FUNC;
    ic->u.func = FunDec->child->val.val_id;//获取函数名
    add_codelist(ic);
    if(strcmp(FunDec->child->brother->brother,"RP") == 0){
        //FunDec -> ID LP RP
        //函数声明
        return;
    }
    else{
        //FunDec -> ID LP VarList RP
        //获取参数列表
        FieldList param = find(FunDec->child->val.val_id)->type->u.function->param;
        while(param != NULL){
            Operand op = NULL;
            if(param->var_no != -1){
                op = find_op(param->var_no);
            }
            if(op == NULL){
                if(param->type->kind == BASIC){
                    op = new_var();
                    op->type = param->type;
                }
                else if(param->type->kind == ARRAY){
                    //array
                    op = new_var();
                    op->type = param->type;
                }
                else if(param->type->kind == STRUCTURE){
                    //structure
                    //assert(0);
                    printf("Cannot translate: Code contains variables or parameters of structure type.\n");
                    structure_error = 1;
                    return;
                }
                op->symbolname = (char*)malloc(sizeof(char)*32);
                strcpy(op->symbolname,param->name);
                if(param->var_no == -1) param->var_no = op->u.varno;
                else op->u.varno = param->var_no;
            }
            InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
            ic1->kind = IC_PARAM;
            ic1->u.one_op.op = op;
            add_codelist(ic1);
            param = param->tail;
        }
    }
}

void translate_CompSt(Node* CompSt){
    if(debug) printf("translate_CompSt\n");
    //CompSt -> LC DefList StmtList RC
    //首先判断AST是否为空
    if(CompSt == NULL) return;
    //接着判断AST结构是否正确
    assert(strcmp(CompSt->name, "CompSt") == 0);
    //CompSt -> LC DefList StmtList RC
    //可能没有DefList
    if(CompSt->child->brother->brother->brother == NULL){
        //CompSt -> LC StmtList RC
        translate_StmtList(CompSt->child->brother);
    }
    else{
        //CompSt -> LC DefList StmtList RC
        translate_DefList(CompSt->child->brother);
        translate_StmtList(CompSt->child->brother->brother);
    }
}

void translate_StmtList(Node* StmtList){
    if(debug) printf("translate_StmtList\n");
    //StmtList -> Stmt StmtList
        //| empty
    //首先判断AST是否为空
    if(StmtList == NULL) return;
    //接着判断AST结构是否正确
    assert(strcmp(StmtList->name, "StmtList") == 0);
    if(StmtList->child == NULL){
        //StmtList -> empty
        return;
    }
    else{
        //StmtList -> Stmt StmtList
        translate_Stmt(StmtList->child);
        translate_StmtList(StmtList->child->brother);
    }
}

void translate_Stmt(Node* Stmt){
    if(debug) printf("translate_Stmt\n");
    //Stmt -> Exp SEMI
        //| CompSt
        //| RETURN Exp SEMI
        //| IF LP Exp RP Stmt
        //| IF LP Exp RP Stmt ELSE Stmt
        //| WHILE LP Exp RP Stmt
    //首先判断AST是否为空
    if(Stmt == NULL) return;
    //接着判断AST结构是否正确
    assert(strcmp(Stmt->name, "Stmt") == 0);
    if(strcmp(Stmt->child->name, "Exp") == 0){
        //Stmt -> Exp SEMI
        Operand t1 = new_temp();
        t1 = translate_Exp(Stmt->child,t1);
    }
    else if(strcmp(Stmt->child->name, "CompSt") == 0){
        //Stmt -> CompSt
        translate_CompSt(Stmt->child);
    }
    else if(strcmp(Stmt->child->name, "RETURN") == 0){
        //Stmt -> RETURN Exp SEMI
        Operand t1 = new_temp();
        //code1
        t1 = translate_Exp(Stmt->child->brother,t1);
        //RETURN t1
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_RETURN;
        ic->u.one_op.op = t1;
        add_codelist(ic);
    }
    else if(strcmp(Stmt->child->name, "IF") == 0){
        if(Stmt->child->brother->brother->brother->brother->brother != NULL){
            //Stmt -> IF LP Exp RP Stmt ELSE Stmt
            if(debug) printf("Stmt -> IF LP Exp RP Stmt ELSE Stmt\n");
            Operand label1 = new_label();
            Operand label2 = new_label();
            Operand label3 = new_label();
            //code1
            translate_Cond(Stmt->child->brother->brother, label1, label2);
            //Label label1
            InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
            ic1->kind = IC_LABEL;
            ic1->u.one_op.op = label1;
            add_codelist(ic1);
            //code2
            translate_Stmt(Stmt->child->brother->brother->brother->brother);
            //GOTO label3
            InterCode ic_right = code_tail->code;
            if(ic_right->kind != IC_RETURN){
                //优化，判断正确的末尾是不是return语句，如果是，就不用生成GOTO语句
                InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
                ic2->kind = IC_GOTO;
                ic2->u.one_op.op = label3;
                printf("Stmt -> IF LP Exp RP Stmt ELSE Stmt label_id:%d\n",label3->u.varno);
                add_codelist(ic2);
            }
            //Label label2
            InterCode ic3 = (InterCode)malloc(sizeof(struct InterCode_));
            ic3->kind = IC_LABEL;
            ic3->u.one_op.op = label2;
            add_codelist(ic3);
            //code3
            translate_Stmt(Stmt->child->brother->brother->brother->brother->brother->brother);
            //Label label3
            if(ic_right->kind != IC_RETURN){
                InterCode ic4 = (InterCode)malloc(sizeof(struct InterCode_));
                ic4->kind = IC_LABEL;
                ic4->u.one_op.op = label3;
                add_codelist(ic4);
            }
        }
        else{
            //Stmt -> IF LP Exp RP Stmt
            if(debug) printf("Stmt -> IF LP Exp RP Stmt\n");
            Operand label1 = new_label();
            Operand label2 = new_label();  
            //code
            translate_Cond(Stmt->child->brother->brother, label1, label2);
            
            //Label label1
            InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
            ic1->kind = IC_LABEL;
            ic1->u.one_op.op = label1;
            add_codelist(ic1);
            //code2
            translate_Stmt(Stmt->child->brother->brother->brother->brother);
            //Label label2
            InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
            ic2->kind = IC_LABEL;
            ic2->u.one_op.op = label2;
            add_codelist(ic2);
        }
    }
    else if(strcmp(Stmt->child->name, "WHILE") == 0){
        //Stmt -> WHILE LP Exp RP Stmt
        if(debug) printf("Stmt -> WHILE LP Exp RP Stmt\n");
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand label3 = new_label();
        //Label label1
        InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
        ic1->kind = IC_LABEL;
        ic1->u.one_op.op = label1;
        add_codelist(ic1);
        //code1
        translate_Cond(Stmt->child->brother->brother, label2, label3);
        //Label label2
        InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
        ic2->kind = IC_LABEL;
        ic2->u.one_op.op = label2;
        add_codelist(ic2);
        //code2
        translate_Stmt(Stmt->child->brother->brother->brother->brother);
        //GOTO label1
        InterCode ic3 = (InterCode)malloc(sizeof(struct InterCode_));
        ic3->kind = IC_GOTO;
        ic3->u.one_op.op = label1;
        add_codelist(ic3);
        //Label label3
        InterCode ic4 = (InterCode)malloc(sizeof(struct InterCode_));
        ic4->kind = IC_LABEL;
        ic4->u.one_op.op = label3;
        add_codelist(ic4);
    }
    else{
        printf("Error: AST structure error!\n");
        return;
    }
}

void translate_DefList(Node* DefList){
    if(debug) printf("translate_DefList\n");
    //DefList -> Def DefList
        //| empty
    //首先判断AST是否为空
    if(DefList == NULL) return;
    //接着判断AST结构是否正确
    assert(strcmp(DefList->name, "DefList") == 0);
    if(DefList->child == NULL){
        //DefList -> empty
        return;
    }
    else{
        //DefList -> Def DefList
        translate_Def(DefList->child);
        translate_DefList(DefList->child->brother);
    }
}

void translate_Def(Node* Def){
    if(debug) printf("translate_Def\n");
    //Def -> Specifier DecList SEMI
    //首先判断AST是否为空
    if(Def == NULL) return;
    //接着判断AST结构是否正确
    assert(strcmp(Def->name, "Def") == 0);
    //Def -> Specifier DecList SEMI
    translate_DecList(Def->child->brother);
}

void translate_DecList(Node* DecList){
    if(debug) printf("translate_DecList\n");
    //DecList -> Dec
        //| Dec COMMA DecList
    //首先判断AST是否为空
    if(DecList == NULL) return;
    //接着判断AST结构是否正确
    assert(strcmp(DecList->name, "DecList") == 0);
    if(DecList->child->brother == NULL){
        //DecList -> Dec
        translate_Dec(DecList->child);
    }
    else{
        //DecList -> Dec COMMA DecList
        translate_Dec(DecList->child);
        translate_DecList(DecList->child->brother->brother);
    }
}

void translate_Dec(Node* Dec){
    if(debug) printf("translate_Dec\n");
    //Dec -> VarDec
        //| VarDec ASSIGNOP Exp
    //首先判断AST是否为空
    if(Dec == NULL) return;
    //接着判断AST结构是否正确
    assert(strcmp(Dec->name, "Dec") == 0);
    Operand var = translate_VarDec(Dec->child);
    if(Dec->child->brother == NULL){
        //Dec -> VarDec
    }
    else{
        //Dec -> VarDec ASSIGNOP Exp
        Operand t1 = new_temp();
        //code1
        t1 = translate_Exp(Dec->child->brother->brother,t1);
        //place := t1
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_ASSIGN;
        ic->u.assign.left = translate_VarDec(Dec->child);
        ic->u.assign.right = t1;
        add_codelist(ic);
    }
}

Operand translate_Exp(Node* Exp, Operand place){
    if(debug) printf("translate_Exp\n");
    //Exp -> ok Exp ASSIGNOP Exp
        //ok | Exp AND Exp
        //ok | Exp OR Exp
        //ok | Exp RELOP Exp
        //ok | Exp PLUS Exp
        //ok | Exp MINUS Exp
        //ok | Exp STAR Exp
        //ok | Exp DIV Exp
        //ok | LP Exp RP
        //ok | MINUS Exp
        //ok | NOT Exp
        //ok | ID LP Args RP
        //ok | ID LP RP
        //| Exp LB Exp RB
        // | Exp DOT ID
        //ok | ID
        //ok | INT
        //wu | FLOAT
    //首先判断AST是否为空
    if(Exp == NULL) return NULL;
    //接着判断AST结构是否正确 
    assert(strcmp(Exp->name, "Exp") == 0);
    if(strcmp(Exp->child->name, "ID") == 0 && Exp->child->brother == NULL){
        if(debug) printf("Exp -> ID\n");
        //Exp -> ID
        //variable = lookup(sym_table, ID)
        FieldList var = find(Exp->child->val.val_id);
        printf("val_id:%s var_no:%d\n",Exp->child->val.val_id,var->var_no);
        assert(var!=NULL);
        assert(place!=NULL);
        //place := variable
        Operand temp = find_op(var->var_no);
        if(temp != NULL){
            //place->u.varno = var->var_no;
            
            place = temp;
            return place;
        }
        else{
            if(var->var_no == -1) var->var_no = place->u.varno;
            else place->u.varno = var->var_no;
            place->symbolname = (char*)malloc(sizeof(char)*32);
            strcpy(place->symbolname,Exp->child->val.val_id);
        }
        if(var->type->kind == BASIC){
            place->kind = EM_VAR;
            place->type = var->type;
        }
        else if(var->type->kind == ARRAY){
            place->kind = EM_ARR;
            place->type = var->type;
        }
        else if(var->type->kind == STRUCTURE){
            printf("Cannot translate: Code contains variables or parameters of structure type.\n");
            structure_error = 1;
        }
    }
    else if(strcmp(Exp->child->name, "INT") == 0){
        if(debug) printf("Exp -> INT\n");
        //Exp -> INT
        if(place == NULL){
            printf("Error:place is NULL!\n");
        }
        /* InterCode ic = (InterCode)malloc(sizeof(struct InterCode_)); */
        /* ic->kind = IC_ASSIGN; */
        /* ic->u.assign.left = place; */
        /* ic->u.assign.right = new_constant(Exp->child->val.val_int); */
        /* add_codelist(ic); */
        place = new_constant(Exp->child->val.val_int);
    }
    else if(strcmp(Exp->child->name, "FLOAT") == 0){
        if(debug) printf("Exp -> FLOAT\n");
        //Exp -> FLOAT
        printf("Error: float type!\n");
    }
    else if(strcmp(Exp->child->name, "NOT") == 0 || strcmp(Exp->child->brother->name,"AND") == 0 || strcmp(Exp->child->brother->name,"OR") == 0 || strcmp(Exp->child->brother->name,"RELOP") == 0){
        if(debug) printf("Exp -> NOT/AND/OR/RELOP Exp\n");
        //| NOT Exp
        //| Exp AND Exp
        //| Exp OR Exp
        //| Exp RELOP Exp
        Operand label1 = new_label();
        Operand label2 = new_label();
        //code0 : code0 = [place := #0]
        /* InterCode ic0 = (InterCode)malloc(sizeof(struct InterCode_)); */
        /* ic0->kind = IC_ASSIGN; */
        /* ic0->u.assign.left = place; */
        /* ic0->u.assign.right = new_constant(0); */
        //简化，直接赋值
        place = new_constant(0);
        //code1 : code1 = translate_Cond(Exp, label1, label2, sym_table)
        translate_Cond(Exp, label1, label2);
        //code2 : code2 = [LABEL label1] + [place := #1]
        InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
        ic2->kind = IC_LABEL;
        ic2->u.one_op.op = label1;
        add_codelist(ic2);
        place = new_constant(1);
        //code3 : code3 = [LABEL label2]
        InterCode ic4 = (InterCode)malloc(sizeof(struct InterCode_));
        ic4->kind = IC_LABEL;
        ic4->u.one_op.op = label2;
        add_codelist(ic4);
    }
    else if(strcmp(Exp->child->name, "MINUS") == 0){
        if(debug) printf("Exp -> MINUS Exp\n");
        //Exp -> MINUS Exp
        Operand t1 = new_temp();
        //code1
        t1 = translate_Exp(Exp->child->brother, t1);
        //load t1，存疑
        //place := #0 - t1
        if(t1->kind == EM_CONSTANT){
            place = new_constant(0 - t1->u.val);
        }
        else{
            InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
            ic->kind = IC_SUB;
            ic->u.three_op.result = place;
            ic->u.three_op.op1 = new_constant(0);
            ic->u.three_op.op2 = t1;
            add_codelist(ic);
        }
    }
    else if(strcmp(Exp->child->brother->name, "ASSIGNOP") == 0){
        if(debug) printf("Exp -> Exp ASSIGNOP Exp\n");
        //Exp -> Exp ASSIGNOP Exp
        Operand t1 = new_temp();
        t1 = translate_Exp(Exp->child, t1);
        Operand t2 = new_temp();
        t2 = translate_Exp(Exp->child->brother->brother, t2);
        //把t2的值赋给t1
        if(t1->kind == EM_ARR && t2->kind == EM_ARR){
            //数组赋值
            copy_array(t1,t2);
            place = t2;
        }
        else{
            //正常变量赋值
            //把t2的值赋给t1
            InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
            ic1->kind = IC_ASSIGN;
            ic1->u.assign.left = t1;
            ic1->u.assign.right = t2;
            add_codelist(ic1);
            //place := t1,简化：不生成语句，直接赋值
            place = t2;
        }
    }
    else if(strcmp(Exp->child->brother->name, "PLUS") == 0 || strcmp(Exp->child->brother->name, "MINUS") == 0 || strcmp(Exp->child->brother->name, "STAR") == 0 || strcmp(Exp->child->brother->name, "DIV") == 0){
        if(debug) printf("Exp -> Exp PLUS Exp\n");
        //Exp -> Exp PLUS Exp
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        //code1
        t1 = translate_Exp(Exp->child, t1);
        //code2
        t2 = translate_Exp(Exp->child->brother->brother, t2);
        //place := t1 +  / - / * / / t2
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        if(strcmp(Exp->child->brother->name, "PLUS") == 0) ic->kind = IC_PLUS;
        else if(strcmp(Exp->child->brother->name, "MINUS") == 0) ic->kind = IC_SUB;
        else if(strcmp(Exp->child->brother->name, "STAR") == 0) ic->kind = IC_MUL;
        else if(strcmp(Exp->child->brother->name, "DIV") == 0) ic->kind = IC_DIV;
        ic->u.three_op.result = place;
        ic->u.three_op.op1 = t1;
        ic->u.three_op.op2 = t2;
        add_codelist(ic);
    }
    else if(strcmp(Exp->child->brother->name, "LB") == 0){
        if(debug) printf("Exp -> Exp LB Exp RB\n");
        //Exp LB Exp RB
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        Operand t3 = new_temp();
        //code1
        t1 = translate_Exp(Exp->child, t1);
        //fprintf(f2,"t1->u.varno:%d\n",t1->u.varno);
        int t1_typedepth = 1;
        Type t1_type = t1->type;
        assert(t1_type->kind == ARRAY);
        //fprintf(f2,"t1_type->u.array.size:%d\n",t1_type->u.array.size);
        Type temp = t1_type->u.array.elem;
        //fprintf(f2,"temp->u.array_size:%d\n",temp->u.array.size);
        place->type = temp;
        int size = 1;
        while(temp->kind == ARRAY){
            t1_typedepth++;
            size *= temp->u.array.size;
            temp = temp->u.array.elem;
        }
        //if(t1_typedepth <= 1) place->kind = EM_VAR;
        //else{
        //    place->kind = EM_ADDRESS; 
        //    place ->type = t1_type->u.array.elem;
        //}
        printf("t1->kind:%d\n",t1->kind);
        //code2
        t2 = translate_Exp(Exp->child->brother->brother, t2);
        printf("t2->kind:%d\n",t2->kind);
        //code3
        t1 = get_address(t1);
        if(t2->kind == EM_CONSTANT){
            //t3 := t1 + t2*4*size
            InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
            ic->kind = IC_PLUS;
            ic->u.three_op.result = t3;
            if(t1->kind == EM_ADDRESS || t1->kind == EM_ARR){
                Operand tempp = new_temp();
                tempp->u.varno = t1->u.varno;
                ic->u.three_op.op1 = tempp;
            }
            else{
                ic->u.three_op.op1 = t1;
            }
            ic->u.three_op.op2 = new_constant(t2->u.val * 4 * size);
            add_codelist(ic);
            //place := *t3
            place->kind = EM_ADDRESS;
            place->u.varno = t3->u.varno;
            /* InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_)); */
            /* ic1->kind = IC_GETVALUE; */
            /* ic1->u.assign.left = place; */
            /* ic1->u.assign.right = t3; */
            /* add_codelist(ic1); */
        }
        else{
            //t3 := t2 * 4 * size
            InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
            ic1->kind = IC_MUL;
            ic1->u.three_op.result = t3;
            ic1->u.three_op.op1 = t2;
            ic1->u.three_op.op2 = new_constant(4*size);
            add_codelist(ic1);
            //t3 := t1 + t3
            InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
            ic2->kind = IC_PLUS;
            ic2->u.three_op.result = t3;
            if(t1->kind == EM_ADDRESS || t1->kind == EM_ARR){
                Operand tempp = new_temp();
                tempp->u.varno = t1->u.varno;
                ic2->u.three_op.op1 = tempp;
            }
            else{
                ic2->u.three_op.op1 = t1;
            }
            ic2->u.three_op.op2 = t3;
            add_codelist(ic2);
            //place := *t3
            place->kind = EM_ADDRESS;
            place->u.varno = t3->u.varno;
            /* InterCode ic3 = (InterCode)malloc(sizeof(struct InterCode_));
            ic3->kind = IC_GETVALUE;
            ic3->u.assign.left = place;
            ic3->u.assign.right = t3;
            add_codelist(ic3); */
        }
    }
    else if(strcmp(Exp->child->name, "LP") == 0){
        if(debug) printf("Exp -> LP Exp RP\n");
        //Exp -> LP Exp RP
        place = translate_Exp(Exp->child->brother, place);
    }
    else if(strcmp(Exp->child->name, "Exp") == 0 && strcmp(Exp->child->brother->name, "DOT") == 0){
        if(debug) printf("Exp -> Exp DOT ID\n");
        printf("Cannot translate: Code contains variables or parameters of structure type.\n");
        structure_error = 1;
    }
    else if(strcmp(Exp->child->name, "ID") == 0 && strcmp(Exp->child->brother->name, "LP") == 0){
        //Exp -> ID LP Args RP
        //| ID LP RP
        FieldList f = find(Exp->child->val.val_id);
        assert(f != NULL);
        if(strcmp(Exp->child->brother->brother->name, "Args") == 0){
            if(debug) printf("Exp -> ID LP Args RP\n");
            //Exp -> ID LP Args RP
            //Args -> Exp COMMA Args
            ArgList arg = (ArgList)malloc(sizeof(struct ArgList_));
            if(strcmp(f->name,"write") == 0){
                translate_Args(Exp->child->brother->brother,arg,1);
                //WRITE arg_list[1]
                /* InterCode ic0 = (InterCode)malloc(sizeof(struct InterCode_)); */
                /* ic0->kind = IC_WRITE; */
                /* ic0->u.one_op.op = arg->args; */
                /* add_codelist(ic0); */
                //place := #0
                /* InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_)); */
                /* ic2->kind = IC_ASSIGN; */
                /* ic2->u.assign.left = place; */
                /* ic2->u.assign.right = new_constant(0); */
                /* add_codelist(ic2); */
                //不写成语句，直接赋值
                place = new_constant(0);
                return place;
            }
            else{
                translate_Args(Exp->child->brother->brother,arg,0);
                /* while(arg != NULL){
                    //ARG arg_list[i]
                    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
                    ic->kind = IC_ARG;
                    ic->u.one_op.op = arg->args;
                    add_codelist(ic);
                    arg = arg->next;
                } */
                //[place := CALL function.name] 
                InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
                ic->kind = IC_CALL;
                ic->u.assign.left = place;
                Operand op = (Operand)malloc(sizeof(struct Operand_));
                op->kind = EM_FUNC;
                op->u.funcname = f->name;
                ic->u.assign.right = op;
                add_codelist(ic);
            }
        }
        else{
            if(debug) printf("Exp -> ID LP RP\n");
            //Exp -> ID LP RP
            if(strcmp(f->name,"read") == 0){
                InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
                ic->kind = IC_READ;
                ic->u.one_op.op = place;
                add_codelist(ic);
            }
            else{
                //place := CALL f.name
                InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
                ic->kind = IC_CALL;
                ic->u.assign.left = place;
                Operand op = (Operand)malloc(sizeof(struct Operand_));
                op->kind = EM_FUNC;
                op->u.funcname = f->name;
                ic->u.assign.right = op;
                add_codelist(ic);
            }
        }
    }
    return place;
}

void translate_Cond(Node* Exp, Operand label_true, Operand label_false){
    if(debug) printf("translate_Cond\n");
    if(strcmp(Exp->child->name,"NOT") == 0){
        translate_Cond(Exp->child->brother, label_false, label_true);
    }
    /* else if(strcmp(Exp->child->name,"INT")==0 || strcmp(Exp->child->name,"ID") == 0 ){
        //Exp -> ID
        //| INT
        if(debug) printf("translate_Cond:ID/INT\n");
        Operand t1 = new_temp();
        //code1
        t1 = translate_Exp(Exp, t1);
        //code2
        //IF t1 RELOP #0 GOTO label_true
        printf('aaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbb\n');
        InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
        ic1->kind = IC_IFGOTO;
        ic1->u.ifgoto.op1 = t1;
        ic1->u.ifgoto.op2 = new_constant(0);
        ic1->u.ifgoto.relop = (char *)malloc(50 * sizeof(char));
        ic1->u.ifgoto.relop = "!=";
        ic1->u.ifgoto.label = label_true;
        add_codelist(ic1);
        //GOTO label_false
        InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
        ic2->kind = IC_GOTO;
        printf("translate_Cond1 label_id:%d\n",label_false->u.varno);
        ic2->u.one_op.op = label_false;
        add_codelist(ic2);
    } */
    else if(strcmp(Exp->child->name,"INT")==0 || strcmp(Exp->child->name,"ID") == 0 || strcmp(Exp->child->brother->name,"RELOP") == 0){
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        //code1
        if(strcmp(Exp->child->name,"INT")==0 || strcmp(Exp->child->name,"ID") == 0 ){
            t1 = translate_Exp(Exp, t1);
            t2 = new_constant(0);
        }
        else{
            t1 = translate_Exp(Exp->child, t1);
            t2 = translate_Exp(Exp->child->brother->brother, t2);
        }
        //code2
        //IF t1 RELOP t2 GOTO label_true
        InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
        ic1->kind = IC_IFGOTO;
        ic1->u.ifgoto.op1 = t1;
        ic1->u.ifgoto.op2 = t2;
        //ic1->u.ifgoto.relop = (char *)malloc(50 * sizeof(char));
        if(strcmp(Exp->child->name,"INT")==0 || strcmp(Exp->child->name,"ID") == 0 ){
            ic1->u.ifgoto.relop = (char *)malloc(50 * sizeof(char));
            ic1->u.ifgoto.relop = "!=";
        }
        else{
            ic1->u.ifgoto.relop = Exp->child->brother->val.val_id;
        }
        ic1->u.ifgoto.label = label_true;
        add_codelist(ic1);
        //GOTO label_false
        InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
        ic2->kind = IC_GOTO;
        printf("translate_Cond2 label_id:%d\n",label_false->u.varno);
        ic2->u.one_op.op = label_false;
        add_codelist(ic2);
    }
    else if(strcmp(Exp->child->brother->name,"AND") == 0){
        Operand label1 = new_label();
        //code1
        translate_Cond(Exp->child, label1, label_false);
        //Label label1
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_LABEL;
        ic->u.one_op.op = label1;
        add_codelist(ic);
        //code2
        translate_Cond(Exp->child->brother->brother, label_true, label_false);
    }
    else if(strcmp(Exp->child->brother->name,"OR") == 0){
        Operand label1 = new_label();
        //code1
        translate_Cond(Exp->child, label_true, label1);
        //Label label1
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_LABEL;
        ic->u.one_op.op = label1;
        add_codelist(ic);
        //code2
        translate_Cond(Exp->child->brother->brother, label_true, label_false);
    }
}

void translate_Args(Node* Args, ArgList arg_list,int write_flag){
    if(debug) printf("translate_Args\n");
    //Args -> Exp COMMA Args
        //| Exp
    // 返回时，根据函数参数依次访问判断，是取地址还是取值，只有数组和结构体要取地址
    // write函数的参数为NULL，要避免空指针访问
    // fdom指向函数形参列表
    if(Args->child->brother == NULL){
        //Args -> Exp
        arg_list->next = NULL;
    }
    else{
        //Args -> Exp COMMA Args
        arg_list->next = (ArgList)malloc(sizeof(struct ArgList_));
        translate_Args(Args->child->brother->brother,arg_list->next,write_flag);
    }
    Operand t1 = new_temp();
    t1 = translate_Exp(Args->child, t1);
    Operand t2 = new_temp();
    //t1 = copy_operand(t1);
    arg_list->args = t1;
    if(write_flag == 1){
        assert(Args->child->brother == NULL);
        //write函数的参数为NULL，要避免空指针访问
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_WRITE;
        ic->u.one_op.op = t1;
        add_codelist(ic);
        return;
    }
    if(t1->kind == EM_ARR){
        //数组作为参数，传递地址
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_GETADDR;
        ic->u.assign.left = t2;
        ic->u.assign.right = t1;
        add_codelist(ic);
        arg_list->args = t2;
        InterCode ic_a = (InterCode)malloc(sizeof(struct InterCode_));
        ic_a->kind = IC_ARG;
        ic_a->u.one_op.op = t2;
        add_codelist(ic_a);
        return;
    }
    else if(t1->kind == EM_ADDRESS){
        //地址作为参数，传递地址
        if(t1->type == NULL){
            //参数是数值,存疑
        }
        else if(t1->type->kind == BASIC){
        }
        else{
            printf("Error: No array whose dimension is more than 1 as arg\n");
        }
    }
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    ic->kind = IC_ARG;
    ic->u.one_op.op = t1;
    add_codelist(ic);
    return;
}