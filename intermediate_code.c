#include "intermediate_code.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define debug 1

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
            fprintf(f,"Error: Intercode kind error!\n");
            break;
    }        
}

void print_IR(FILE* f){
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
    op->u.varno = temp_num++;
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
    return op;
}

Operand new_constant(int val){
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = EM_CONSTANT;
    op->u.val = val;
    return op;
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
    temp_num = 1;
    //遍历AST，一次调用translate_X函数
    Node* node = root->child; // Program -> ExtDefList
    while(node != NULL){
        node = node->child; // ExtDefList -> ExtDef ExtDefList
        translate_ExtDef(node);
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
    //检查是否为";""
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
            return NULL;
        }
        res = new_var();
        var->var_no = res->u.varno;
    }
    else{
        //VarDec -> VarDec LB INT RB
        res = translate_VarDec(VarDec->child);
        //DEC res child->brother->brother->val.val_int
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_DEC;
        ic->u.dec.op = res;
        ic->u.dec.size = VarDec->child->brother->brother->val.val_int * 4;
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
            Operand op = new_var();
            InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
            ic2->kind = IC_PARAM;
            ic2->u.one_op.op = op;
            add_codelist(ic2);
            param = param->next;
        }
        printf("shizaizhelima\n");
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
    translate_DefList(CompSt->child->brother);
    translate_StmtList(CompSt->child->brother->brother);
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
        translate_Exp(Stmt->child,t1);
    }
    else if(strcmp(Stmt->child->name, "CompSt") == 0){
        //Stmt -> CompSt
        translate_CompSt(Stmt->child);
    }
    else if(strcmp(Stmt->child->name, "RETURN") == 0){
        //Stmt -> RETURN Exp SEMI
        Operand t1 = new_temp();
        //code1
        translate_Exp(Stmt->child->brother,t1);
        //RETURN t1
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_RETURN;
        ic->u.one_op.op = t1;
        add_codelist(ic);
    }
    else if(strcmp(Stmt->child->name, "IF") == 0){
        if(Stmt->child->brother->brother->brother->brother->brother != NULL){
            //Stmt -> IF LP Exp RP Stmt ELSE Stmt
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
            InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
            ic2->kind = IC_GOTO;
            ic2->u.one_op.op = label3;
            add_codelist(ic2);
            //Label label2
            InterCode ic3 = (InterCode)malloc(sizeof(struct InterCode_));
            ic3->kind = IC_LABEL;
            ic3->u.one_op.op = label2;
            add_codelist(ic3);
            //code3
            translate_Stmt(Stmt->child->brother->brother->brother->brother->brother->brother);
            //Label label3
            InterCode ic4 = (InterCode)malloc(sizeof(struct InterCode_));
            ic4->kind = IC_LABEL;
            ic4->u.one_op.op = label3;
            add_codelist(ic4);
        }
        else{
            //Stmt -> IF LP Exp RP Stmt
            Operand label1 = new_label();
            Operand label2 = new_label();
            //code1
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
        translate_Exp(Dec->child->brother->brother,t1);
        //place := t1
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_ASSIGN;
        ic->u.assign.left = translate_VarDec(Dec->child);
        ic->u.assign.right = t1;
        add_codelist(ic);
    }
}

