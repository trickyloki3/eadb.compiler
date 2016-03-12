#include "rbt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define MAX_NODE 255

int node_print(struct rbt_node * node, void * data, int flag) {
    if(flag & RBT_FIRST)
        fprintf(stderr, "[RBT]\n");
    (flag & RBT_LAST) ?
        fprintf(stderr, "%d\n", node->key) :
        fprintf(stderr, "%d " , node->key) ;

    return 0;
}

int main(int argc, char * argv[]) {
    int i;
    struct rbt_tree * tree = NULL;
    struct rbt_node * node[MAX_NODE];
    struct rbt_node_cache * cache = NULL;

    assert(0 == rbt_tree_init(&tree));
    memset(node, 0, sizeof(struct rbt_node *) * MAX_NODE);
    assert(0 == rbt_node_cache_init(&cache, MAX_NODE));

    /* test tree insertion */
    for (i = 0; i < MAX_NODE; i++)
        if (rbt_node_cache_alloc(cache, &node[i], i, NULL) ||
            rbt_insert(tree, node[i]) )
            return 1;

    /* test tree traversal */
    rbt_deploy(tree, node_print, NULL);

    /* test tree deletion */
    for (i = 0; i < MAX_NODE; i++)
        if (rbt_delete(tree, node[i]))
            return 1;

    /* test tree traversal */
    rbt_deploy(tree, node_print, NULL);

    /* test tree destructor */
    for (i = 0; i < MAX_NODE; i++)
        if (rbt_insert(tree, node[i]))
            return 1;

    assert(0 == rbt_tree_deit(&tree));

    /* rbt_tree_deit does not free the nodes, only
     * removing the doubly link list and red black
     * tree links */
    for (i = 0; i < MAX_NODE; i++)
        if (rbt_node_cache_free(cache, &node[i]))
            return 1;

    assert(0 == rbt_node_cache_deit(&cache));

    fprintf(stderr, "RBT test completed.\n");
    return 0;
}
