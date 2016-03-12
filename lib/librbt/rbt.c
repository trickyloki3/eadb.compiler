#include "rbt.h"

enum { black, red };
#define is_ptr(x)           ((x) != NULL)
#define is_nil(x)           ((x) == NULL)
#define is_root(x)          ((x)->p == NULL)
#define is_lchild(x)        ((x)->p->l == (x))
#define is_rchild(x)        ((x)->p->r == (x))
#define is_black(x)         ((x) == NULL || (x)->c == black)
#define is_red(x)           ((x) != NULL && (x)->c == red)
#define calloc_ptr(x)       (NULL == ((x) = calloc(1, sizeof(*(x)))))
#define calloc_arr(x, n)    (NULL == ((x) = calloc((n), sizeof(*(x)))))
#define free_ptr(x)         if(x) { free(x); (x) = NULL; }
#define last_node(x)        ((x)->next == (x))

/* right rotation on x shifts the tree's height
 * from the left sub tree to the right sub tree
 * by one
 *
 *     p            p
 *     |            |
 *     x            y
 *    / \          / \
 *   y   c   =>   a   x
 *  / \              / \
 * a   b            b   c
 *
 * the left-to-right ordering is preserved
 * y < b < x    ; b remains right of y and left of x
 * a < y < x    ; a remains left  of y and x
 * y < x < c    ; c remains right of x and y
 */
static void rrot(struct rbt_tree * tree, struct rbt_node * x) {
    struct rbt_node * y = x->l;

    /* link x and b */
    x->l = y->r;
    if(x->l)
        x->l->p = x;

    /* link y and p */
    y->p = x->p;
    if(is_root(x))
        tree->root = y;
    else if(is_lchild(x))
        y->p->l = y;
    else
        y->p->r = y;

    /* link y and x */
    x->p = y;
    y->r = x;
}

/* left rotation on  x shifts the tree's height
 * from the right sub tree to the left sub tree
 * by one
 *
 *   p               p
 *   |               |
 *   x               y
 *  / \             / \
 * a   y     =>    x   c
 *    / \         / \
 *   b   c       a   b
 *
 * the left-to-right ordering is preserved
 * x < b < y    ; b remains left  of y and right of x
 * a < x < y    ; a remains left  of x and y
 * x < y < c    ; c remains right of y and x
 */
static void lrot(struct rbt_tree * tree, struct rbt_node * x) {
    struct rbt_node * y = x->r;

    /* link x and b */
    x->r = y->l;
    if(x->r)
        x->r->p = x;

    /* link y and p */
    y->p = x->p;
    if(is_root(x))
        tree->root = y;
    else if(is_lchild(x))
        y->p->l = y;
    else
        y->p->r = y;

    /* link y and x */
    x->p = y;
    y->l = x;
}

/* change the parent of y to
 * p to stage y to replace x
 *     p          p
 *     |          |
 *     x   =>   x |
 *    / \      / \|
 *   l   y    l   y
 */
static void pchg(struct rbt_tree * tree, struct rbt_node * x, struct rbt_node * y) {
    if(is_nil(x->p))
        tree->root = y;
    else if(is_lchild(x))
        x->p->l = y;
    else
        x->p->r = y;

    if(y)
        y->p = x->p;
}

/* circularly link the sub list x and y
 * together to form one list where both
 * ends are connected
 *   _                _
 *  / \              / \
 * 0 = 1 <- x   y-> 2 = 3
 *  \_/              \_/
 *        _________
 *       /         \
 *      0 = 1 = 2 = 3
 *       \_________/
 *
 * swapping x and y  would
 * insert to left or right
 */
static void subl(struct rbt_node * x, struct rbt_node * y) {
    x->next->prev = y->prev;
    y->prev->next = x->next;
    x->next = y;
    y->prev = x;
}

/* remove the node from the doubly
 * and circularly linked list
 *
 *       _________
 *      /         \
 * x-> 0 = 1 = 2 = 3
 *      \_________/
 *
 *     _     _____
 *    \ /   /     \
 * x-> 0   1 = 2 = 3
 *    /_\   \_____/
 */
static void reml(struct rbt_node * x) {
    x->prev->next = x->next;
    x->next->prev = x->prev;
    x->next = x;
    x->prev = x;
}

int rbt_tree_init(struct rbt_tree ** tree) {
    struct rbt_tree * object = NULL;

    if( is_nil( tree) ||
        is_ptr(*tree) ||
        calloc_ptr(object) ||
        rbt_tree_init_static(object) ) {
        rbt_tree_deit(&object);
        return 1;
    }

    *tree = object;
    return 0;
}

int rbt_tree_deit(struct rbt_tree ** tree) {
    struct rbt_tree * object;


    if( is_nil( tree) ||
        is_nil(*tree) )
        return 1;

    object = *tree;
    rbt_tree_deit_static(object);
    free_ptr(object);
    *tree = object;
    return 0;
}

