#ifndef RBT_NAME_RANGE_H
#define RBT_NAME_RANGE_H
    #include "stdio.h"
    #include "string.h"
    #include "rbt_range.h"

    /* rbt_name_range's implementation is extremely
     * inefficient in certain cases where sub-trees
     * are copied many times over, but it is easier
     * to implement it this way.
     *
     * also this implementation uses excessive extra
     * and and or nodes to preserve the structure of
     * the logical expression, which is unnecessary,
     * but makes the implementation easier.
     * :D  */

    enum {
        var,
        and,
        or,
        not
    };

    struct rbt_logic {
        int id;
        int type;               /* var, and, or */
        char * name;
        size_t length;
        rbt_range * range;
        struct rbt_logic * p; /* parent */
        struct rbt_logic * l; /* left */
        struct rbt_logic * r; /* right */

        struct rbt_logic * next;
        struct rbt_logic * prev;
    };

    int rbt_logic_init(struct rbt_logic **, char *, rbt_range *, int);
    int rbt_logic_deit(struct rbt_logic **);
    int rbt_logic_copy(struct rbt_logic **, struct rbt_logic *);
    int rbt_logic_dump(struct rbt_logic *);
    int rbt_logic_append(struct rbt_logic *, struct rbt_logic *);
    int rbt_logic_remove(struct rbt_logic *);
    int rbt_logic_op(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **, int);
    int rbt_logic_not_all(struct rbt_logic *, struct rbt_logic **);
    int rbt_logic_search(struct rbt_logic *, const char *, rbt_range **, int);

    typedef struct rbt_logic rbt_logic;
#endif
