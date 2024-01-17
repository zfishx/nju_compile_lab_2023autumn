#include "semantic.h"
#include <assert.h>

#define debug 0

void print_error_semantic(int errtype, int line, char* msg){
    printf("Error type %d at Line %d: %s.\n",errtype,line,msg);
    return;
}

void semantic_check(Node* root){
    //初始化工作，初始化符号表
    init_symbol_table();
    //添加lab3中的read,write函数
    FieldList read_f = (FieldList)malloc(sizeof(FieldList_));
    read_f->name = "read";
    read_f->type = (Type)malloc(sizeof(Type_));
    read_f->type->kind = FUNCTION;
    read_f->type->u.function = (Function)malloc(sizeof(Function_));
    read_f->type->u.function->name = "read";
    read_f->type->u.function->param = NULL;
    read_f->type->u.function->ret = (Type)malloc(sizeof(Type_));
    read_f->type->u.function->ret->kind = BASIC;
    read_f->type->u.function->ret->u.basic = 0;
    read_f->type->u.function->ret->assign = RIGHT;
    read_f->tail = NULL;
    insert(read_f);
    FieldList write_f = (FieldList)malloc(sizeof(FieldList_));
    write_f->name = "write";
    write_f->type = (Type)malloc(sizeof(Type_));
    write_f->type->kind = FUNCTION;
    write_f->type->u.function = (Function)malloc(sizeof(Function_));
    write_f->type->u.function->name = "write";
    write_f->type->u.function->param = (FieldList)malloc(sizeof(FieldList_));//未知待定
    write_f->type->u.function->ret = (Type)malloc(sizeof(Type_));
    write_f->type->u.function->ret->kind = BASIC;
    write_f->type->u.function->ret->u.basic = 0;
    write_f->type->u.function->ret->assign = RIGHT;
    write_f->tail = NULL;
    insert(write_f);
    //遍历语法树中的ExtDefList节点
    ExtDefList(root->child);
}

int type_equal(Type a, Type b){
    //Todo:判断两个类型是否相等
    if(debug) printf("type_equal\n");

    if(a == NULL && b == NULL){
        return 1;
    }
    if(a == NULL || b == NULL){
        return 0;
    }
    if(debug) printf("a: %d\n",a->kind);
    if(debug) printf("b: %d\n",b->kind);
    if(a -> kind == BASIC){
        if(b -> kind != BASIC){
            return 0;
        }
        if(a -> u.basic == b -> u.basic){
            if(debug) printf("type_equal: a -> u.basic == b -> u.basic\n");
            return 1;
        }
        else{
            return 0;
        }
    }
    if(a -> kind == ARRAY){
        if(b -> kind != ARRAY){
            return 0;
        }
        return type_equal(a -> u.array.elem, b -> u.array.elem);
    }
    if(a -> kind == STRUCTURE){
        if(b -> kind != STRUCTURE){
            return 0;
        }
        FieldList a_f = a -> u.structure;
        FieldList b_f = b -> u.structure;
        while(a_f != NULL && b_f != NULL){
            if(!type_equal(a_f -> type, b_f -> type)){
                return 0;
            }
            a_f = a_f -> tail;
            b_f = b_f -> tail;
        }
        if(a_f == NULL && b_f == NULL){
            return 1;
        }
        else{
            return 0;
        }
        return 1;
    }
    //return 0;
}
int param_equal(FieldList p1,FieldList p2){
    //Todo:判断两个参数是否相等
    while (p1 != NULL && p2 != NULL) {
        if (!type_equal(p1->type, p2->type)) return 0; // 参数类型不一致
        p1 = p1->tail;
        p2 = p2->tail; // 获取下一个参数
    }
    if (p1 == NULL && p2 == NULL) return 1; // 检查结束，参数类型一致
    return 0; // 默认类型不一致
}

void Program(Node* node){
    
}

