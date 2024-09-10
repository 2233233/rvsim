//copy form https://blog.csdn.net/weixin_54186646/article/details/124557830

#ifndef MYVM_AVLTREE_H
#define MYVM_AVLTREE_H
#include "riscv.h"
typedef struct AVLNode
{
    uint32_t data;
    int height;
    struct AVLNode* left;
    struct AVLNode* right;
}Node;
#define HEIGHT(node) ((node == NULL)? 0 : (((Node*)(node))->height ))
#define MAX(a,b) ((a > b) ? (a) : (b))

Node* create(uint32_t key);
Node* Insert(Node* tree, uint32_t key);
Node* search(Node* tree, uint32_t key);
Node* del(Node* tree, uint32_t key);
void free_tree(Node* tree);

#endif //MYVM_AVLTREE_H
