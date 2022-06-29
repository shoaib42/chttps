#include "avl.h"
// typedef struct trie {
//     char *level;
//     t_avl *next_level;
// } t_trie;
typedef void* (*t_func)(void *arg);
typedef struct level {
    char *val;
    t_avl *next_level;
    t_func func;
    char glob;
} t_trie;