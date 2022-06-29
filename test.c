#include <stdio.h>
#include "avl.h"

typedef struct m {
    int a;
    char b;
} t_m;

int comp(void *l, void *r) {
    return ((t_m *)l)->a - ((t_m *)r)->a;
}


int compFind(void *l, void *r) {
    return *(int *)l - ((t_m *)r)->a;
}

int printTree(t_avl *n, int padding) {
    int i = 0;
    padding += 10; 
    if(n->right != NULL) printTree(n->right, padding);
    printf("%*s",padding,"");
    printf("%d\n", ((t_m *)(n->val))->a);
    if(n->left != NULL) printTree(n->left, padding);
}

int main(void) {

    t_avl *root = NULL;
    t_m *tmp = NULL;
    int i = 0;
    for(i = 0; i < 15 ; i ++) {
        tmp = (t_m *)malloc(sizeof(t_m));
        tmp->a = i;
        printf("adding %d\n", i);
        root = add(root,tmp, &comp);
        printTree(root, 0);
        printf("\n\n\n");
    }

    for ( i = 0 ; i < 20 ; i++ ) {
        tmp = find(root, &i, &compFind);
        if (tmp != NULL) {
            printf("Found i=%d, t_mp->a=%d\n", i, tmp->a);
        } else {
            printf("Not Found %d\n", i);
        }
    }

    return 0;
}