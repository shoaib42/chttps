#include "avl.h"

static inline t_avl* rot_l(t_avl* n) {
    int l, r;
    t_avl* t = n->left;

    n->left = t->right;
    t->right = n;
    r = n->height = GEILR(n);
    l = LHEIGHT(t);
    t->height = GEI(l, r);
    return t;
}

static inline t_avl* rot_r(t_avl* n) {
    int l, r;
    t_avl* t = n->right;

    n->right = t->left;
    t->left = n;
    r = n->height = GEILR(n);
    l = RHEIGHT(t);
    t->height = GEI(l, r);
    return t;
}

static inline t_avl* d_l(t_avl* n) {
    int l, r;
    t_avl* nl = n->left;
    t_avl* t = nl->right;

    nl->right = t->left;
    t->left = nl;
    r = nl->height = GEILR(n);
    l = RHEIGHT(t);
    t->height = GEI(l, r);

    n->left = t;
    nl = n;

    t = nl->left;
    nl->left = t->right;
    t->right = nl;
    r = nl->height = GEILR(nl);
    l = LHEIGHT(t);
    t->height = GEI(l, r);

    return t;
}

static inline t_avl* d_r(t_avl* n) {
    int l, r;
    t_avl* nr = n->right;
    t_avl* t = nr->left;

    nr->left = t->right;
    t->right = nr;
    r = nr->height = GEILR(nr);
    l = LHEIGHT(t);
    t->height = GEI(l, r);

    n->right = t;
    nr = n;

    t = nr->right;
    nr->right = t->left;
    t->left = nr;
    r = nr->height = GEILR(nr);
    l = RHEIGHT(t);
    t->height = GEI(l, r);

    return t;
}

void* find(t_avl* restrict node, void* val, int (*comparator)(void* left, void* right)) {
    int comp;
    while (node != NULL) {
        comp = comparator(val, node->val);
        if (comp < 0) {
            node = node->left;
        } else if (comp > 0) {
            node = node->right;
        } else {
            return node->val;
        }
    }
    return NULL;
}

t_avl* add(t_avl* node, void* val, int (*comparator)(void* left, void* right)) {
    int comp;
    if (NULL == node) {
        node = (t_avl*)malloc(sizeof(t_avl));
        node->height = 0;
        node->left = NULL;
        node->right = NULL;
        node->val = val;
    } else {
        comp = comparator(val, node->val);
        if (comp < 0) {
            node->left = add(node->left, val, comparator);
            if (HEIGHT_THRESHOLD(node->left, node->right)) {
                if (comparator(val, node->val) < 0) {
                    node = rot_l(node);
                } else {
                    node = d_l(node);
                }
            }
        } else if (comp > 0) {
            node->right = add(node->right, val, comparator);
            if (HEIGHT_THRESHOLD(node->right, node->left)) {
                if (comparator(val, node->val) > 0) {
                    node = rot_r(node);
                } else {
                    node = d_r(node);
                }
            }
        } else {
            node->val = val;
        }
    }
    node->height = GEILR(node);
    return node;
}

t_avl* delete(t_avl* node, void* val, int (*comparator)(void* left, void* right)) {
    if (node == NULL) {
        return NULL;
    }

    int comp = comparator(val, node->val);
    if (comp < 0) {
        node->left = delete(node->left, val, comparator);
        node->height = GEILR(node);
        if (RHEIGHT(node) - LHEIGHT(node) == 2) {
            if (RHEIGHT(node->right) < LHEIGHT(node->left)) {
                node = d_r(node);
            } else {
                node = rot_r(node);
            }
        }
    } else if (comp > 0) {
        node->right = delete(node->right, val, comparator);
        node->height = GEILR(node);
        if (LHEIGHT(node) - RHEIGHT(node) == 2) {
            if (LHEIGHT(node->left) < RHEIGHT(node->right)) {
                node = d_l(node);
            } else {
                node = rot_l(node);
            }
        }
    } else {
        if (node->left == NULL && node->right == NULL) {
            free(node);
            return NULL;
        } else if (node->left == NULL) {
            t_avl* temp = node->right;
            free(node);
            return temp;
        } else if (node->right == NULL) {
            t_avl* temp = node->left;
            free(node);
            return temp;
        } else {
            t_avl* successor = node->right;
            while (successor->left != NULL) {
                successor = successor->left;
            }
            node->val = successor->val;
            node->right = delete(node->right, successor->val, comparator);
            node->height = GEILR(node);
            if (LHEIGHT(node) - RHEIGHT(node) == 2) {
                if (LHEIGHT(node->left) < RHEIGHT(node->right)) {
                    node = d_l(node);
                } else {
                    node = rot_l(node);
                }
            }
        }
    }

    return node;
}

