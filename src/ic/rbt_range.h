#ifndef RBT_RANGE_H
#define RBT_RANGE_H
    #include "stdio.h"
    #include "rbt.h"

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

    #define FLAG_RBT_EMPTY  0x1
    #define FLAG_RBT_MAX    0x1

    int rbt_range_init(struct rbt_range **, int, int, int);
    int rbt_range_deit(struct rbt_range **);
    int rbt_range_dump(struct rbt_range *, char *);                                     /* print range set to stderr */
    int rbt_range_min(struct rbt_range *, int *);                                       /* get global min value */
    int rbt_range_max(struct rbt_range *, int *);                                       /* get global max value */
    int rbt_range_add(struct rbt_range *, int, int, struct range **);                   /* add range to range set */
    int rbt_range_dup(struct rbt_range *, struct rbt_range **);                         /* copy range set */
    int rbt_range_neg(struct rbt_range *, struct rbt_range **);                         /* unary -  */
    int rbt_range_or(struct rbt_range *, struct rbt_range *, struct rbt_range **);      /* logical or */
    int rbt_range_and(struct rbt_range *, struct rbt_range *, struct rbt_range **);     /* logical and */
    int rbt_range_not(struct rbt_range *, struct rbt_range **);                         /* logical not */
    int rbt_range_in(struct rbt_range *, int);                                          /* check whether integer in range */
    int rbt_range_op(struct rbt_range *, struct rbt_range *, struct rbt_range **, int); /* support arithmetic, relational, and equality operations */

    typedef struct rbt_range rbt_range;
#endif