void ExtDefList(Node* node){
    if (node == NULL) {
        return;
    }
    if(debug) printf("ExtDefList:%s\n",node->name);
    //遍历第一个节点
    ExtDef(node->child);
    //遍历兄弟节点
    ExtDefList(node->child->brother);
}

void ExtDef(Node* node){
    if (node == NULL) {
        return;
    }
    if(debug) printf("ExtDef:%s\n",node->name);
    assert(node->child != NULL);
    assert(node->child->brother != NULL);
    //遍历第一个节点
    Type t = Specifier(node->child);
    //遍历第二个节点
    if (strcmp(node->child->brother->name, "ExtDecList") == 0) {
        //ExtDef -> Specifier ExtDecList SEMI
        ExtDecList(node->child->brother,t);
    } 
    else if (strcmp(node->child->brother->name, "SEMI") == 0) {
        //ExtDef -> Specifier SEMI
        //do nothing
    } 
    else if (strcmp(node->child->brother->name, "FunDec") == 0) {
        //ExtDef -> Specifier FunDec CompSt
        FunDec(node->child->brother,t);
        CompSt(node->child->brother->brother,t);
    }
}

void ExtDecList(Node* node, Type type){ 
    if (node == NULL) {
        return;
    }
    if(debug) printf("ExtDecList:%s\n",node->name);
    //ExtDecList -> VarDec
    //           | VarDec COMMA ExtDecList
    FieldList f = VarDec(node->child,type,0);
    if(node->child->brother == NULL){
        //ExtDecList -> VarDec
    }
    else{
        //ExtDecList -> VarDec COMMA ExtDecList
        ExtDecList(node->child->brother->brother,type);
    }
}

Type Specifier(Node* node){
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("Specifier:%s\n",node->name);
    assert(node->child != NULL);
    if (strcmp(node->child->name, "TYPE") == 0) {
        //Specifier -> TYPE
        Type t = (Type)malloc(sizeof(Type_));
        Node* type = node->child;
        t->kind = BASIC;
        //printf("%s\n",type->val.val_id);
        if (strcmp(type->val.val_id, "int") == 0 || strcmp(type->val.val_id, "INT") == 0) {
            t->u.basic = 0;
        } 
        else if (strcmp(type->val.val_id, "float") == 0) {
            t->u.basic = 1;
        }
        return t;
    } 
    else if (strcmp(node->child->name, "StructSpecifier") == 0) {
        //Specifier -> StructSpecifier
        return StructSpecifier(node->child);
    }
}

Type StructSpecifier(Node* node){
    //node是StructSpecifier节点
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("StructSpecifier:%s\n",node->name);
    assert(node->child != NULL);
    assert(strcmp(node->child->name ,"STRUCT") == 0);
    FieldList f = NULL;
    if (strcmp(node->child->brother->name, "OptTag") == 0) {
        //StructSpecifier -> STRUCT OptTag LC DefList RC 
        //OptTag -> ID
        Node* opttag = node->child->brother;//可选的，能不能为空，还未处理，存疑
        Node* id = opttag->child;
        FieldList f = find(id->val.val_id);
        if(f != NULL){
            print_error_semantic(16,id->line,"Duplicated define for struct");
            return NULL;
        }
        else{
            f = (FieldList)malloc(sizeof(FieldList_));
            f->name = id->val.val_id;
            //printf("StructSpecifier:%s\n",f->name);
            f->type = (Type)malloc(sizeof(Type_));
            f->type->kind = STRUCTURE;
            f->tail = NULL;
            insert(f);
            //f->type->u.structure = (Structure)malloc(sizeof(Structure));
            //f->type->u.structure->name = id->val;
            //f->type->u.structure->next = NULL;
            //f->type->u.structure->tail = NULL;

            //DefList -> Def DefList
            Node* deflist = opttag->brother->brother;
            FieldList def_f = DefList(deflist,1);
            f->type->u.structure = def_f;
            return f->type;
        }
    }
    else if (strcmp(node->child->brother->name, "Tag") == 0) {
        //StructSpecifier -> STRUCT Tag
        //判断该结构体是否在符号表中
        //Tag -> ID 
        Node* tag = node->child->brother;
        Node* id = tag->child;
        FieldList f = find(id->val.val_id);
        if(f == NULL){
            print_error_semantic(17,id->line,"Undefined struct");
            return NULL;
        }
        else if(f->type==NULL || f->type->kind != STRUCTURE){
            print_error_semantic(17,id->line,"Not a struct");
            return NULL;
        }
        else{
            return f->type;
        }
    }
    else if(strcmp(node->child->brother->name, "LC") == 0){
        Type res = (Type)malloc(sizeof(Type_));
        res->kind = STRUCTURE;
        Node* deflist = node->child->brother->brother;
        FieldList def_f = DefList(deflist,1);
        res->u.structure = def_f;
        return res;
    }
}

