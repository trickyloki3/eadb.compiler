#include "assert.h"
#include "rbt_range.h"

void rbt_range_or_test();
void rbt_range_and_test();
void rbt_range_not_test();
void rbt_range_op_test();

int main(int argc, char * argv[]) {
    int min;
    int max;

    struct rbt_range * range1 = NULL;
    struct rbt_range * range2 = NULL;
    struct rbt_range * range3 = NULL;
    struct rbt_range * range4 = NULL;

    /* test the rbt_range interface */
    assert(0 == rbt_range_init(&range1, 0, 3, 0));
    assert(0 == rbt_range_init(&range2, 5, 7, 0));
    assert(0 == rbt_range_dup(range1, &range3));
    assert(0 == rbt_range_neg(range3, &range4));
    assert(0 == rbt_range_min(range1, &min));
    assert(0 == min);
    assert(0 == rbt_range_max(range1, &max));
    assert(3 == max);
    assert(0 == rbt_range_deit(&range4));
    assert(0 == rbt_range_deit(&range3));
    assert(0 == rbt_range_deit(&range2));
    assert(0 == rbt_range_deit(&range1));

    /* test the rbt_range_in */
    rbt_range_init(&range1, 0, 3, 0);
    rbt_range_add(range1, 6, 9, NULL);
    rbt_range_add(range1, 12, 15, NULL);
    rbt_range_add(range1, 18, 21, NULL);
    assert(0 == rbt_range_in(range1, 2));
    assert(0 == rbt_range_in(range1, 6));
    assert(0 == rbt_range_in(range1, 15));
    assert(0 == rbt_range_in(range1, 19));
    assert(1 == rbt_range_in(range1, 4));
    assert(1 == rbt_range_in(range1, 5));
    assert(1 == rbt_range_in(range1, 16));
    rbt_range_deit(&range1);

    /* test logical or cases */
    rbt_range_or_test();

    /* test logical and cases */
    rbt_range_and_test();

    /* test logical not cases */
    rbt_range_not_test();

    /* test operators */
    rbt_range_op_test();

    return 0;
}

void rbt_range_or_test() {
    int min;
    int max;
    struct rbt_range * range1 = NULL;
    struct rbt_range * range2 = NULL;
    struct rbt_range * range3 = NULL;
    struct rbt_range * range4 = NULL;

    /* test individual cases */
    rbt_range_init(&range1, 5, 10, 0);
    rbt_range_init(&range2, 0, 7, 0);
    rbt_range_or(range1, range2, &range3);
    rbt_range_dump(range3, "left overlapped");
    assert(0 == rbt_range_min(range3, &min) && min == 0);
    assert(0 == rbt_range_max(range3, &max) && max == 10);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, 5, 10, 0);
    rbt_range_init(&range2, 7, 15, 0);
    rbt_range_or(range1, range2, &range3);
    rbt_range_dump(range3, "right overlapped");
    assert(0 == rbt_range_min(range3, &min) && min == 5);
    assert(0 == rbt_range_max(range3, &max) && max == 15);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, 5, 10, 0);
    rbt_range_init(&range2, 6, 9, 0);
    rbt_range_or(range1, range2, &range3);
    rbt_range_dump(range3, "outer subset");
    assert(0 == rbt_range_min(range3, &min) && min == 5);
    assert(0 == rbt_range_max(range3, &max) && max == 10);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, 0, 3, 0);
    rbt_range_add(range1, 5, 7, NULL);
    rbt_range_add(range1, 9, 11, NULL);
    rbt_range_init(&range2, 0, 10, 0);
    rbt_range_or(range1, range2, &range3);
    rbt_range_dump(range3, "outer subset many");
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, 5, 10, 0);
    rbt_range_init(&range2, 0, 15, 0);
    rbt_range_or(range1, range2, &range3);
    rbt_range_dump(range3, "inner subset");
    assert(0 == rbt_range_min(range3, &min) && min == 0);
    assert(0 == rbt_range_max(range3, &max) && max == 15);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, 17, 20, 0);
    rbt_range_or(range1, range3, &range4);
    rbt_range_dump(range4, "right disjoint");
    assert(0 == rbt_range_min(range4, &min) && min == 0);
    assert(0 == rbt_range_max(range4, &max) && max == 20);
    rbt_range_deit(&range4);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, -3, -1, 0);
    rbt_range_or(range1, range3, &range4);
    rbt_range_dump(range4, "left disjoint");
    assert(0 == rbt_range_min(range4, &min) && min == -3);
    assert(0 == rbt_range_max(range4, &max) && max == 15);
    rbt_range_deit(&range4);
    rbt_range_deit(&range1);
    rbt_range_deit(&range3);

    rbt_range_init(&range1, 0, 4, 0);
    rbt_range_init(&range2, 5, 9, 0);
    rbt_range_or(range1, range2, &range3);
    rbt_range_dump(range3, "right merge");
    assert(0 == rbt_range_min(range3, &min) && min == 0);
    assert(0 == rbt_range_max(range3, &max) && max == 9);
    assert(1 == range3->ranges->count);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, 5, 9, 0);
    rbt_range_init(&range2, 0, 4, 0);
    rbt_range_or(range1, range2, &range3);
    rbt_range_dump(range3, "left merge");
    assert(0 == rbt_range_min(range3, &min) && min == 0);
    assert(0 == rbt_range_max(range3, &max) && max == 9);
    assert(1 == range3->ranges->count);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);
}

