# librbt

A simple red black tree implementation in C with MIT License.

```
Each key must be an (int).
 * I avoided (void *) to protect you from you. ~.~
 * As for (char *) keys, find a hash function.

Each node has a parent pointer.
 * Does away an implicit or explicit stack.

Each node is part of a doubly linked list.
 * An extra 10 lines of code for fast traversal.

#include "rbt.h"
#include <stdio.h>

int agent(struct rbt_node * node, void * data, int flag) {
    printf("%d -> %p\n", node->key, node->val);
    return 0;
}

int main(int argc, char * argv[]) {
    struct rbt_tree * tree = NULL;
    struct rbt_node * node = NULL;
    struct rbt_node * find = NULL;

    rbt_tree_init(&tree);
    rbt_node_init(&node, 1, NULL);
    rbt_insert(tree, node);
    rbt_search(tree, &find, 1);
    rbt_deploy(tree, agent, NULL);
    rbt_delete(tree, node);
    rbt_node_deit(&node);
    rbt_tree_deit(&tree);
}

```