FieldList DefList(Node* node,int structflag){
    //node是DefList节点
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("DefList:%s\n",node->name);
    //DefList -> Def DefList
    //        | ε
    if(node->child == NULL){
        return NULL;
    }
    FieldList f = Def(node->child,structflag);
    if(node->child->brother == NULL) return f;
    FieldList f2 = DefList(node->child->brother,structflag);
    if(f == NULL){
        return f2;
    }
    if(f2 == NULL){
        return f;
    }
    else{
        FieldList f_cur = f;
        while(f_cur->tail != NULL){
            f_cur = f_cur->tail;
        }
        f_cur->tail = f2;
        return f;
    }
}

FieldList Def(Node* node,int structflag){
    //node是Def节点
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("Def:%s\n",node->name);
    //Def -> Specifier DecList SEMI
    Type t = Specifier(node->child);
    FieldList f = DecList(node->child->brother,t,structflag);
    return f;
}

FieldList DecList(Node* node, Type type, int structflag){
    //node是DecList节点
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("DecList:%s\n",node->name);
    //DecList -> Dec
    //        | Dec COMMA DecList
    FieldList f = Dec(node->child,type,structflag);
    if(node->child->brother == NULL){
        //DecList -> Dec
    }
    else{
        //DecList -> Dec COMMA DecList
        FieldList f2 = DecList(node->child->brother->brother,type,structflag);
        if(f2 == NULL){
            return f;
        }
        else{
            FieldList f_cur = f;
            while(f_cur->tail != NULL){
                f_cur = f_cur->tail;
            }
            f_cur->tail = f2;
        }
    }
    return f;
}

FieldList Dec(Node* node, Type type, int structflag){
    //node是Dec节点
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("Dec:%s\n",node->name);
    //Dec -> VarDec
    //    | VarDec ASSIGNOP Exp
    assert(strcmp(node->child->name,"VarDec")==0);
    FieldList f = VarDec(node->child,type,structflag);
    if(node->child->brother == NULL){
        //Dec -> VarDec
    }
    else{
        if(structflag){
            print_error_semantic(15,node->child->line,"Initialize field in struct");
            return f;
        }
        //Dec -> VarDec ASSIGNOP Exp
        //检查赋值类型是否匹配
        Type t = Exp(node->child->brother->brother);
        if(f == NULL){
            return NULL;
        }
        if(!type_equal(f->type,t)){
            print_error_semantic(5,node->child->brother->brother->line,"Dec:The expression types on both sides of the assignment sign do not match");
        }
    }
    return f;
}