void rbt_range_and_test() {
    int min;
    int max;
    struct rbt_range * range1 = NULL;
    struct rbt_range * range2 = NULL;
    struct rbt_range * range3 = NULL;

    rbt_range_init(&range1, 0, 3, 0);       /* (0, 3) U (5, 7) U (9, 11) */
    rbt_range_add(range1, 5, 7, NULL);
    rbt_range_add(range1, 9, 11, NULL);
    rbt_range_init(&range2, 0, 10, 0);      /* (0, 10) */
    rbt_range_and(range1, range2, &range3);
    rbt_range_dump(range3, "single overlap left");
    assert(0 == rbt_range_min(range3, &min) && min == 0);
    assert(0 == rbt_range_max(range3, &max) && max == 10);
    assert(3 == range3->ranges->count);
    rbt_range_deit(&range3);
    rbt_range_and(range2, range1, &range3);
    rbt_range_dump(range3, "single overlap right");
    assert(0 == rbt_range_min(range3, &min) && min == 0);
    assert(0 == rbt_range_max(range3, &max) && max == 10);
    assert(3 == range3->ranges->count);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, 0, 3, 0);       /* (0, 3) U (5, 7) U (9, 11) */
    rbt_range_add(range1, 5, 7, NULL);
    rbt_range_add(range1, 9, 11, NULL);
    rbt_range_init(&range2, 2, 10, 0);      /* (0, 10) */
    rbt_range_and(range2, range1, &range3);
    rbt_range_dump(range3, "single overlap left (double edge) ");
    assert(0 == rbt_range_min(range3, &min) && min == 2);
    assert(0 == rbt_range_max(range3, &max) && max == 10);
    assert(3 == range3->ranges->count);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, 0, 5, 0);       /* (0, 5) U (10, 15) U (20, 25) */
    rbt_range_add(range1, 10, 15, NULL);
    rbt_range_add(range1, 20, 25, NULL);
    rbt_range_init(&range2, 3, 12, 0);      /* (3, 12) U (14, 23) */
    rbt_range_add(range2, 14, 23, NULL);
    rbt_range_and(range1, range2, &range3);
    rbt_range_dump(range3, "multiple overlap left");
    assert(0 == rbt_range_min(range3, &min) && min == 3);
    assert(0 == rbt_range_max(range3, &max) && max == 23);
    assert(4 == range3->ranges->count);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, 0, 5, 0);       /* (0, 5) U (10, 15) U (20, 25) */
    rbt_range_add(range1, 10, 15, NULL);
    rbt_range_add(range1, 20, 25, NULL);
    rbt_range_init(&range2, 5, 10, 0);      /* (5, 10) U (15, 20) */
    rbt_range_add(range2, 15, 20, NULL);
    rbt_range_and(range1, range2, &range3);
    rbt_range_dump(range3, "multiple edge overlap left");
    assert(0 == rbt_range_min(range3, &min) && min == 5);
    assert(0 == rbt_range_max(range3, &max) && max == 20);
    assert(4 == range3->ranges->count);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);
}

void rbt_range_not_test() {
    int min = 0;
    int max = 0;
    struct rbt_range * range1 = NULL;
    struct rbt_range * range2 = NULL;
    struct rbt_range * range3 = NULL;

    rbt_range_init(&range1, 0, 3, 0);   /* (0, 3) U (6, 9) */
    rbt_range_add(range1, 6, 9, NULL);
    rbt_range_not(range1, &range2);
    rbt_range_dump(range2, "not");
    assert(0 == rbt_range_min(range2, &min) && min == 4);
    assert(0 == rbt_range_max(range2, &max) && max == 5);
    assert(1 == range2->ranges->count);
    rbt_range_not(range2, &range3);
    rbt_range_dump(range3, "not twice");
    assert(0 == rbt_range_min(range3, &min) && min == 0);
    assert(0 == rbt_range_max(range3, &max) && max == 9);
    assert(2 == range3->ranges->count);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    rbt_range_init(&range1, 1, 3, 0);   /* (1, 3) */
    rbt_range_dump(range1, "not zero");
    rbt_range_not(range1, &range2);
    rbt_range_dump(range2, "not zero");
    rbt_range_not(range2, &range3);
    assert(0 == rbt_range_min(range2, &min) && min == 0);
    assert(0 == rbt_range_max(range2, &max) && max == 0);
    assert(1 == range2->ranges->count);
    rbt_range_dump(range3, "not zero twice");
    assert(0 == rbt_range_min(range3, &min) && min == 1);
    assert(0 == rbt_range_max(range3, &max) && max == 3);
    assert(1 == range3->ranges->count);
    rbt_range_deit(&range3);
    rbt_range_deit(&range2);
    rbt_range_deit(&range1);
}

