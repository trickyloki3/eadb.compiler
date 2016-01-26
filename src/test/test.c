#include "assert.h"
#include "rbt_range.h"

int main(int argc, char * argv[]) {
    struct rbt_range * range1 = NULL;
    struct rbt_range * range2 = NULL;

    assert(0 == rbt_range_init(&range1, 0, 3));
    assert(0 == rbt_range_init(&range2, 5, 7));

    rbt_range_dump(range1, "range1");
    rbt_range_dump(range2, "range2");

    assert(0 == rbt_range_deit(&range2));
    assert(0 == rbt_range_deit(&range1));
    return 0;
}