FieldList VarDec(Node* node, Type type, int structflag){
    //node是VarDec节点
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("VarDec:%s\n",node->name);
    //VarDec -> ID
    //        | VarDec LB INT RB
    if(node->child->brother == NULL){
        //VarDec -> ID
        Node* id = node->child;
        assert(strcmp(id->name,"ID")==0);
        FieldList f = find(id->val.val_id);

        if(f != NULL){
            if(structflag){
                //判断结构体中域名重复定义
                print_error_semantic(15,id->line,"Redefined field in struct");
                return f;
            }
            print_error_semantic(3,id->line,"Redefined variable");
            return f;
        }
        else{
            f = (FieldList)malloc(sizeof(FieldList_));
            f->name = id->val.val_id;
            f->type = type;
            /* if(f->type->kind == ARRAY){ */
            /*     printf("f->name:%s:f->type->u.array.size:%d\n",f->name,f->type->u.array.size); */
            /* } */
            f->tail = NULL;
            insert(f);
            return f;
        }
    }
    else{
        //VarDec -> VarDec LB INT RB , 数组定义
        //检查数组大小是否为正整数
        Node* int_node = node->child->brother->brother;
        if(int_node->type != type_int || int_node->val.val_int <= 0){
            print_error_semantic(12,int_node->line,"Array size must be an positive integer");
            return NULL;
        }
        //构造数组类型
        Type arrt = (Type)malloc(sizeof(Type_));
        arrt->kind = ARRAY;
        arrt->u.array.elem = type;
        arrt->u.array.size = int_node->val.val_int;
        //printf("arrt->u.array.size:%d\n",arrt->u.array.size);
        //递归调用VarDec
        return VarDec(node->child,arrt,structflag);
    }
}