void rbt_range_op_test() {
    int min;
    int max;
    struct rbt_range * range1 = NULL;
    struct rbt_range * range2 = NULL;
    struct rbt_range * range3 = NULL;
        
    /* test the equality and inequality operators */
    rbt_range_init(&range1, -5, -2, 0);
    rbt_range_add(range1, 0, 3, NULL);
    rbt_range_add(range1, 6, 9, NULL);
    rbt_range_add(range1, 12, 15, NULL);
    rbt_range_init(&range2, 0, 5, 0);

    rbt_range_op(range1, range2, &range3, '=' + '=');
    rbt_range_dump(range3, "equality range1");
    assert(0 == rbt_range_min(range3, &min) && min == 0);
    assert(0 == rbt_range_max(range3, &max) && max == 3);
    assert(1 == range3->ranges->count);
    rbt_range_deit(&range3);

    rbt_range_op(range2, range1, &range3, '=' + '=');
    rbt_range_dump(range3, "equality range2");
    assert(0 == rbt_range_min(range3, &min) && min == 0);
    assert(0 == rbt_range_max(range3, &max) && max == 3);
    assert(1 == range3->ranges->count);
    rbt_range_deit(&range3);

    rbt_range_op(range1, range2, &range3, '!' + '=');
    rbt_range_dump(range3, "inequality range1");
    assert(0 == rbt_range_min(range3, &min) && min == -5);
    assert(0 == rbt_range_max(range3, &max) && max == 15);
    assert(2 == range3->ranges->count);
    rbt_range_deit(&range3);

    rbt_range_op(range2, range1, &range3, '!' + '=');
    rbt_range_dump(range3, "inequality range2");
    assert(0 == rbt_range_min(range3, &min) && min == 4);
    assert(0 == rbt_range_max(range3, &max) && max == 5);
    assert(1 == range3->ranges->count);
    rbt_range_deit(&range3);

    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    /* test the relational operators */
    rbt_range_init(&range1, -5, 0, 0);                      /* (-5, 0) U (5, 10) */
    rbt_range_add(range1, 5, 10, NULL);                     
    rbt_range_init(&range2, 0, 5, 0);                       /* (0, 5) */
    
    rbt_range_op(range1, range2, &range3, '<');
    rbt_range_dump(range3, "less than");
    assert(0 == rbt_range_min(range3, &min) && min == -5);
    assert(0 == rbt_range_max(range3, &max) && max ==  0);
    assert(1 == range3->ranges->count);
    rbt_range_deit(&range3);

    rbt_range_op(range1, range2, &range3, '<' + '=');
    rbt_range_dump(range3, "less than or equal to");
    assert(0 == rbt_range_min(range3, &min) && min == -5);
    assert(0 == rbt_range_max(range3, &max) && max == 5);
    assert(2 == range3->ranges->count);
    rbt_range_deit(&range3);

    rbt_range_op(range1, range2, &range3, '>');
    rbt_range_dump(range3, "greater than");
    assert(0 == rbt_range_min(range3, &min) && min == 6);
    assert(0 == rbt_range_max(range3, &max) && max == 10);
    assert(1 == range3->ranges->count);
    rbt_range_deit(&range3);

    rbt_range_op(range1, range2, &range3, '>' + '=');
    rbt_range_dump(range3, "greater than or equal to");
    assert(0 == rbt_range_min(range3, &min) && min == 5);
    assert(0 == rbt_range_max(range3, &max) && max == 10);
    assert(1 == range3->ranges->count);
    rbt_range_deit(&range3);

    rbt_range_deit(&range2);
    rbt_range_deit(&range1);

    /* test the null set */
    rbt_range_init(&range1, 0, 3, 0);                      /* (0, 3) */
    rbt_range_init(&range2, 6, 9, 0);                      /* (6, 9) */

    rbt_range_op(range1, range2, &range3, '>' + '=');
    rbt_range_dump(range3, "null set");
    rbt_range_deit(&range3);

    rbt_range_deit(&range2);
    rbt_range_deit(&range1);
}
