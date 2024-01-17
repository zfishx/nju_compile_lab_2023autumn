#include "common.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_SIZE 0x3fff

typedef struct Type_* Type;
typedef struct Type_ Type_;
typedef struct FieldList_* FieldList;
typedef struct FieldList_ FieldList_;
typedef struct Function_* Function;
typedef struct Function_ Function_;

struct Function_{
  char* name;
  int line;
  Type ret;
  FieldList param;
};

struct Type_
{
  enum { BASIC=1, ARRAY=2, STRUCTURE=3 , FUNCTION=4 } kind;
  union
  {
    // 基本类型
    int basic;//用0表示int,用1表示float
    // 数组类型信息包括元素类型与数组大小构成
    struct {Type elem; int size; } array;
    // 结构体类型信息是一个链表
    FieldList structure;
    Function function;
  } u;
  enum {
    LEFT, // left value
    RIGHT, // right value
    BOTH // left | right value
  } assign;
} ;

struct FieldList_
{
  char* name; // 域的名字
  Type type; // 域的类型
  FieldList tail; // 下一个域
  int var_no;//变量的序号，用于中间代码生成
};

typedef struct {
    FieldList f;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode* table[HASH_TABLE_SIZE+10];
} SymbolTable;

SymbolTable st;//全局符号表

unsigned int hash_pjw(char* name);//讲义上的哈希函数
void init_symbol_table();//初始化符号表
void insert(FieldList f);//插入一个符号
FieldList find(char* name);//查找一个符号
void set_var_no();//设置变量的序号,用于中间代码生成

