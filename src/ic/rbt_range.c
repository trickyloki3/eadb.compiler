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

#define get_min(x)          (((struct range *) ((x)->val))->min)
#define get_max(x)          (((struct range *) ((x)->val))->max)

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

int rbt_range_init(struct rbt_range ** rbt_range, int min, int max, int flag) {
    struct rbt_range * object = NULL;

    if( is_nil( rbt_range) ||
        is_ptr(*rbt_range) ||
        calloc_ptr(object) )
        return 1;

    if( rbt_tree_init(&object->ranges) ||
        range_init(&object->global, min, max) )
        goto failed;

    if(!flag)
        if(rbt_range_add(object, min, max, NULL))
            goto failed;

    *rbt_range = object;
    return 0;

failed:
    rbt_range_deit(&object);
    return 1;
}

static int rbt_range_deit_(struct rbt_range * rbt_range) {
    struct rbt_node * r;
    struct rbt_tree * t;
    struct range * p;
    t = rbt_range->ranges;
    if(t->root) {
        while(!is_last(t->root)) {
            r = t->root;
            /* inefficient; require optimization */
            rbt_delete(t, r);
            p = r->val;
            free_ptr_call(p, range_deit);
            free_ptr_call(r, rbt_node_deit);
        }
        p = t->root->val;
        free_ptr_call(p, range_deit);
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

int rbt_range_min(struct rbt_range * rbt_range, int * min) {
    rbt_node * node;
    if(rbt_min(rbt_range->ranges, &node))
        return 1;
    *min = get_min(node);
    return 0;
}

int rbt_range_max(struct rbt_range * rbt_range, int * max) {
    rbt_node * node;
    if(rbt_max(rbt_range->ranges, &node))
        return 1;
    *max = get_max(node);
    return 0;
}

int rbt_range_add(struct rbt_range * rbt_range, int min, int max, struct range ** result) {
    struct range * range = NULL;
    struct rbt_node * node = NULL;

    if(rbt_range->global->min > min)
        rbt_range->global->min = min;

    if(rbt_range->global->max < max)
        rbt_range->global->max = max;

    if( range_init(&range, min, max) ||
        rbt_node_init(&node, range->max, range) ||
        rbt_insert(rbt_range->ranges, node) ) {
        /* failed to insert range */
        free_ptr_call(range, range_deit);
        free_ptr_call(node, rbt_node_deit);
        return 1;
    }

    if(result)
        *result = range;
    return 0;
}

int rbt_range_dup(struct rbt_range * rbt_range_x, struct rbt_range ** rbt_range_y) {
    rbt_node * i, * r;
    struct rbt_range * object = NULL;

    if( rbt_min(rbt_range_x->ranges, &r) ||
        rbt_range_init(&object, rbt_range_x->global->min, rbt_range_x->global->max, FLAG_RBT_EMPTY) )
        goto failed;

    i = r;
    do {
        if(rbt_range_add(object, get_min(i), get_max(i), NULL))
            goto failed;
        i = i->next;
    } while(i != r);

    *rbt_range_y = object;
    return 0;

failed:
    rbt_range_deit(&object);
    return 1;
}

int rbt_range_neg(struct rbt_range * rbt_range_x, struct rbt_range ** rbt_range_y) {
    int min;
    int max;
    rbt_node * i, * r;
    struct rbt_range * object = NULL;

    min = rbt_range_x->global->max * -1;
    max = rbt_range_x->global->min * -1;

    if( rbt_min(rbt_range_x->ranges, &r) ||
        rbt_range_init(&object, min, max, FLAG_RBT_EMPTY) )
        goto failed;

    i = r;
    do {
        min = -1 * get_max(i);
        max = -1 * get_min(i);
        if(rbt_range_add(object, min, max, NULL))
            goto failed;
        i = i->prev;
    } while(i != r);

    *rbt_range_y = object;
    return 0;

failed:
    rbt_range_deit(&object);
    return 1;
}

typedef int (*rbt_range_merge_func) (struct rbt_range *, struct range **, struct range *);

static int rbt_range_merge(struct rbt_range * rbt_range, struct rbt_node * xr, struct rbt_node * yr, rbt_range_merge_func merge) {
    rbt_node * xi = xr;
    rbt_node * yi = yr;
    struct range * range_next = NULL;
    struct range * range_last = NULL;

    /* merge sort algorithm for merging ranges */
    while(xi && yi) {
        if(get_min(xi) < get_min(yi)) {
            range_next = xi->val;
            xi = xi->next;
            if(xi == xr)
                xi = NULL;
        } else {
            range_next = yi->val;
            yi = yi->next;
            if(yi == yr)
                yi = NULL;
        }

        if(merge(rbt_range, &range_last, range_next))
            return 1;
    }

    while(xi) {
        range_next = xi->val;
        xi = xi->next;
        if(xi == xr)
            xi = NULL;
        if(merge(rbt_range, &range_last, range_next))
            return 1;
    }

    while(yi) {
        range_next = yi->val;
        yi = yi->next;
        if(yi == yr)
            yi = NULL;

        if(merge(rbt_range, &range_last, range_next))
            return 1;
    }

    return 0;
}

static int rbt_range_or_next(struct rbt_range * rbt_range, struct range ** last, struct range * next) {
    if(is_nil(*last)) {
        if (rbt_range_add(rbt_range, next->min, next->max, last))
            return 1;
    } else {
        if(next->min > (*last)->max + 1) {
            if(rbt_range_add(rbt_range, next->min, next->max, last))
                return 1;
        } else {
            (*last)->min = min((*last)->min, next->min);
            (*last)->max = max((*last)->max, next->max);
        }
    }
    return 0;
}

int rbt_range_or(struct rbt_range * rbt_range_x, struct rbt_range * rbt_range_y, struct rbt_range ** rbt_range_z) {
    int min, max;
    rbt_node * xr;
    rbt_node * yr;
    struct rbt_range * object = NULL;

    if( is_nil(rbt_range_x) ||
        is_nil(rbt_range_y) )
        return 1;

    min = min(rbt_range_x->global->min, rbt_range_y->global->min);
    max = max(rbt_range_x->global->max, rbt_range_y->global->max);

    if( rbt_range_init(&object, min, max, FLAG_RBT_EMPTY) ||
        rbt_min(rbt_range_x->ranges, &xr) ||
        rbt_min(rbt_range_y->ranges, &yr) ||
        rbt_range_merge(object, xr, yr, rbt_range_or_next) )
        goto failed;

    if(!object->ranges->count) {
        if(rbt_range_add(object, 0, 0, 0))
            goto failed;
        object->global->min = min;
        object->global->max = max;
    }

    *rbt_range_z = object;
    return 0;

failed:
    free_ptr_call(object, rbt_range_deit);
    return 1;
}

static int rbt_range_and_next(struct rbt_range * rbt_range, struct range ** last, struct range * next) {
    int min, max;
    if(is_nil(*last)) {
        *last = next;
    } else {
        if( (*last)->min <= next->max &&
            (*last)->max >= next->min ) {
            min = max((*last)->min, next->min);
            max = min((*last)->max, next->max);
            if(rbt_range_add(rbt_range, min, max, NULL))
                return 1;
        } else {
            if( next->min <= (*last)->max &&
                next->max >= (*last)->min ) {
                min = max((*last)->min, next->min);
                max = min((*last)->max, next->max);
                if(rbt_range_add(rbt_range, min, max, NULL))
                    return 1;
            }
        }

        if((*last)->max < next->max)
            *last = next;
    }
    return 0;
}

int rbt_range_and(struct rbt_range * rbt_range_x, struct rbt_range * rbt_range_y, struct rbt_range ** rbt_range_z) {
    int min, max;
    rbt_node * xr;
    rbt_node * yr;
    struct rbt_range * object = NULL;

    if( is_nil(rbt_range_x) ||
        is_nil(rbt_range_y) )
        return 1;

    min = max(rbt_range_x->global->min, rbt_range_y->global->min);
    max = min(rbt_range_x->global->max, rbt_range_y->global->max);

    if( rbt_range_init(&object, min, max, FLAG_RBT_EMPTY) ||
        rbt_min(rbt_range_x->ranges, &xr) ||
        rbt_min(rbt_range_y->ranges, &yr) ||
        rbt_range_merge(object, xr, yr, rbt_range_and_next) )
        goto failed;

    if(!object->ranges->count) {
        if(rbt_range_add(object, 0, 0, 0))
            goto failed;
        object->global->min = min;
        object->global->max = max;
    }

    *rbt_range_z = object;
    return 0;

failed:
    free_ptr_call(object, rbt_range_deit);
    return 1;
}

int rbt_range_not(struct rbt_range * rbt_range_x, struct rbt_range ** rbt_range_y) {
    int min;
    int max;
    rbt_node * root;
    rbt_node * iter;
    struct range * last = NULL;
    struct rbt_range * object = NULL;

    if( is_nil(rbt_range_x) ||
        rbt_min(rbt_range_x->ranges, &root) )
        return 1;

    min = rbt_range_x->global->min;
    max = rbt_range_x->global->max;

    if( is_last(root) &&
        get_min(root) == 0 &&
        get_max(root) == 0) {
        return rbt_range_init(rbt_range_y, min, max, 0);
    } else if(rbt_range_init(&object, min, max, FLAG_RBT_EMPTY)) {
        return 1;
    }

    if(min < get_min(root))
        if(rbt_range_add(object, min, get_min(root) - 1, &last))
            goto failed;

    if(!is_last(root)) {
        iter = root;
        do {
            if(rbt_range_add(object, get_max(iter) + 1, get_min(iter->next) - 1, &last))
                goto failed;

            iter = iter->next;
        } while(iter->next != root);
    }

    if(get_max(root->prev) < max)
        if(rbt_range_add(object, get_max(root->prev) + 1, max, &last))
            goto failed;

    if(is_nil(last)) {
        if(rbt_range_add(object, 0, 0, &last)) {
            goto failed;
        } else {
            /* rbt_range_add modified the global */
            object->global->min = min;
            object->global->max = max;
        }
    }

    *rbt_range_y = object;
    return 0;

failed:
    rbt_range_deit(&object);
    return 1;
}

int rbt_range_in(struct rbt_range * rbt_range, int key) {
    rbt_node * i;

    /* modified version of rbt_search() */
    i = rbt_range->ranges->root;
    while(i != NULL) {
        if( key >= get_min(i) &&
            key <= get_max(i) ) {
            return 0;
        } else if(key < get_max(i)) {
            i = i->l;
        } else {
            i = i->r;
        }
    }

    return 1;
}

/* advisable to divide and conquer each operator's implementation but this simplifies the interface :D */
int rbt_range_op(struct rbt_range * rbt_range_x, struct rbt_range * rbt_range_y, struct rbt_range ** rbt_range_z, int operator) {
    int xmin, ymin;
    int xmax, ymax;
    int rmin, rmax;
    rbt_node * iter = NULL;
    rbt_node * root = NULL;
    struct range * last = NULL;
    struct rbt_range * range_1 = NULL;
    struct rbt_range * result = NULL;

    /* relational operators are delegated */
    switch(operator) {
        case '&' + '&': return rbt_range_and(rbt_range_x, rbt_range_y, rbt_range_z);
        case '|' + '|': return rbt_range_or (rbt_range_x, rbt_range_y, rbt_range_z);
    }

    /* equality operators map to logical AND */
    switch(operator) {
        case '=' + '=':
        case '!' + '=':
            /* compute x == y */
            if(rbt_range_and(rbt_range_x, rbt_range_y, &range_1)) {
                goto failed;
            } else {
                range_1->global->min = rbt_range_x->global->min;
                range_1->global->max = rbt_range_x->global->max;
            }

            /* compute !(x == y), which is x != y */
            if(operator == '!' + '=') {
                if(rbt_range_not(range_1, &result))
                    goto failed;
            } else {
                result = range_1;
                range_1 = NULL;
            }
            goto passed;
    }

    /* get the min and max of x and y */
    if( rbt_range_min(rbt_range_x, &xmin) ||
        rbt_range_max(rbt_range_x, &xmax) ||
        rbt_range_min(rbt_range_y, &ymin) ||
        rbt_range_max(rbt_range_y, &ymax) )
        goto failed;

    switch(operator) {
        case '<':
            /* compute x < y */
            ymax--;
        case '<' + '=':
            /* compute x <= y */
            if( ((ymax < xmin) ?
                   rbt_range_init(&range_1, 0, 0, 0) :
                   rbt_range_init(&range_1, xmin, ymax, 0)) ||
                rbt_range_and(rbt_range_x, range_1, &result) ) {
                goto failed;
            } else {
                result->global->min = rbt_range_x->global->min;
                result->global->max = rbt_range_x->global->max;
            }
            goto passed;
        case '>':
            /* compute x > y */
            ymax++;
        case '>' + '=':
            /* compute x >= y */
            if( ((xmax < ymax) ?
                    rbt_range_init(&range_1, 0, 0, 0) :
                    rbt_range_init(&range_1, ymax, xmax, 0)) ||
                rbt_range_and(rbt_range_x, range_1, &result) ) {
                goto failed;
            } else {
                result->global->min = rbt_range_x->global->min;
                result->global->max = rbt_range_x->global->max;
            }
            goto passed;
    }

    /* build the initial object */
    switch(operator) {
        case '*':
            rmin = xmin * ymin;
            rmax = xmax * ymax;
            break;
        case '/':
            rmin = ymin ? (xmin / ymin) : 0;
            rmax = ymax ? (xmax / ymax) : 0;
            break;
        case '+':
            rmin = xmin + ymin;
            rmax = xmax + ymax;
            break;
        case '-':
            rmin = xmin - ymin;
            rmax = xmax - ymax;
            break;
        case '&':
            rmin = xmin & ymin;
            rmax = xmax & ymax;
            break;
        case '|':
            rmin = xmin | ymin;
            rmax = xmax | ymax;
            break;
        default:
            goto failed;
    }
    if(rbt_range_init(&result, rmin, rmax, FLAG_RBT_EMPTY))
        goto failed;

    /* get the root of the x */
    if(rbt_min(rbt_range_x->ranges, &root))
        goto failed;
    else
        iter = root;

    do {
        /* the operation is commutative */
        switch(operator) {
            case '*':
                rmin = get_min(iter) * ymin;
                rmax = get_max(iter) * ymax;
                break;
            case '/':
                rmin = ymin ? (get_min(iter) / ymin) : 0;
                rmax = ymax ? (get_max(iter) / ymax) : 0;
                break;
            case '+':
                rmin = get_min(iter) + ymin;
                rmax = get_max(iter) + ymax;
                break;
            case '-':
                rmin = get_min(iter) - ymin;
                rmax = get_max(iter) - ymax;
                break;
            case '&':
                rmin = get_min(iter) & ymin;
                rmax = get_max(iter) & ymax;
                break;
            case '|':
                rmin = get_min(iter) | ymin;
                rmax = get_max(iter) | ymax;
                break;
            default:
                goto failed;
        }

        if(is_nil(last)) {
            if(rbt_range_add(result, rmin, rmax, &last))
                goto failed;
        } else {
            if(rmin <= last->max) {
                last->max = rmax;
            } else if(rbt_range_add(result, rmin, rmax, &last)) {
                goto failed;
            }
        }

        iter = iter->next;
    } while(iter != root);

passed:
    rbt_range_deit(&range_1);
    *rbt_range_z = result;
    return 0;

failed:
    rbt_range_deit(&range_1);
    rbt_range_deit(&result);
    return 1;
}

int rbt_range_work(struct rbt_range * rbt_range, rbt_work work, void * context) {
    return rbt_deploy(rbt_range->ranges, work, context);
}