int rbt_node_init(struct rbt_node ** node, int key, void * val) {
    struct rbt_node * object = NULL;

    if( is_nil( node) ||
        is_ptr(*node) ||
        calloc_ptr(object) ||
        rbt_node_init_static(object, key, val) ) {
        rbt_node_deit(&object);
        return 1;
    }

    *node = object;
    return 0;
}

int rbt_node_deit(struct rbt_node ** node) {
    struct rbt_node * object;

    if( is_nil( node) ||
        is_nil(*node) )
        return 1;

    object = *node;
    free_ptr(object);
    *node = object;
    return 0;
}

int rbt_tree_init_static(struct rbt_tree * tree) {
    if(is_nil(tree))
        return 1;

    tree->root = NULL;
    return 0;
}

int rbt_tree_deit_static(struct rbt_tree * tree) {
    struct rbt_node * node;

    if(is_nil(tree))
        return 1;

    while(tree->root) {
        node = tree->root;
        rbt_delete(tree, node);
    }

    return 0;
}

int rbt_node_init_static(struct rbt_node * node, int key, void * val) {
    if(is_nil(node))
        return 1;

    node->key = key;
    node->val = val;
    node->c   = red;

    /* node must be circular link
     * otherwise subl would crash */
    node->next = node;
    node->prev = node;
    return 0;
}

int rbt_node_deit_static(struct rbt_node * node) {
    return 0;
}

int rbt_insert(struct rbt_tree * tree, struct rbt_node * x) {
    struct rbt_node * p;    /* parent */
    struct rbt_node * i;    /* iterator */
    struct rbt_node * s;    /* parent's sibling */

    /* binary search for x's parent */
    p = NULL;
    i = tree->root;
    while(i) {
        p = i;
        i = (x->key < i->key) ? i->l : i->r;
    }

    /* link x with x's parent */
    x->p = p;
    if(is_nil(p)) {
        tree->root = x;
    } else {
        if(x->key < p->key) {
            p->l = x;
            subl(x, p);
        } else {
            p->r = x;
            subl(p, x);
        }
    }

    while(is_red(p)) {
        if(is_lchild(p)) {
            s = p->p->r;
            if(is_red(s)) {
                p->c = black;
                s->c = black;
                x = p->p;
                p = x->p;
                x->c = red;
            } else {
                if(is_rchild(x)) {
                    x = x->p;
                    lrot(tree, x);
                    p = x->p;
                }

                p->c = black;
                p->p->c = red;
                rrot(tree, p->p);
            }
        } else {
            s = p->p->l;
            if(is_red(s)) {
                p->c = black;
                s->c = black;
                x = p->p;
                p = x->p;
                x->c = red;
            } else {
                if(is_lchild(x)) {
                    x = x->p;
                    rrot(tree, x);
                    p = x->p;
                }

                p->c = black;
                p->p->c = red;
                lrot(tree, p->p);
            }
        }
    }

    tree->root->c = black;
    tree->count++;
    return 0;
}

int rbt_delete(struct rbt_tree * tree, struct rbt_node * x) {
    int c;
    struct rbt_node * y;    /* x's replacement */
    struct rbt_node * z;    /* x or y's successor */
    struct rbt_node * p;    /* z's parent */
    struct rbt_node * s;    /* z's sibling */
    struct rbt_node * r = tree->root;

    if(is_nil(x->l)) {
        z = x->r;
        p = x->p;
        c = x->c;
        pchg(tree, x, z);
    } else if(is_nil(x->r)) {
        z = x->l;
        p = x->p;
        c = x->c;
        pchg(tree, x, z);
    } else {
        /* get x's successor */
        y = x->r;
        while(y->l)
            y = y->l;
        c = y->c;

        if(y->p != x) {
            /* get y's successor to replace y */
            z = y->r;
            p = y->p;
            pchg(tree, y, y->r);

            /* y inherits x's right sub tree */
            y->r = x->r;
            y->r->p = y;
        } else {
            /* y is x's successor and immediate right sub tree */
            z = y->r;
            p = y;
        }

        /* y inherits x's parent, left sub tree, and color */
        pchg(tree, x, y);
        y->l = x->l;
        y->l->p = y;
        y->c = x->c;
    }

    if(c == black) {
        while(p && is_black(z)) {
            if(z == p->l) {
                s = p->r;

                if(is_red(s)) {
                    s->c = black;
                    p->c = red;
                    lrot(tree, p);
                    s = p->r;
                }

                if(is_nil(s)) {
                    z = p;
                    p = z->p;
                } else {
                    if(is_black(s->l) && is_black(s->r)) {
                        s->c = red;
                        z = p;
                        p = z->p;
                    } else {
                        if(is_black(s->r)) {
                            s->c = red;
                            s->l->c = black;
                            rrot(tree, s);
                            s = p->r;
                        }

                        s->c = p->c;
                        s->r->c = black;
                        p->c = black;
                        lrot(tree, p);
                        z = r;
                    }
                }
            } else {
                s = p->l;

                if(is_red(s)) {
                    s->c = black;
                    p->c = red;
                    rrot(tree, p);
                    s = p->l;
                }

                if(is_nil(s)) {
                    z = p;
                    p = z->p;
                } else {
                    if(is_black(s->l) && is_black(s->r)) {
                        s->c = red;
                        z = p;
                        p = z->p;
                    } else {
                        if(is_black(s->l)) {
                            s->c = red;
                            s->r->c = black;
                            lrot(tree, s);
                            s = p->l;
                        }

                        s->c = p->c;
                        s->l->c = black;
                        p->c = black;
                        rrot(tree, p);
                        z = r;
                    }
                }
            }
        }
        if(z)
            z->c = black;
    }

    reml(x);
    (x)->l = NULL;
    (x)->r = NULL;
    tree->count--;
    return 0;
}

