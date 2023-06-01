#ifndef AVL_H
#define AVL_H

#include <stddef.h>
#include <stdlib.h>

typedef struct bst {
    void* val;
    struct bst* left;
    struct bst* right;
    int height;
} t_avl;

#define HEIGHT_THRESHOLD(left, right) 2 == ((NULL == left ? -1 : left->height) - \
                                            (NULL == right ? -1 : right->height))

#define LHEIGHT(n) (NULL == n->left ? -1 : n->left->height)
#define RHEIGHT(n) (NULL == n->right ? -1 : n->right->height)
#define GEI(l, r) ((l) > (r) ? (l) : (r)) + 1
#define GEILR(n) GEI(LHEIGHT(n), RHEIGHT(n))

void* find(t_avl* restrict node, void* val, int (*comparator)(void* left, void* right));
t_avl* add(t_avl* node, void* val, int (*comparator)(void* left, void* right));
t_avl* delete(t_avl* node, void* val, int (*comparator)(void* left, void* right));

#endif