Type Exp(Node* node){
    Type res = NULL;
    //node是Exp节点
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("Exp:%s\n",node->name);
    //Exp -> Exp ASSIGNOP Exp
    //    ok | Exp AND Exp
    //    ok | Exp OR Exp
    //    ok | Exp RELOP Exp
    //    ok | Exp PLUS Exp
    //    ok | Exp MINUS Exp
    //    ok | Exp STAR Exp
    //    ok | Exp DIV Exp
    //    ok | LP Exp RP
    //    ok | MINUS Exp
    //    ok | NOT Exp
    //    ok | ID LP Args RP
    //    ok | ID LP RP
    //    ok | Exp LB Exp RB
    //    ok | Exp DOT ID
    //    ok | ID
    //    ok | INT 
    //    ok | FLOAT
    if(strcmp(node->child->name,"Exp")==0){
        if(strcmp(node->child->brother->name,"ASSIGNOP")==0){
            //Exp -> Exp ASSIGNOP Exp
            //检查赋值类型是否匹配
            if(debug) printf("Exp ASSIGNOP Exp\n");
            Type t1 = Exp(node->child);
            Type t2 = Exp(node->child->brother->brother);
            if(t1 == NULL || t2 == NULL){
                if(debug) printf("Exp ASSIGNOP Exp end\n");
                return NULL;
            } 
            int flag = 0;
            if(t1->assign == RIGHT){
                print_error_semantic(6,node->child->line,"ASSIGNOP: An expression with only the right value appears to the left of the assignment number");
                if(debug) printf("Exp ASSIGNOP Exp end\n");
                flag = 1;
            }
            if(!type_equal(t1,t2)){
                print_error_semantic(5,node->child->line,"ASSIGNOP: The expression types on both sides of the assignment sign do not match");
                if(debug) printf("Exp ASSIGNOP Exp end\n");
                flag = 1;
            }
            if(flag == 1) return NULL;
            else{
                if(debug) printf("Exp ASSIGNOP Exp end\n");
                return t1;
            }
        }
        else if(strcmp(node->child->brother->name,"AND")==0){
            //Exp -> Exp AND Exp
            //检查操作数类型是否匹配
            if(debug) printf("Exp AND Exp\n");
            Type t1 = Exp(node->child);
            Type t2 = Exp(node->child->brother->brother);
            if(t1 == NULL || t2 == NULL){
                if(debug) printf("Exp AND Exp end\n");
                return NULL;
            } 
            if(!type_equal(t1,t2) || t1->kind != BASIC || t1->u.basic != 0 || t2->kind != BASIC || t2->u.basic != 0 ){
                print_error_semantic(7,node->child->line,"And: operand type can't match");
                if(debug) printf("Exp AND Exp end\n");
                return NULL;
            }
            else{
                if(debug) printf("Exp AND Exp end\n");
                return t1;
            }
        }
        else if(strcmp(node->child->brother->name,"OR")==0){
            //Exp -> Exp OR Exp
            //检查操作数类型是否匹配
            if(debug) printf("Exp OR Exp\n");
            Type t1 = Exp(node->child);
            Type t2 = Exp(node->child->brother->brother);
            if(t1 == NULL || t2 == NULL){
                return NULL;
            } 
            if(t1->kind != BASIC || t1->u.basic != 0 || t2->kind != BASIC || t2->u.basic != 0 || !type_equal(t1,t2)){
                print_error_semantic(7,node->child->line,"OR: operand type can't match");
                return NULL;
            }
            else{
                return t1;
            }
        }
        else if(strcmp(node->child->brother->name,"RELOP")==0){
            //Exp -> Exp RELOP Exp
            //检查操作数类型是否匹配
            if(debug) printf("Exp RELOP Exp\n");
            Type t1 = Exp(node->child);
            Type t2 = Exp(node->child->brother->brother);
            if(t1 == NULL || t2 == NULL){
                return NULL;
            } 
            if(t1->kind != BASIC || t1->u.basic != 0 || t2->kind != BASIC || t2->u.basic != 0 || !type_equal(t1,t2)){
                print_error_semantic(7,node->child->line,"RELOP: operand type can't match");
                return NULL;
            }
            else{
                return t1;
            }
        }
        else if(strcmp(node->child->brother->name,"PLUS")==0){
            //Exp -> Exp PLUS Exp
            //检查操作数类型是否匹配
            if(debug) printf("Exp PLUS Exp\n");
            Type t1 = Exp(node->child);
            Type t2 = Exp(node->child->brother->brother);
            if(t1 == NULL || t2 == NULL){
                return NULL;
            } 
            //printf("%d\n",type_equal(t1,t2));
            if(t1->kind != BASIC || t2->kind != BASIC || !type_equal(t1,t2)){
                print_error_semantic(7,node->child->line,"PLUS: operand type can't match");
                return NULL;
            }
            else{
                if(debug) printf("Exp PLUS Exp end\n");
                return t1;
            }
        }
        else if(strcmp(node->child->brother->name,"MINUS")==0){
            //Exp -> Exp MINUS Exp
            //检查操作数类型是否匹配
            if(debug) printf("Exp MINUS Exp\n");
            Type t1 = Exp(node->child);
            Type t2 = Exp(node->child->brother->brother);
            if(t1 == NULL || t2 == NULL){
                return NULL;
            } 
            if(t1->kind != BASIC || t2->kind != BASIC || !type_equal(t1,t2)){
                print_error_semantic(7,node->child->line,"MINUS: operand type can't match");
                return NULL;
            }
            else{
                return t1;
            }
        }
        else if(strcmp(node->child->brother->name,"STAR")==0){
            //Exp -> Exp STAR Exp
            //检查操作数类型是否匹配
            if(debug) printf("Exp STAR Exp\n");
            Type t1 = Exp(node->child);
            Type t2 = Exp(node->child->brother->brother);
            if(t1 == NULL || t2 == NULL){
                return NULL;
            } 
            if(t1->kind != BASIC || t2->kind != BASIC || !type_equal(t1,t2)){
                print_error_semantic(7,node->child->line,"STAR: operand type can't match");
                return NULL;
            }
            else{
                return t1;
            }
        }
        else if(strcmp(node->child->brother->name,"DIV")==0){
            //Exp -> Exp DIV Exp
            //检查操作数类型是否匹配
            if(debug) printf("Exp DIV Exp\n");
            Type t1 = Exp(node->child);
            Type t2 = Exp(node->child->brother->brother); 
            if(t1 == NULL || t2 == NULL){
                return NULL;
            } 
            if(t1->kind != BASIC || t2->kind != BASIC || !type_equal(t1,t2)){
                print_error_semantic(7,node->child->line,"STAR: operand type can't match");
                return NULL;
            }
            else{
                return t1;
            }
        }
        else if(strcmp(node->child->brother->name,"LB")==0){
            //Exp -> Exp LB Exp RB
            //检查数组下标是否为正整数
            if(debug) printf("Exp LB Exp RB\n");
            Type t1 = Exp(node->child);
            Type t2 = Exp(node->child->brother->brother);
            if(t1==NULL || t2==NULL){
                //存疑
                //print_error_semantic(12,node->child->brother->brother->line,"Exp LB Exp RB:Array index must be an integer",node->child->brother->brother->val);
                return NULL;
            }
            if(t1->kind != ARRAY){
                print_error_semantic(10,node->child->line,"Exp LB Exp RB:Not an array");
                if(t2->kind != BASIC || t2->u.basic != 0){
                    print_error_semantic(12,node->child->brother->brother->line,"Exp LB Exp RB:Array index must be an integer");
                }
                return NULL;
            }
            if(t2->kind != BASIC || t2->u.basic != 0){
                print_error_semantic(12,node->child->brother->brother->line,"Exp LB Exp RB:Array index must be an integer");
                return NULL;
            }
            else{
                return t1->u.array.elem;
            }
        }
        else if(strcmp(node->child->brother->name,"DOT")==0){
            //Exp -> Exp DOT ID
            //检查结构体是否有该域
            if(debug) printf("Exp DOT ID\n");
            Type t = Exp(node->child);
            if(t == NULL){
                return NULL;
            }
            if(t->kind != STRUCTURE){
                print_error_semantic(13,node->child->line,"Exp DOT ID:Illegal use of DOT");
                return NULL;
            }
            else{
                Node* id = node->child->brother->brother;
                FieldList f = t->u.structure;
                if(f == NULL){
                    printf("Error in Exp DOT ID:structure is empty\n");
                }
                while(f != NULL){
                    if(strcmp(f->name,id->val.val_id) == 0){
                        return f->type;
                    }
                    f = f->tail;
                }
                print_error_semantic(14,id->line,"Exp DOT ID:Non-existent field");
                return NULL;
            }
        }
    } 
    else{
        if(strcmp(node->child->name,"ID")==0){
            //Exp -> ID
            //Exp -> ID LP Args RP
            //Exp -> ID LP RP
            Node* id = node->child;
            FieldList f = find(id->val.val_id);
            if(id->brother == NULL){
                //Exp -> ID
                if(debug) printf("Exp -> ID\n");
                if(f == NULL){
                    print_error_semantic(1,id->line,"Undefined variable");
                    return NULL;
                }
                //printf("%d\n",f->type->kind);
                return f->type;
            }
            else{
                //Exp -> ID LP Args RP
                //Exp -> ID LP RP
                if(f == NULL){
                    print_error_semantic(2,id->line,"Undefined function");
                    return NULL;
                }
                if(f->type == NULL || f->type->kind != FUNCTION){
                    print_error_semantic(11,id->line,"Exp:Not a function");
                    return NULL;
                }
                else{
                    if(strcmp(id->brother->brother->name,"Args") == 0){
                        //Exp -> ID LP Args RP
                        //检查参数类型是否匹配
                        if(debug) printf("Exp -> ID LP Args RP\n");
                        FieldList args = Args(node->child->brother->brother);
                        if(!param_equal(f->type->u.function->param,args) && strcmp(f->type->u.function->name,"write")!=0){
                            //&& strcmp(f->type->u.function->name,"write")!=0
                            //printf("end\n");
                            //printf("%s\n",f->type->u.function->name);
                            print_error_semantic(9,node->child->line,"Function call with wrong arguments");
                        }
                        //printf("lalaala\n");
                    }
                    else{
                        //Exp -> ID LP RP
                        //检查参数个数是否匹配
                        if(debug) printf("Exp -> ID LP RP\n");
                        if(f->type->u.function->param != NULL){
                            print_error_semantic(9,node->child->line,"Function call with wrong arguments");
                        }
                    }
                    return f->type->u.function->ret;
                }
            }
        }
        else if(strcmp(node->child->name,"INT")==0){
            //Exp -> INT
            if(debug) printf("Exp -> INT\n");
            if(debug) printf("INT: %d\n",node->child->val.val_int);
            Type t = (Type)malloc(sizeof(Type_));
            t->kind = BASIC;
            t->u.basic = 0;
            t->assign = RIGHT;
            return t;
        }
        else if(strcmp(node->child->name,"FLOAT")==0){
            //Exp -> FLOAT
            if(debug) printf("Exp -> FLOAT\n");
            Type t = (Type)malloc(sizeof(Type_));
            t->kind = BASIC;
            t->u.basic = 1;
            t->assign = RIGHT;
            return t;
        }
        else if(strcmp(node->child->name,"LP")==0){
            //Exp -> LP Exp RP
            if(debug) printf("LP Exp RP\n");
            Node* exp = node->child->brother;
            return Exp(exp);
        }
        else if(strcmp(node->child->name,"MINUS")==0){
            //Exp -> MINUS Exp
            if(debug) printf("MINUS Exp\n");
            res = Exp(node->child->brother);
            if(res != NULL && (res->kind != BASIC || res->u.basic != 0)){
                print_error_semantic(7,node->child->line,"operand type can't match");
                return NULL;
            }
        }
        else if(strcmp(node->child->name,"NOT")==0){
            //Exp -> NOT Exp
            if(debug) printf("NOT Exp\n");
            res = Exp(node->child->brother);
            return res;
        }
    }
    return res;
}

