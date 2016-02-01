#ifndef RBT_RANGE_H
#define RBT_RANGE_H
    #include "rbt.h"
    #include <stdio.h>

    struct range {
        int min;
        int max;
    };

    int range_init(struct range **, int, int);
    int range_deit(struct range **);

    struct rbt_range {
        rbt_tree * ranges;
        struct range * global;
    };

    int rbt_range_init(struct rbt_range **, int, int);
    int rbt_range_deit(struct rbt_range **);
    int rbt_range_dump(struct rbt_range *, char *);
    int rbt_range_copy(struct rbt_range *, struct rbt_range **);
    int rbt_range_delete(struct rbt_range *, struct rbt_node *);
    int rbt_range_min(struct rbt_range *, int *);
    int rbt_range_max(struct rbt_range *, int *);
    int rbt_range_negate(struct rbt_range *, struct rbt_range **);

    int rbt_range_add(struct rbt_range *, int, int, struct range **);
    int rbt_range_or(struct rbt_range *, struct rbt_range *, struct rbt_range **);
#endif
