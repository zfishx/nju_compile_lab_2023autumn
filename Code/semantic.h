#include "symbol_tab.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

void semantic_check(Node* root);//语法分析入口函数

int type_equal(Type a, Type b);
int param_equal(FieldList p1,FieldList p2);

void Program(Node* node);
void ExtDefList(Node* node);
void ExtDecList(Node* node, Type type);
void ExtDef(Node* node);
Type Specifier(Node* node);
Type StructSpecifier(Node* node);
FieldList DefList(Node* node,int structflag);  
FieldList Def(Node* node,int structflag);
FieldList DecList(Node* node, Type type,int structflag); 
FieldList Dec(Node* node, Type type,int structflag);
FieldList VarDec(Node* node, Type type, int structflag);
Type Exp(Node* node);
FieldList Args(Node* node);
void CompSt(Node* node, Type type);
void Stmt(Node* node, Type type);
void Stmtlist(Node* node, Type type);
void FunDec(Node* node, Type type);
FieldList VarList(Node* node);
FieldList ParamDec(Node* node);






