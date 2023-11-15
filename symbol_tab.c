#include "symbol_tab.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

unsigned int hash_pjw(char* name)//讲义上的哈希函数
{
  unsigned int val = 0, i;
  for (; *name; ++name)
  {
    val = (val << 2) + *name;
    if (i = val & ~HASH_TABLE_SIZE) val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
  }
  return val;
}

void init_symbol_table()
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        st.table[i] = NULL;
    }
}

void insert(FieldList f){
    unsigned int index = hash_pjw(f->name);
    HashNode* p = st.table[index];
    if (p == NULL) {
        st.table[index] = (HashNode*)malloc(sizeof(HashNode));
        st.table[index]->f = f;
        st.table[index]->next = NULL;
    } else {
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = (HashNode*)malloc(sizeof(HashNode));
        ((HashNode*)p->next)->f = f;
        ((HashNode*)p->next)->next = NULL;
    }
}

FieldList find(char* name){
    unsigned int index = hash_pjw(name);
    HashNode* p = st.table[index];
    while (p != NULL) {
        if (strcmp(p->f->name, name) == 0) {
            return p->f;
        }
        p = p->next;
    }
    return NULL;
}