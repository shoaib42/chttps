#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "avl.h"

typedef struct m {
    int a;
    char b;
} t_m;

int comp(void *l, void *r) {
    return strcmp(l,r);
}


int compFind(void *l, void *r) {
    return strcmp(l,r);
}

int printTree(t_avl *n, int padding) {
    int i = 0;
    padding += 10; 
    if(n->right != NULL) printTree(n->right, padding);
    printf("%*s",padding,"");
    printf("%s\n\n", n->val);
    if(n->left != NULL) printTree(n->left, padding);
}

int main(void) {
    const char *notexists = "no";
    const char *fname = "teststr.txt";
    FILE *f = fopen(fname, "r");
    char *tmp = NULL;

    t_avl *root = NULL;

    struct stat sb;
    stat(fname, &sb);
    char *line = malloc(sb.st_size);
    while (EOF != fscanf(f, "%[^\n] ", line)) {
        tmp = strdup(line);
        root = add(root,tmp, &comp);
        printTree(root, 0);
        printf("\n\n\n\n\n\n");
    }

    rewind(f);


    while (EOF != fscanf(f, "%[^\n] ", line)) {
        tmp = find(root, line, &compFind);
        if (tmp != NULL) {
            printf("Found l=%s, t_mp=%s\n", line, tmp);
        } else {
            printf("Not Found %s\n", line);
        }
    }

    tmp = find(root, notexists, &compFind);
    if (tmp != NULL) {
        printf("Found l=%s, t_mp=%s\n", notexists, tmp);
    } else {
        printf("Not Found %s\n", notexists);
    }

    fclose(f);


    return 0;
}