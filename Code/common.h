#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;

typedef enum
{
    type_int = 1,
    type_float = 2,
    type_id = 3,
    type_oct = 4,
    type_hex = 5,
    type_type = 6,
    other = 7
} data_type;

struct Node
{
    char name[35];  //每个节点的名字
    data_type type; //每个节点的类型
    int line;       //每个节点的行号
    int token_flag; //是否是终结符
    union Value
    {
        int val_int;
        float val_float;
        char val_id[35];
    } val;                //每个节点的值
    struct Node* child;   //每个节点的孩子节点
    struct Node* brother; //每个节点的兄弟节点
};

Node *add_node(char *name, char *val, data_type type, int token_flag, int line, int num, ...);

void print_AST(Node *root, int depth);