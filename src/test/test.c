#include "assert.h"
#include "rbt_range.h"

int main(int argc, char * argv[]) {
    int min;
    int max;
    struct rbt_range * range1 = NULL;
    struct rbt_range * range2 = NULL;
    struct rbt_range * range3 = NULL;
    struct rbt_range * range4 = NULL;
    struct rbt_range * range5 = NULL;
    assert(0 == rbt_range_init(&range1, 0, 3));
    assert(0 == rbt_range_init(&range2, 5, 7));
    assert(0 == rbt_range_copy(range1, &range3));
    assert(0 == rbt_range_negate(range3, &range4));
    assert(0 == rbt_range_or(range1, range2, &range5));

    assert(0 == rbt_range_dump(range1, "range1"));
    assert(0 == rbt_range_dump(range2, "range2"));
    assert(0 == rbt_range_dump(range3, "range3"));
    assert(0 == rbt_range_dump(range4, "range4"));
    assert(0 == rbt_range_dump(range5, "range5"));
    assert(0 == rbt_range_min(range1, &min));
    assert(0 == min);
    assert(0 == rbt_range_max(range1, &max));
    assert(3 == max);

    assert(0 == rbt_range_deit(&range3));
    assert(0 == rbt_range_deit(&range2));
    assert(0 == rbt_range_deit(&range1));
    return 0;
}