FieldList Args(Node* node){
    //node是Args节点
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("Args:%s\n",node->name);
    //Args -> Exp COMMA Args
    //     | Exp
    FieldList f = (FieldList)malloc(sizeof(FieldList_));
    f->type = Exp(node->child);
    f->name = node->child->val.val_id;
    if(node->child->brother == NULL){
        //Args -> Exp
        f->tail = NULL;
    }
    else{
        //Args -> Exp COMMA Args
        f->tail = Args(node->child->brother->brother);
    }
    return f;
}

void CompSt(Node* node, Type type){
    //node是CompSt节点
    if (node == NULL) {
        return;
    }
    if(debug) printf("CompSt:%s\n",node->name);
    //if(debug) printf("%s,%s,%s\n",node->child->name,node->child->brother->name,node->child->brother->brother->name);
    //CompSt -> LC DefList StmtList RC,有时候没有deflist，有时候没有stmtlist，需要判断
    Node* curnode = node->child;
    while(curnode != NULL){
        if(strcmp(curnode->name,"DefList") == 0){
            DefList(curnode,0);
            curnode = curnode->brother;
        }
        else if(strcmp(curnode->name,"StmtList") == 0){
            StmtList(curnode,type);
            curnode = curnode->brother;
        }
        else if(strcmp(curnode->name,"RC") == 0){
            return;
        }
        else{
            curnode = curnode->brother;
        }
    }
    return;
}