void translate_Exp(Node* Exp, Operand place){
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
    if(Exp == NULL) return;
    //接着判断AST结构是否正确
    assert(strcmp(Exp->name, "Exp") == 0);
    if(strcmp(Exp->child->name, "ID") == 0 && Exp->child->brother == NULL){
        if(debug) printf("Exp -> ID\n");
        //Exp -> ID
        //variable = lookup(sym_table, ID)
        if(debug) printf("%s\n",Exp->child->val.val_id);
        FieldList var = find(Exp->child->val.val_id);
        if(var == NULL){
            printf("Error: undefined variable!\n");
            return;
        }
        if(place == NULL){
            printf("Error:place is NULL!\n");
        }
        else{
            //place := variable
            if(var->type->kind == BASIC){
                place->kind = EM_VAR;
                place->u.varno = var->var_no;//存疑
            }
            else{
                //array
                //Intercode ic = (InterCode)malloc(sizeof(struct InterCode_));
                //ic->kind = IC_GETADDR;
            }
        }
    }
    else if(strcmp(Exp->child->name, "INT") == 0){
        if(debug) printf("Exp -> INT\n");
        //Exp -> INT
        if(place == NULL){
            printf("Error:place is NULL!\n");
        }
        place->kind = EM_CONSTANT;
        place->u.val = Exp->child->val.val_int;
        if(debug) printf("%d\n",Exp->child->val.val_int);
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
        place = new_constant(1);
        //code3 : code3 = [LABEL label2]
        InterCode ic4 = (InterCode)malloc(sizeof(struct InterCode_));
        ic4->kind = IC_LABEL;
        ic4->u.one_op.op = label2;
    }
    else if(strcmp(Exp->child->name, "MINUS") == 0){
        if(debug) printf("Exp -> MINUS Exp\n");
        //Exp -> MINUS Exp
        Operand t1 = new_temp();
        //code1
        translate_Exp(Exp->child->brother, t1);
        //load t1，存疑
        //place := #0 - t1
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_SUB;
        ic->u.three_op.result = place;
        ic->u.three_op.op1 = new_constant(0);
        ic->u.three_op.op2 = t1;
        add_codelist(ic);
    }
    else if(strcmp(Exp->child->brother->name, "ASSIGNOP") == 0){
        if(debug) printf("Exp -> Exp ASSIGNOP Exp\n");
        //Exp -> Exp ASSIGNOP Exp
        Operand t1 = new_temp();
        translate_Exp(Exp->child, t1);
        Operand t2 = new_temp();
        translate_Exp(Exp->child->brother->brother, t2);
        //把t2的值赋给t1
        if(t1->kind == EM_ADDRESS || t1->kind == EM_ARR){
            //TODO
            if(t2->kind == EM_ADDRESS || t2->kind == EM_ARR){
                //把t2的值赋给t1，数组或者地址形式（即：*a1 = *a2 或 *a1 = v 或 a1 = a2(数组））
            }
            else{
            }
        }
        else{
            //正常变量赋值
            //把t2的值赋给t1
            InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
            ic1->kind = IC_ASSIGN;
            ic1->u.assign.left = t1;
            ic1->u.assign.right = t2;
            fprintf(f2,"left:%d;right:%d",t1->u.varno,t2->u.varno);
            add_codelist(ic1);
            //place := t1,简化：不生成语句，直接赋值
            place = t2;
        }
    }
    else if(strcmp(Exp->child->brother->name, "PLUS") == 0){
        if(debug) printf("Exp -> Exp PLUS Exp\n");
        //Exp -> Exp PLUS Exp
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        //code1
        translate_Exp(Exp->child, t1);
        //code2
        translate_Exp(Exp->child->brother->brother, t2);
        //place := t1 + t2
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_PLUS;
        ic->u.three_op.result = place;
        ic->u.three_op.op1 = t1;
        ic->u.three_op.op2 = t2;
        add_codelist(ic);
    } 
    else if(strcmp(Exp->child->brother->name, "MINUS") == 0){
        if(debug) printf("Exp -> Exp MINUS Exp\n");
        //Exp MINUS Exp
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        //code1
        translate_Exp(Exp->child, t1);
        //code2
        translate_Exp(Exp->child->brother->brother, t2);
        //place := t1 - t2
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_SUB;
        ic->u.three_op.result = place;
        ic->u.three_op.op1 = t1;
        ic->u.three_op.op2 = t2;
        add_codelist(ic);
    } 
    else if(strcmp(Exp->child->brother->name, "STAR") == 0){
        if(debug) printf("Exp -> Exp STAR Exp\n");
        //Exp STAR Exp
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        //code1
        translate_Exp(Exp->child, t1);
        //code2
        translate_Exp(Exp->child->brother->brother, t2);
        //place := t1 * t2
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_MUL;
        ic->u.three_op.result = place;
        ic->u.three_op.op1 = t1;
        ic->u.three_op.op2 = t2;
        add_codelist(ic);
    } 
    else if(strcmp(Exp->child->brother->name, "DIV") == 0){
        if(debug) printf("Exp -> Exp DIV Exp\n");
        //Exp DIV Exp
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        //code1
        translate_Exp(Exp->child, t1);
        //code2
        translate_Exp(Exp->child->brother->brother, t2);
        //place := t1 / t2
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_DIV;
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
        translate_Exp(Exp->child, t1);
        //code2
        translate_Exp(Exp->child->brother->brother, t2);
        //code3
        InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
        ic1->kind = IC_MUL;
        ic1->u.three_op.result = t3;
        ic1->u.three_op.op1 = t2;
        ic1->u.three_op.op2 = new_constant(4);
        add_codelist(ic1);
        //code4
        InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
        ic2->kind = IC_PLUS;
        ic2->u.three_op.result = t3;
        ic2->u.three_op.op1 = t1;
        ic2->u.three_op.op2 = t3;
        add_codelist(ic2);
        //place := *t3
        InterCode ic3 = (InterCode)malloc(sizeof(struct InterCode_));
        ic3->kind = IC_GETVALUE;
        ic3->u.assign.left = place;
        ic3->u.assign.right = t3;
        add_codelist(ic3);
    }
    else if(strcmp(Exp->child->name, "LP") == 0){
        if(debug) printf("Exp -> LP Exp RP\n");
        //Exp -> LP Exp RP
        translate_Exp(Exp->child->brother, place);
    }
    else if(strcmp(Exp->child->name, "Exp") == 0 && strcmp(Exp->child->brother->name, "DOT") == 0){
        if(debug) printf("Exp -> Exp DOT ID\n");
        //Exp -> Exp DOT ID
        //TODO
    }
    else if(strcmp(Exp->child->name, "ID") == 0 && strcmp(Exp->child->brother->name, "LP") == 0){
        //Exp -> ID LP Args RP
        //| ID LP RP
        FieldList f = find(Exp->child->val.val_id);
        if(f == NULL){
            printf("Error: undefined function!\n");
            return;
        }
        if(strcmp(Exp->child->brother->brother->name, "Args") == 0){
            if(debug) printf("Exp -> ID LP Args RP\n");
            //Exp -> ID LP Args RP
            //Args -> Exp COMMA Args
            ArgList arg = (ArgList)malloc(sizeof(struct ArgList_));
            if(strcmp(f->name,"write") == 0){
                translate_Args(Exp->child->brother->brother,arg,1);
                //WRITE arg_list[1]
                InterCode ic0 = (InterCode)malloc(sizeof(struct InterCode_));
                ic0->kind = IC_WRITE;
                ic0->u.one_op.op = arg->args;
                fprintf(f2,"%d\n",arg->args->u.varno);
                add_codelist(ic0);
                //place := #0
                /* InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_)); */
                /* ic2->kind = IC_ASSIGN; */
                /* ic2->u.assign.left = place; */
                /* ic2->u.assign.right = new_constant(0); */
                /* add_codelist(ic2); */
                //不写成语句，直接赋值
                place = new_constant(0);
                return;
            }
            else{
                translate_Args(Exp->child->brother->brother,arg,0);
                while(arg != NULL){
                    //ARG arg_list[i]
                    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
                    ic->kind = IC_ARG;
                    ic->u.one_op.op = arg->args;
                    add_codelist(ic);
                    arg = arg->next;
                }
                //place := CALL f.name
                InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
                ic2->kind = IC_CALL;
                ic2->u.assign.left = place;
                ic2->u.assign.right = f->name;
                //code2 [place := CALL function.name] function.name怎么表示，存疑
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
}

void translate_Cond(Node* Exp, Operand label_true, Operand label_false){
    if(debug) printf("translate_Cond\n");
    if(strcmp(Exp->child->name,"NOT") == 0){
        translate_Cond(Exp->child->brother, label_false, label_true);
    }
    else if(Exp->child->brother == NULL){
        //Exp -> ID
        //| INT
        Operand t1 = new_temp();
        //code1
        translate_Exp(Exp, t1);
        //code2
        //IF t1 RELOP #0 GOTO label_true
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
    else if(strcmp(Exp->child->brother->name,"RELOP") == 0){
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        //code1
        translate_Exp(Exp->child, t1);
        //code2
        translate_Exp(Exp->child->brother->brother, t2);
        //IF t1 RELOP t2 GOTO label_true
        InterCode ic1 = (InterCode)malloc(sizeof(struct InterCode_));
        ic1->kind = IC_IFGOTO;
        ic1->u.ifgoto.op1 = t1;
        ic1->u.ifgoto.op2 = t2;
        ic1->u.ifgoto.relop = (char *)malloc(50 * sizeof(char));
        printf("relop: %s\n",Exp->child->brother->val.val_id);
        strcpy(ic1->u.ifgoto.relop, Exp->child->brother->val.val_id);
        ic1->u.ifgoto.label = label_true;
        add_codelist(ic1);
        //GOTO label_false
        InterCode ic2 = (InterCode)malloc(sizeof(struct InterCode_));
        ic2->kind = IC_GOTO;
        ic2->u.one_op.op = label_false;
        add_codelist(ic2);
    }
}

void translate_Args(Node* Args, ArgList arg_list,int write_flag){
    if(debug) printf("translate_Args\n");
    //Args -> Exp COMMA Args
        //| Exp
    // 返回时，根据函数参数依次访问判断，是取地址还是取值，只有数组和结构体要取地址
    // write函数的参数为NULL，要避免空指针访问
    // fdom指向函数形参列表
    Operand t1 = new_temp();
    translate_Exp(Args->child, t1);
    arg_list->args = t1;
    arg_list->next = NULL;
    if(write_flag == 1){
        assert(Args->child->brother == NULL);
        //write函数的参数为NULL，要避免空指针访问
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_WRITE;
        ic->u.one_op.op = t1;
        return;
    }
    if(t1->kind == EM_STRUCT) printf("Error: No struct as arg\n");
    else if(t1->kind == EM_ARR){
        //数组作为参数，传递地址
        InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
        ic->kind = IC_GETADDR;
        ic->u.assign.left = t1;
        ic->u.assign.right = t1;
        add_codelist(ic);
    }
    else if(t1->kind == EM_ADDRESS){
        //地址作为参数，传递地址
        if(t1->type == NULL){
            //参数是数值,存疑
            InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
            ic->kind = IC_GETVALUE;
            ic->u.assign.left = t1;
            ic->u.assign.right = t1;
        }
        else if(t1->type->kind == BASIC){
            //存疑
        }
        else{
            printf("Error: No array whose dimension is more than 1 as arg\n");
        }
    }
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    ic->kind = IC_ARG;
    ic->u.one_op.op = t1;
    if(Args->child->brother == NULL){
        //Args -> Exp

    }
    else{
        //Args -> Exp COMMA Args
        arg_list->next = (ArgList)malloc(sizeof(struct ArgList_));
        translate_Args(Args->child->brother->brother,arg_list->next,write_flag);
    }
}