int rbt_search(struct rbt_tree * tree, struct rbt_node ** x, int key) {
    struct rbt_node * i;

    i = tree->root;
    while(i != NULL) {
        if(key == i->key) {
            *x = i;
            return 0;
        } else if(key < i->key) {
            i = i->l;
        } else {
            i = i->r;
        }
    }

    return 1;
}

int rbt_deploy(struct rbt_tree * tree, rbt_work work, void * data) {
    struct rbt_node * i;
    struct rbt_node * r;

    /* check if empty tree */
    i = tree->root;
    if(is_nil(i))
        return 1;

    /* get leftmost (smallest) node */
    while(!is_nil(i->l))
        i = i->l;


    r = i->prev;
    if(i == r) {
        /* a single node in the tree means  that
         * the single node is both the first and
         * last node */
        if(work(i, data, RBT_FIRST | RBT_LAST))
            return 1;
    } else {
        if(work(i, data, RBT_FIRST))
            return 1;
        i = i->next;

        while(i != r) {
            if(work(i, data, 0))
                return 1;
            i = i->next;
        }

        if(work(i, data, RBT_LAST))
            return 1;
    }

    return 0;
}

int rbt_min(struct rbt_tree * tree, struct rbt_node ** node) {
    struct rbt_node * i = tree->root;

    if(is_nil(i))
        return 1;

    while(!is_nil(i->l))
        i = i->l;

    *node = i;
    return 0;
}

int rbt_max(struct rbt_tree * tree, struct rbt_node ** node) {
    struct rbt_node * i = tree->root;

    if(is_nil(i))
        return 1;

    while(!is_nil(i->r))
        i = i->r;

    *node = i;
    return 0;
}

int rbt_node_cache_init(struct rbt_node_cache ** cache, int count) {
    int i;
    struct rbt_node_cache * object = NULL;

    if( is_nil( cache) ||
        is_ptr(*cache) ||
        0 >= count ||
        calloc_ptr(object) ||
        calloc_arr(object->cache, count)) {
        rbt_node_cache_deit(&object);
        return 1;
    }

    for(i = 0; i < count; i++)
        if(rbt_node_init_static(&object->cache[i], 0, NULL)) {
            rbt_node_cache_deit(&object);
            return 1;
        }

    for(i = 1; i < count; i++)
        subl(&object->cache[i - 1], &object->cache[i]);

    object->free_list = &object->cache[0];
    object->size = (count) * sizeof(struct rbt_node);

    *cache = object;
    return 0;
}

int rbt_node_cache_deit(struct rbt_node_cache ** cache) {
    struct rbt_node_cache * object;

    if( is_nil( cache) ||
        is_nil(*cache) )
        return 1;

    object = *cache;
    free_ptr(object->cache);
    free_ptr(object);
    *cache = object;

    return 0;
}

int rbt_node_cache_alloc(struct rbt_node_cache * cache, struct rbt_node ** node, int key, void * val) {
    struct rbt_node * object = NULL;

    if( is_nil(node) ||
        is_nil(cache) ||
        is_nil(cache->free_list) )
        return 1;

    /* last free node */
    if(last_node(cache->free_list)) {
        object = cache->free_list;
        cache->free_list = NULL;
    } else {
        object = cache->free_list;
        cache->free_list = cache->free_list->next;
    }

    reml(object);
    rbt_node_init_static(object, key, val);
    *node = object;
    return 0;
}


int rbt_node_cache_free(struct rbt_node_cache * cache, struct rbt_node ** node) {
    size_t base = (size_t) *node;
    size_t start = (size_t) cache->cache;
    size_t end = start + cache->size;

    if( is_nil( node) ||
        is_nil(*node) ||
        is_nil(cache) ||
        /* check whether the node is from the cache */
        base < start ||
        base > end)
        return 1;

    if(is_nil(cache->free_list)) {
        cache->free_list = *node;
    } else {
        subl(cache->free_list, *node);
    }

    *node = NULL;
    return 0;
}
