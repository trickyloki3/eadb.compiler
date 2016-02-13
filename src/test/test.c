#include "assert.h"
#include "rbt_range.h"

void rbt_range_or_test();
void rbt_range_and_test();

int main(int argc, char * argv[]) {
    int min;
    int max;

    struct rbt_range * range1 = NULL;
    struct rbt_range * range2 = NULL;
    struct rbt_range * range3 = NULL;
    struct rbt_range * range4 = NULL;

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

    /* test logical or cases */
    rbt_range_or_test();

    /* test logical and cases */
    /*rbt_range_and_test();*/

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


}
