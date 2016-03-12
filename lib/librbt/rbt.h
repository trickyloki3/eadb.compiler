#ifndef RBT_H
#define RBT_H
    #include "stdlib.h"

    struct rbt_node {
        int key;                /* key */
        void * val;             /* value */

        /* red black tree - O(lgn) search */
        int c;                  /* color */
        struct rbt_node * l;    /* left */
        struct rbt_node * r;    /* right */
        struct rbt_node * p;    /* parent */

        /* doubly linked list - O(n) traversal */
        struct rbt_node * prev;
        struct rbt_node * next;
    };

    struct rbt_tree {
        struct rbt_node * root; /* root node */
        int count;              /* total number of nodes */
    };

    #define RBT_FIRST   0x1
    #define RBT_LAST    0x2
    typedef int (*rbt_work) (struct rbt_node *, void *, int);

    /* constructors and destructors */
    int rbt_tree_init(struct rbt_tree **);
    int rbt_tree_deit(struct rbt_tree **);
    int rbt_node_init(struct rbt_node **, int, void *);
    int rbt_node_deit(struct rbt_node **);

    /* static constructors and destructors */
    int rbt_tree_init_static(struct rbt_tree *);
    int rbt_tree_deit_static(struct rbt_tree *);
    int rbt_node_init_static(struct rbt_node *, int, void *);
    int rbt_node_deit_static(struct rbt_node *);

    /* basic operations on red black tree */
    int rbt_insert(struct rbt_tree *, struct rbt_node *);
    int rbt_delete(struct rbt_tree *, struct rbt_node *);
    int rbt_search(struct rbt_tree *, struct rbt_node **, int);
    int rbt_deploy(struct rbt_tree *, rbt_work, void *);

    int rbt_min(struct rbt_tree *, struct rbt_node **);
    int rbt_max(struct rbt_tree *, struct rbt_node **);

    /* red black node cache (aka lookaside list)
     * help manage and recycle red black node and
     * improve performance by avoiding the system
     * allocator, i.e. calloc and free. */
    struct rbt_node_cache {
        size_t size;
        struct rbt_node * cache;        /* array of red black node */
        struct rbt_node * free_list;    /* root node to free list */
    };

    int rbt_node_cache_init(struct rbt_node_cache **, int);
    int rbt_node_cache_deit(struct rbt_node_cache **);
    int rbt_node_cache_alloc(struct rbt_node_cache *, struct rbt_node **, int, void *);
    int rbt_node_cache_free(struct rbt_node_cache *, struct rbt_node **);

    /* cache aliases */
    typedef struct rbt_tree rbt_tree;
    typedef struct rbt_node rbt_node;
    typedef struct rbt_node_cache rbt_node_cache;
#endif
