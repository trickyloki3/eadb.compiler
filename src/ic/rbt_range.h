#ifndef RBT_RANGE_H
#define RBT_RANGE_H
    #include "rbt.h"
    #include <stdio.h>

    struct range {
        int min;
        int max;
    };

    int range_init(struct range **, int min, int max);
    int range_deit(struct range **);

    struct rbt_range {
        rbt_tree * ranges;
        struct range * global;
    };

    int rbt_range_init(struct rbt_range **, int min, int max);
    int rbt_range_deit(struct rbt_range **);
    int rbt_range_dump(struct rbt_range *, char *);
    int rbt_range_insert(struct rbt_range *, int min, int max);
#endif
