%{
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>
    #include <stdlib.h>
    #include <string.h>
    #include "lex.yy.c"
    extern void yyerrok;
    //#include "common.h"

    int syntax_error = 0;

    Node* root = NULL;

    int wrong_line[100];
    int wrong_lin_num;

    void yyerror(const char *msg){
        for(int i=0;i<wrong_lin_num;i++){
            if(wrong_line[i]==yylineno){
                return;
            }
        }
        printf("Error type B at Line %d: %s,yytext:%s.\n", yylineno, msg,yytext);
        syntax_error++;
        wrong_line[wrong_lin_num++] = yylineno;
    }
%}

%union {
    struct Node* node;
}
%token <node> INT
%token <node> FLOAT
%token <node> ID
%token <node> SEMI
%token <node> COMMA
%token <node> ASSIGNOP
%token <node> RELOP
%token <node> PLUS
%token <node> MINUS
%token <node> STAR
%token <node> DIV
%token <node> AND
%token <node> OR
%token <node> DOT
%token <node> NOT
%token <node> TYPE
%token <node> LP RP LB RB LC RC
%token <node> STRUCT
%token <node> RETURN 
%token <node> IF
%token <node> ELSE
%token <node> WHILE

%type <node> Program
%type <node> ExtDecList
%type <node> ExtDef
%type <node> ExtDefList
%type <node> Specifier
%type <node> StructSpecifier
%type <node> OptTag
%type <node> Tag
%type <node> VarDec
%type <node> FunDec
%type <node> VarList
%type <node> ParamDec
%type <node> CompSt
%type <node> StmtList
%type <node> Stmt
%type <node> DefList
%type <node> Def
%type <node> DecList
%type <node> Dec
%type <node> Exp
%type <node> Args

%start Program
%right ASSIGNOP
%left OR
%left AND 
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT 
%left NEG //负号 
%left DOT 
%left LB RB 
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%

/* High-level Definitions */
Program : ExtDefList                                { //printf("debug-1");
                                                      $$=add_node("Program",NULL,other, 0, @$.first_line,1,$1);
                                                      root = $$;
                                                    }                                             
    ;               
ExtDefList : ExtDef ExtDefList                      { 
                                                      $$=add_node("ExtDefList",NULL,other, 0, @$.first_line,2,$1,$2); 
                                                      //printf("debug-2");
                                                    }
    |  /*ε*/                                        { $$=NULL; }
    ;               
