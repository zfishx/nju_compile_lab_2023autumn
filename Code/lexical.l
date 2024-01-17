%{
    #include "syntax.tab.h"
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>
    #include <stdlib.h>
    #include <string.h>
    #include "common.h"

    #define debug 0

    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
	    yylloc.first_column = yycolumn; \
	    yylloc.last_column = yycolumn + yyleng - 1; \
	    yycolumn += yyleng; 

    int lex_error = 0;

    //int wrong_line[100];
    //int wrong_lin_num = 0;

    void print_error_msg_A(char *msg){
        lex_error++;
        //wrong_line[wrong_lin_num++] = yylineno;
        //printf("Error type A at Line %d: %s %s.\n", yylineno, msg ,yytext);
        printf("Error type A at Line %d: %s.\n", yylineno, msg);
    }

    //extern void yyerror(char *msg);

    void add_yy_node(char *name, char *val, data_type type, int token_flag, int line, int num, ...){
        yylval.node = (struct Node*)add_node(name, val, type, token_flag, line, num);
    }

%}

%option yylineno
digit   [0-9]
letter  [_a-zA-Z]
STRUCT  "struct"
RETURN  "return"
IF      "if"
ELSE    "else"
WHILE   "while"
TYPE    int|float
OCT     0[0-7]+
HEX     0[X|x][0-9a-fA-F]+
RELOP   ">"|"<"|">="|"<="|"=="|"!="
SEMI    ";"
COMMA   ","
ASSIGNOP    "="
PLUS    "+"
MINUS   "-"
STAR    "*"
DIV     "/"
AND     "&&"
OR      "||"
DOT     "."
NOT     "!"
LP      \(
RP      \)
LB      \[
RB      \]
LC      \{
RC      \}
INT     0|([1-9]{digit}*)
FLOAT   {digit}+\.{digit}+
EFLOAT  {digit}*\.{digit}+[eE][+-]?{digit}+|{digit}+\.{digit}*[eE][+-]?{digit}+
ID      {letter}({digit}|{letter})*
%x      COMMENT
%%

{TYPE}    {
    if(debug) printf("matched type:%s\n",yytext);
    add_yy_node("TYPE",yytext,type_type,1,yylineno,0);
    return TYPE;
}
{OCT}     {
    if(debug) printf("matched oct:%s\n",yytext);
    add_yy_node("INT",yytext,type_oct,1,yylineno,0);
    return INT;
}
{HEX}     {
    if(debug) printf("matched hex:%s\n",yytext);
    add_yy_node("INT",yytext,type_hex,1,yylineno,0);
    return INT;
}
{RELOP}   {
    if(debug) printf("matched relop:%s\n",yytext);
    add_yy_node("RELOP",NULL,other,1,yylineno,0);
    return RELOP;
}
{SEMI}    {
    if(debug) printf("matched semi:%s\n",yytext);
    add_yy_node("SEMI",NULL,other,1,yylineno,0);
    return SEMI;
}
{COMMA}   {
    if(debug) printf("matched comma:%s\n",yytext);
    add_yy_node("COMMA",NULL,other,1,yylineno,0);
    return COMMA;
}
{ASSIGNOP}    {
    if(debug) printf("matched assignop:%s\n",yytext);
    add_yy_node("ASSIGNOP",NULL,other,1,yylineno,0);
    return ASSIGNOP;
}
{PLUS}    {
    if(debug) printf("matched plus:%s\n",yytext);
    add_yy_node("PLUS",NULL,other,1,yylineno,0);
    return PLUS;
}
{MINUS}   {
    if(debug) printf("matched minus:%s\n",yytext);
    add_yy_node("MINUS",NULL,other,1,yylineno,0);
    return MINUS;
}
{STAR}    {
    if(debug) printf("matched star:%s\n",yytext);
    add_yy_node("STAR",NULL,other,1,yylineno,0);
    return STAR;
}
{DIV}    {
    if(debug) printf("matched div:%s\n",yytext);
    add_yy_node("DIV",NULL,other,1,yylineno,0);
    return DIV;
}
{AND}     {
    if(debug) printf("matched and:%s\n",yytext);
    add_yy_node("AND",NULL,other,1,yylineno,0);
    return AND;
}
{OR}      {
    if(debug) printf("matched or:%s\n",yytext);
    add_yy_node("OR",NULL,other,1,yylineno,0);
    return OR;
}
{DOT}     {
    if(debug) printf("matched dot:%s\n",yytext);
    add_yy_node("DOT",NULL,other,1,yylineno,0);
    return DOT;
}
{NOT}     {
    if(debug) printf("matched dot:%s\n",yytext);
    add_yy_node("NOT",NULL,other,1,yylineno,0);
    return NOT;
}

