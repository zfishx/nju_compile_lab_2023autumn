#include "common.h"
void print_AST(Node *root, int depth)
{
    if (root == NULL)
    {
        return;
    }
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }
    if (root->token_flag == 1)
    {
        if (root->type == type_int)
        {
            printf("%s: %d\n", root->name, root->val.val_int);
        }
        else if (root->type == type_float)
        {
            printf("%s: %f\n", root->name, root->val.val_float);
        }
        else if (root->type == type_id)
        {
            printf("%s: %s\n", root->name, root->val.val_id);
        }
        else if (root->type == type_type)
        {
            printf("%s: %s\n", root->name, root->val.val_id);
        }
        else{
            printf("%s\n", root->name);
        }
    }
    else{
        printf("%s (%d)\n", root->name, root->line);
    }
    print_AST((Node *)root->child, depth + 1);
    print_AST((Node *)root->brother, depth);
}

Node *add_node(char *name, char *val, data_type type, int token_flag, int line, int num, ...)
{
    //("add node: %s ", name);
    Node *node = (Node *)malloc(sizeof(Node));
    strcpy(node->name, name);
    node->type = type;
    node->line = line;
    node->token_flag = token_flag;
    node->child = NULL;
    node->brother = NULL;
    if (token_flag == 1)
    { //终结符
        if (type == type_int)
        {
            node->val.val_int = atoi(val);
        }
        else if (type == type_float)
        {
            node->val.val_float = atof(val);
        }
        else if (type == type_id)
        {
            strcpy(node->val.val_id, val);
        }
        else if (type == type_oct)
        {
            int octalNum = (int)strtol(val, NULL, 0); // Base 0 means the base is auto-detected
            //printf("The decimal equivalent of octal %s is %d\n", val, octalNum);
            node->val.val_int = octalNum;
            node->type = type_int;
        }
        else if (type == type_hex)
        {
            int hexNum = (int)strtol(val, NULL, 0); // Base 0 means the base is auto-detected
            //printf("The decimal equivalent of hexadecimal %s is %d\n", val, hexNum);
            node->val.val_int = hexNum;
            node->type = type_int;
        }
        else if (type == type_type)
        {
            strcpy(node->val.val_id, val);
        }
    }
    if (num > 0)
    {
        //易错点：va_arg(valist, Node *)这里传的参数有可能是NULL
        va_list valist;
        Node** nonNullNodes = (Node**)malloc(num * sizeof(Node*));
        int nonNullCount = 0;
        // 第一次遍历：存储非NULL元素和计数
        va_start(valist, num);
        for(int i = 0; i < num; i++) {
            Node* node = va_arg(valist, Node*);
            if(node != NULL) {
                nonNullNodes[nonNullCount++] = node;
                //printf("%s ", node->name);
            }
        }
        va_end(valist);
        node->child = nonNullNodes[0];
        Node *temp = (Node *)node->child;
        for (int i = 1; i < nonNullCount; i++)
        {
            temp->brother = nonNullNodes[i];
            temp = (Node *)temp->brother;
        }
    }
    //printf("\n");
    //print_AST(node, 0);
    return node;
}
    