ExtDef : Specifier ExtDecList SEMI                  { 
                                                       $$=add_node("ExtDef",NULL,other, 0, @$.first_line,3,$1,$2,$3); 
                                                       //printf("debug-3");
                                                    }
    | Specifier SEMI                                { 
                                                       $$=add_node("ExtDef",NULL,other, 0, @$.first_line,2,$1,$2); 
                                                       //printf("debug-4");
                                                    }
    | Specifier FunDec CompSt                       { $$=add_node("ExtDef",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Specifier FunDec SEMI                         { $$=add_node("ExtDef",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | error SEMI                            { yyerrok; }
    | Specifier error SEMI                  { yyerrok; }
    | Specifier error CompSt                { yyerrok; }
    ;               
ExtDecList : VarDec                                 { $$=add_node("ExtDecList",NULL,other, 0, @$.first_line,1,$1); }
    | VarDec COMMA ExtDecList                       { $$=add_node("ExtDecList",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
   // | VarDec error ExtDecList               {yyerror("Missing something"); yyerrok;}
    ;

/* Specifiers */
Specifier : TYPE                                    { $$=add_node("Specifier",NULL,other, 0, @$.first_line,1,$1); }
    | StructSpecifier                               { $$=add_node("Specifier",NULL,other, 0, @$.first_line,1,$1); }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC       { $$=add_node("StructSpecifier",NULL,other, 0, @$.first_line,5,$1,$2,$3,$4,$5); }
    | STRUCT Tag                                    { $$=add_node("StructSpecifier",NULL,other, 0, @$.first_line,2,$1,$2); }
    | STRUCT OptTag LC DefList error RC       { yyerrok; }
    ;
OptTag : ID                                         { $$=add_node("OptTag",NULL,other, 0, @$.first_line,1,$1); }
    |                                               { $$=NULL; }
    ;
Tag : ID                                            { $$=add_node("Tag",NULL,other, 0, @$.first_line,1,$1); }
    ;

/* Declarators */
VarDec : ID                                         { $$=add_node("VarDec",NULL,other, 0, @$.first_line,1,$1); }
    | VarDec LB INT RB                              { $$=add_node("VarDec",NULL,other, 0, @$.first_line,4,$1,$2,$3,$4); }
    ;               
FunDec : ID LP VarList RP                           { $$=add_node("FunDec",NULL,other, 0, @$.first_line,4,$1,$2,$3,$4); }
    | ID LP RP                                      { $$=add_node("FunDec",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
VarList : ParamDec COMMA VarList                    { $$=add_node("VarList",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | ParamDec                                      { $$=add_node("VarList",NULL,other, 0, @$.first_line,1,$1); }
    ;
ParamDec : Specifier VarDec                         { $$=add_node("ParamDec",NULL,other, 0, @$.first_line,2,$1,$2); }
    ;

/* Statements */
CompSt : LC DefList StmtList RC                     { $$=add_node("CompSt",NULL,other, 0, @$.first_line,4,$1,$2,$3,$4); }
   // | LC error RC                            {print_error_msg_B("syntax error",@$.first_line); syntax_error++; yyerrok;}
    ;
StmtList : Stmt StmtList                            { $$=add_node("StmtList",NULL,other, 0, @$.first_line,2,$1,$2); }
    |                                               { $$=NULL; }
    ;
Stmt : Exp SEMI                                     { $$=add_node("Stmt",NULL,other, 0, @$.first_line,2,$1,$2); }
    | CompSt                                        { $$=add_node("Stmt",NULL,other, 0, @$.first_line,1,$1); }
    | RETURN Exp SEMI                               { $$=add_node("Stmt",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE       { $$=add_node("Stmt",NULL,other, 0, @$.first_line,5,$1,$2,$3,$4,$5); }
    | IF LP Exp RP Stmt ELSE Stmt                   { $$=add_node("Stmt",NULL,other, 0, @$.first_line,7,$1,$2,$3,$4,$5,$6,$7); }
    | WHILE LP Exp RP Stmt                          { $$=add_node("Stmt",NULL,other, 0, @$.first_line,5,$1,$2,$3,$4,$5); }       
    | error SEMI                             {/*  yyerror("syntax error-26");*/ yyerrok;  }
    | error Stmt                                    { yyerrok; }
    | Exp error                                     {}
    | WHILE LP error RP Stmt                        { yyerrok; }

/* Local Definitions */
DefList : Def DefList                               { $$=add_node("DefList",NULL,other, 0, @$.first_line,2,$1,$2); }
    |                                               { $$=NULL; }
    ;
Def : Specifier DecList SEMI                        { $$=add_node("Def",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Specifier DecList error                {/* yyerror("syntax error-31"); */yyerrok; } 
    | Specifier error SEMI                   {/* yyerror("syntax error-30");*/ yyerrok; } 
    ;
DecList : Dec                                       { $$=add_node("DecList",NULL,other, 0, @$.first_line,1,$1); }
    | Dec COMMA DecList                             { $$=add_node("DecList",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    ;
Dec : VarDec                                        { $$=add_node("Dec",NULL,other, 0, @$.first_line,1,$1); }
    | VarDec ASSIGNOP Exp                           { $$=add_node("Dec",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    ;

/* Expressions */
Exp : Exp ASSIGNOP Exp                              { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Exp AND Exp                                   { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Exp OR Exp                                    { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Exp RELOP Exp                                 { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Exp PLUS Exp                                  { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Exp MINUS Exp                                 { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Exp STAR Exp                                  { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Exp DIV Exp                                   { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | LP Exp RP                                     { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | MINUS Exp %prec NEG                           { $$=add_node("Exp",NULL,other, 0, @$.first_line,2,$1,$2); }
   // | MINUS Exp                                     { $$=add_node("Exp",NULL,other, 0, @$.first_line,2,$1,$2); }
    | NOT Exp                                       { $$=add_node("Exp",NULL,other, 0, @$.first_line,2,$1,$2); }
    | ID LP Args RP                                 { $$=add_node("Exp",NULL,other, 0, @$.first_line,4,$1,$2,$3,$4); }
    | ID LP RP                                      { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Exp LB Exp RB                                 { $$=add_node("Exp",NULL,other, 0, @$.first_line,4,$1,$2,$3,$4); }
    | Exp DOT ID                                    { $$=add_node("Exp",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | ID                                            { $$=add_node("Exp",NULL,other, 0, @$.first_line,1,$1); }
    | INT                                           { $$=add_node("Exp",NULL,other, 0, @$.first_line,1,$1); }
    | FLOAT                                         { $$=add_node("Exp",NULL,other, 0, @$.first_line,1,$1); }
    | Exp ASSIGNOP error                    { /* yyerror("syntax error-1");  */ }  
    | Exp AND error                         { /* yyerror("syntax error-2");  */ }    
    | Exp OR error                          { /* yyerror("syntax error-3");  */ }    
    | Exp RELOP error                       { /* yyerror("syntax error-4");  */ }    
    | Exp PLUS error                        { /* yyerror("syntax error-5");  */ }    
    | Exp MINUS error                       { /* yyerror("syntax error-6");  */ }  
    | Exp STAR error                        { /* yyerror("syntax error-7");  */ }    
    | Exp DIV error                         { /* yyerror("syntax error-8");  */ }        
    | LP error RP                           { /* yyerror("syntax error-11"); */ }   
    | LP Exp error                          { /* yyerror("syntax error-12"); */ } 
    | MINUS error %prec NEG                 {}
    | NOT error                             { /* yyerror("syntax error-10"); */ }  
    | ID LP error RP                        { /* yyerror("syntax error-13"); */ }
    | ID LP error SEMI                      { yyerror("Missing something"); yyerrok;}
    | Exp LB error RB                       { /* yyerror("syntax error-15"); */ }
    ;
Args : Exp COMMA Args                               { $$=add_node("Args",NULL,other, 0, @$.first_line,3,$1,$2,$3); }
    | Exp                                           { $$=add_node("Args",NULL,other, 0, @$.first_line,1,$1); }
    | error COMMA Exp                               {}
    ;
%%