{LP}      {
    if(debug) printf("matched lp:%s\n",yytext);
    add_yy_node("LP",NULL,other,1,yylineno,0);
    return LP;
}
{RP}      {
    if(debug) printf("matched rp:%s\n",yytext);
    add_yy_node("RP",NULL,other,1,yylineno,0);
    return RP;
}
{LB}      {
    if(debug) printf("matched lb:%s\n",yytext);
    add_yy_node("LB",NULL,other,1,yylineno,0);
    return LB;
}
{RB}      {
    if(debug) printf("matched rb:%s\n",yytext);
    add_yy_node("RB",NULL,other,1,yylineno,0);
    return RB;
}
{LC}      {
    if(debug) printf("matched lc:%s\n",yytext);
    add_yy_node("LC",NULL,other,1,yylineno,0);
    return LC;
}
{RC}      {
    if(debug) printf("matched rc:%s\n",yytext);
    add_yy_node("RC",NULL,other,1,yylineno,0);
    return RC;
}
{STRUCT}  {
    if(debug) printf("matched struct:%s\n",yytext);
    add_yy_node("STRUCT",NULL,other,1,yylineno,0);
    return STRUCT;
}
{RETURN}  {
    if(debug) printf("matched return:%s\n",yytext);
    add_yy_node("RETURN",NULL,other,1,yylineno,0);
    return RETURN;
}
{IF}      {
    if(debug) printf("matched if:%s\n",yytext);
    add_yy_node("IF",NULL,other,1,yylineno,0);
    return IF;
}
{ELSE}    {
    if(debug) printf("matched else:%s\n",yytext);
    add_yy_node("ELSE",NULL,other,1,yylineno,0);
    return ELSE;
}
{WHILE}   {
    if(debug) printf("matched while:%s\n",yytext);
    add_yy_node("WHILE",NULL,other,1,yylineno,0);
    return WHILE;
}
{INT}    {
    if(debug) printf("matched int:%s\n",yytext);
    add_yy_node("INT",yytext,type_int,1,yylineno,0);
    return INT;
}
{FLOAT}   {
    if(debug) printf("matched float:%s\n",yytext);
    add_yy_node("FLOAT",yytext,type_float,1,yylineno,0);
    return FLOAT;
}
{EFLOAT}   {
    print_error_msg_A("Can't recognize the exponential form of float");
    return FLOAT;
}
0[0-7]*[89][0-7]*    {
    print_error_msg_A("Wrong type of oct");
    return INT;
}
0[X|x][0-9a-fA-F]*[g-zG-Z]*[0-9a-fA-F]*    {
    print_error_msg_A("Wrong type of hex");
    return INT;
}
0{digit}+    {
    print_error_msg_A("Wrong type of int");
    return INT;
}
({INT}\.)|(\.{INT})   {
    print_error_msg_A("Wrong type of float");
    return 0;
}
{ID}      {
    if(debug) printf("matched id:%s\n",yytext);
    add_yy_node("ID",yytext,type_id,1,yylineno,0);
    return ID;
}
({digit}|{letter})*@+({digit}|{letter})* {
    print_error_msg_A("error ID");
    return ID;
}
{FLOAT}.{FLOAT}+ {
    print_error_msg_A("error float");
    return ID;
}
"//"[^"\n"]* {  
    //print_error_msg_A("Can't recognize the annotation");
    yyerror("Can't recognize the annotation");
}
"/*"         { 
    //print_error_msg_A("Can't recognize the annotation");
    yyerror("Can't recognize the annotation");
    BEGIN(COMMENT); 
}  /* 当匹配到 "/*"，进入COMMENT状态 */
<COMMENT>"*/"   { 
    //print_error_msg_A("Can't recognize the annotation");
    yyerror("Can't recognize the annotation");
    BEGIN(INITIAL); 
} /* 当在COMMENT状态中匹配到 * /返回到INITIAL状态 */
<COMMENT>.|\n  { 
    //("Can't recognize the annotation");
    yyerror("Can't recognize the comment of annotation");
    /* 这里什么也不做，因此注释中的内容会被忽略 */ 
}
Mysterious  {
    //print_error_msg_A("Mysterious character");
    return ID;
}
"\n"        { yycolumn = 1; }
[\t\f\r\v ]	      {}

.|\n {
    print_error_msg_A("Mysterious character");
    //printf("unmatched character: %s\n",yytext);
} 

%%

