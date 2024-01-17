#include <stdio.h> 
#include "common.h"
//extern int yydebug;
extern FILE* yyin;
void  yyrestart(FILE* f);
extern int yyparse();
extern int lex_error;
extern int syntax_error;
extern Node* root;

//#define YYDEBUG 1

// int main(int argc, char **argv)
// {
//     if (argc > 1)
//     {
//         if (!(yyin = fopen(argv[1], "r")))
//         {
//             perror(argv[1]);
//             return 1;
//         }
//     }
//     while (yylex() != 0);
//     return 0;
// }

int main(int argc, char** argv) 
{ 
    //printf("Begin\n");
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f){
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    //printf("Before yyparse()\n");
    //yydebug = 1;
    yyparse();
    //printf("After yyparse()\n");
    if(lex_error == 0 && syntax_error == 0){
        //print_AST(root,0);
        semantic_check(root);
        //printf("syntax_error: %d\n",syntax_error);
        set_var_no();
        if(argv[2] == NULL){f = fopen("out1.ir", "w");}
        else f =fopen(argv[2], "w"); // 构造输出中间代码文件
        Intercode(root,f); // 中间代码生成
       // print_IR(f); // 写入生成的中间代码
       // FILE* f2 = fopen("result.s", "w");
        generate_asm(f); // 生成汇编代码
        fclose(f); // 关闭文件
       // fclose(f2);
        return 0; 
    } 
    else{
        //print_AST(root,0);
        //printf("syntax_error: %d\n",syntax_error);
    }
    return 0;
} 