void Stmt(Node* node, Type type){
    if(node == NULL){
        return;
    }
    if(debug) printf("Stmt:%s\n",node->name);
    //Stmt -> Exp SEMI
    //     | CompSt
    //     | RETURN Exp SEMI
    //     | IF LP Exp RP Stmt
    //     | IF LP Exp RP Stmt ELSE Stmt
    //     | WHILE LP Exp RP Stmt
    if(strcmp(node->child->name,"Exp")==0){
        //Stmt -> Exp SEMI
        Exp(node->child);
        if(debug) printf("Exp SEMI end\n");
    }
    else if(strcmp(node->child->name,"CompSt")==0){
        //Stmt -> CompSt
        CompSt(node->child,type);
    }
    else if(strcmp(node->child->name,"RETURN")==0){
        //Stmt -> RETURN Exp SEMI
        Type t = Exp(node->child->brother);
        if(!type_equal(t,type)){
            print_error_semantic(8,node->child->line,"Stmt -> RETURN Exp SEMI: The return type mismatch for the function return type");
        }
    }
    else if(strcmp(node->child->name,"IF")==0){
        //Stmt -> IF LP Exp RP Stmt
        //     | IF LP Exp RP Stmt ELSE Stmt
        Type t = Exp(node->child->brother->brother);
        if(t != NULL && (t->kind != BASIC || t->u.basic != 0)){
            print_error_semantic(7,node->child->line,"Stmt->IF...: operand type can't match");
        }
        Stmt(node->child->brother->brother->brother->brother,type);
        if(node->child->brother->brother->brother->brother->brother != NULL){
            //Stmt -> IF LP Exp RP Stmt ELSE Stmt
            Stmt(node->child->brother->brother->brother->brother->brother->brother,type);
        }
    }
    else if(strcmp(node->child->name,"WHILE")==0){
        //Stmt -> WHILE LP Exp RP Stmt
        Type t = Exp(node->child->brother->brother);
        if(t != NULL && (t->kind != BASIC || t->u.basic != 0)){
            print_error_semantic(7,node->child->line,"WHILE: operand type can't match");
        }
        Stmt(node->child->brother->brother->brother->brother,type);
    }
}

