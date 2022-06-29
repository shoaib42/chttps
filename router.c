#include "router.h"
#include <string.h>
#include <stdio.h>

static t_trie routes = {.func = NULL, .glob = 0, .next_level = NULL, .val = NULL};

static int comp(void *l, void *r) {
    return strcmp(((t_trie *)l)->val,((t_trie *)r)->val);
}

static int compstr(void *l, void *r) {
    return strcmp((char *)l,(char *)(((t_trie *)r)->val));
}

int addRoute(char *path, void* (*func)(void *args))
{
    char *p = strdup(path);
    char *token;
    char *dup;
    t_trie *ret = &routes;
    t_trie *tmp;

    printf("path= %s\n", p);

    while(token = strtok_r(p, "/", &p)) {
        tmp = find(ret->next_level, token, compstr);
        if(NULL == tmp) {
            printf("%s was not found\n", token);
            fflush(0);
            dup = strdup(token);
            tmp = (t_trie *)calloc(1,sizeof(t_trie));
            tmp->val = strdup(token);
            if(dup[0]=='{'&&dup[strlen(dup)-1]=='}')
                tmp->glob =1;
            ret->next_level = add(ret->next_level,tmp,comp);
        } else {
            printf("%s was found moving on\n", token);
        }
        ret = tmp;
    }
    ret->func=func;
}

t_trie* resolveRoute(char *path) {
    char *p = strdup(path);
    char *t;
    t_trie *ret = &routes;
    t_trie *tmp;

    printf("Finding route %s\n", p);

    while((t = strtok_r(p, "/", &p)) && ret != NULL) {
        printf("Finding %s\n", t);
        fflush(0);
        ret = find(ret->next_level,t,compstr);
        if(NULL != ret) {
            printf("Found %s\n", t);
            fflush(0);
        }
    }
    return ret;
}

void* awesomeFunction(void *args) {
    printf("my leaf level is %s\n", (char*)args);
}

int main(void)
{
    t_trie *ret = NULL;
    addRoute("/abcd/erfg/def", &awesomeFunction);
    addRoute("/abcd/erfg/def", &awesomeFunction);
    addRoute("/abcd/efg/def", &awesomeFunction);
    addRoute("/abcd/erfg/abracadabra", &awesomeFunction);
    addRoute("/abcd/erfg/def/edf", &awesomeFunction);
    addRoute("/kkk/erfg/def/edf", &awesomeFunction);

    if(NULL != (ret = resolveRoute("/abcd/erfg/abracadabra"))) {
        printf("Found in main!\n");
        printf("Function is null? : %s\n", NULL == ret->func ? "yea" : "nah");
        ret->func(ret->val);
    }

    return 0;
}