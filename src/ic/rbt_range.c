#include "rbt_range.h"

#define calloc_ptr(x)       (NULL == ((x) = calloc(1, sizeof(*(x)))))
#define free_ptr_null(x)    if(x) { free(x); (x) = NULL; }
#define free_ptr_call(x, y) if(x) { y(&x); }
#define is_nil(x)           ((x) == NULL)
#define is_ptr(x)           ((x) != NULL)
#define is_last(x)          ((x) == (x)->next)

#ifndef min
#define min(x, y)           ((x < y) ? x : y)
#endif

#ifndef max
#define max(x, y)           ((x < y) ? y : x)
#endif

int range_init(struct range ** range, int min, int max) {
    struct range * object = NULL;

    if( is_nil( range) ||
        is_ptr(*range) ||
        calloc_ptr(object) )
        return 1;

    object->min = min(min, max);
    object->max = max(min, max);
    *range = object;
    return 0;
}

int range_deit(struct range ** range) {
    struct range * object;

    if( is_nil( range) ||
        is_nil(*range) )
        return 1;

    object = *range;
    free_ptr_null(object);
    *range = object;
    return 0;
}

int rbt_range_init(struct rbt_range ** rbt_range, int min, int max) {
    struct rbt_range * object = NULL;

    if( is_nil( rbt_range) ||
        is_ptr(*rbt_range) ||
        calloc_ptr(object) )
        return 1;

    if( rbt_tree_init(&object->ranges) ||
        range_init(&object->global, min, max) ||
        rbt_range_insert(object, min, max) ) {
        rbt_range_deit(&object);
        return 1;
    }

    *rbt_range = object;
    return 0;
}

static int rbt_range_deit_(struct rbt_range * rbt_range) {
    struct rbt_node * r;
    struct rbt_tree * t;

    t = rbt_range->ranges;
    if(t->root) {
        while(!is_last(t->root)) {
            r = t->root;
            /* inefficient; require optimization */
            rbt_delete(t, r);
            free_ptr_call( (struct range *) r->val, range_deit);
            free_ptr_call(r, rbt_node_deit);
        }
        free_ptr_call((struct range *) t->root->val, range_deit);
        free_ptr_call(t->root, rbt_node_deit);
    }

    return 0;
}

int rbt_range_deit(struct rbt_range ** rbt_range) {
    struct rbt_range * object;

    if( is_nil( rbt_range) ||
        is_nil(*rbt_range) )
        return 1;

    object = *rbt_range;
    rbt_range_deit_(object);
    free_ptr_call(object->ranges, rbt_tree_deit);
    free_ptr_call(object->global, range_deit);
    free_ptr_null(object);
    *rbt_range = object;
    return 0;
}

int rbt_range_dump(struct rbt_range * rbt_range, char * tag) {
    rbt_node * i, * r;
    struct range * range;

    fprintf(stderr, "[%s] [Min: %d Max: %d]\n", tag,
    rbt_range->global->min, rbt_range->global->max);

    if(rbt_min(rbt_range->ranges, &r))
        return 1;

    i = r;
    do {
        range = i->val;
        fprintf(stderr, " -> %d - %d\n", range->min, range->max);
        i = i->next;
    } while(i != r);

    return 0;
}

int rbt_range_copy(struct rbt_range * rbt_range_x, struct rbt_range ** rbt_range_y) {
    rbt_node * i, * r;
    struct rbt_range * object = NULL;
    struct range * range = NULL;

    if( is_nil(rbt_range_x) ||
        is_nil(rbt_range_y) ||
        rbt_min(rbt_range_x->ranges, &r) ||
        rbt_range_init(&object, rbt_range_x->global->min, rbt_range_x->global->max) )
        return 1;

    if(!is_last(r)) {
        i = r;
        do {
            range = i->val;
            if(rbt_range_insert(object, range->min, range->max))
                goto failed;
            i = i->next;
        } while(i != r);
    }

    *rbt_range_y = object;
    return 0;
failed:
    rbt_range_deit(&object);
    return 1;
}

int rbt_range_insert(struct rbt_range * rbt_range, int min, int max) {
    struct range * range = NULL;
    struct rbt_node * node = NULL;

    if( range_init(&range, min, max) ||
        rbt_node_init(&node, range->max, range) ||
        rbt_insert(rbt_range->ranges, node) ) {
        /* failed to insert range */
        free_ptr_call(range, range_deit);
        free_ptr_call(node, rbt_node_deit);
        return 1;
    }

    return 0;
}

int rbt_range_min(struct rbt_range * rbt_range, int * min) {
    rbt_node * i;

    if(rbt_min(rbt_range->ranges, &i))
        return 1;

    *min = ((struct range *) (i->val))->min;
    return 0;
}

int rbt_range_max(struct rbt_range * rbt_range, int * max) {
    rbt_node * i;

    if(rbt_max(rbt_range->ranges, &i))
        return 1;

    *max = ((struct range *) (i->val))->max;
    return 0;
}