void StmtList(Node* node, Type type){
    //node是StmtList节点
    if (node == NULL) {
        return;
    }
    if(debug) printf("StmtList:%s\n",node->name);
    //StmtList -> Stmt StmtList
    //         | ε
    if(node->child == NULL){
        return;
    }
    Stmt(node->child,type);
    StmtList(node->child->brother,type);
}

void FunDec(Node* node, Type type){
    //node是FunDec节点
    if (node == NULL) {
        return;
    }
    if(debug) printf("FunDec:%s\n",node->name);
    //FunDec -> ID LP VarList RP
    //       | ID LP RP
    Node* id = node->child;
    FieldList f = find(id->val.val_id);
    if(f != NULL && f->type != NULL && f->type->kind == FUNCTION){
        print_error_semantic(4,id->line,"Redefined function");
        //return;
    }
    
        f = (FieldList)malloc(sizeof(FieldList_));
        f->name = id->val.val_id;
        f->type = (Type)malloc(sizeof(Type_));
        f->type->kind = FUNCTION;
        f->type->u.function = (Function)malloc(sizeof(Function_));
        f->type->u.function->ret = type;
        f->type->u.function->ret->assign = RIGHT;
        f->type->u.function->line = id->line;
        f->type->u.function->name = id->val.val_id;
        f->type->u.function->param = NULL;
        f->tail = NULL;
        insert(f);
        if(strcmp(id->brother->brother->name,"VarList") == 0){
            //FunDec -> ID LP VarList RP
            f->type->u.function->param = VarList(id->brother->brother);
        }
        else{
            //FunDec -> ID LP RP
            f->type->u.function->param = NULL;
        }
    
}

FieldList VarList(Node* node){
    //node是VarList节点
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("VarList:%s\n",node->name);
    //VarList -> ParamDec COMMA VarList
    //        | ParamDec
    FieldList f = ParamDec(node->child);
    if(node->child->brother == NULL){
        //VarList -> ParamDec
        return f;
    }
    else{
        //VarList -> ParamDec COMMA VarList
        FieldList f2 = VarList(node->child->brother->brother);
        if(f2 == NULL){
            return f;
        }
        else{
            FieldList f_cur = f;
            while(f_cur->tail != NULL){
                f_cur = f_cur->tail;
            }
            f_cur->tail = f2;
            return f;
        }
    }
}

FieldList ParamDec(Node* node){
    //node是ParamDec节点
    if (node == NULL) {
        return NULL;
    }
    if(debug) printf("ParamDec:%s\n",node->name);
    //ParamDec -> Specifier VarDec
    Type t = Specifier(node->child);
    FieldList f = VarDec(node->child->brother,t,0);
    return f;
}