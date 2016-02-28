#include "assert.h"
#include "rbt_name_range.h"

int main(int argc, char * argv[]) {
    rbt_logic * logic_1 = NULL;
    rbt_logic * logic_2 = NULL;
    rbt_logic * logic_3 = NULL;
    rbt_logic * logic_4 = NULL;
    rbt_logic * logic_5 = NULL;
    rbt_range * range_1 = NULL;
    rbt_range * range_2 = NULL;
    rbt_range * range_3 = NULL;

    /* test and_var_var */
    rbt_range_init(&range_1, 0, 15, 0);
    rbt_range_init(&range_2, 5, 10, 0);
    rbt_range_init(&range_3, 15, 30, 0);
    rbt_logic_init(&logic_1, "getrefine", range_1);
    rbt_logic_init(&logic_2, "readparam", range_2);
    assert(0 == rbt_logic_op(logic_1, logic_2, &logic_3, and));

    rbt_logic_deit(&logic_2);
    rbt_logic_deit(&logic_1);

    /* test and_var_var merge */
    rbt_logic_init(&logic_1, "getrefine", range_1);
    rbt_logic_init(&logic_2, "getrefine", range_2);
    assert(0 == rbt_logic_op(logic_1, logic_2, &logic_4, and));
    rbt_logic_deit(&logic_2);
    rbt_logic_deit(&logic_1);

    /* test and_and_and */
    assert(0 == rbt_logic_op(logic_3, logic_4, &logic_5, and));
    rbt_logic_dump(logic_5);
    rbt_logic_deit(&logic_5);
    rbt_logic_deit(&logic_4);
    rbt_logic_deit(&logic_3);

    /* test and_and_and unique */
    rbt_logic_init(&logic_1, "a", range_1);
    rbt_logic_init(&logic_2, "b", range_1);
    rbt_logic_op(logic_1, logic_2, &logic_3, and);
    rbt_logic_deit(&logic_2);
    rbt_logic_deit(&logic_1);
    rbt_logic_init(&logic_1, "c", range_1);
    rbt_logic_init(&logic_2, "d", range_1);
    rbt_logic_op(logic_1, logic_2, &logic_4, and);
    rbt_logic_deit(&logic_2);
    rbt_logic_deit(&logic_1);
    rbt_logic_op(logic_3, logic_4, &logic_5, and);
    rbt_logic_dump(logic_5);
    rbt_logic_deit(&logic_5);
    rbt_logic_deit(&logic_4);
    rbt_logic_deit(&logic_3);

    /* test and_or_var / or_or_var */
    rbt_logic_init(&logic_1, "b", range_1);
    rbt_logic_init(&logic_2, "c", range_2);
    rbt_logic_init(&logic_3, "d", range_1);
    rbt_logic_op(logic_2, logic_3, &logic_4, or);
    rbt_logic_op(logic_1, logic_4, &logic_5, or);
    rbt_logic_deit(&logic_4);
    rbt_logic_deit(&logic_3);
    rbt_logic_deit(&logic_2);
    rbt_logic_deit(&logic_1);
    rbt_logic_init(&logic_1, "a", range_1);
    rbt_logic_op(logic_1, logic_5, &logic_2, and);
    rbt_logic_dump(logic_2);
    rbt_logic_deit(&logic_2);
    rbt_logic_deit(&logic_1);
    rbt_logic_deit(&logic_5);

    /* test and_and_or */
    rbt_logic_init(&logic_1, "a", range_1);
    rbt_logic_init(&logic_2, "b", range_2);
    rbt_logic_op(logic_1, logic_2, &logic_3, and);
    rbt_logic_deit(&logic_2);
    rbt_logic_deit(&logic_1);
    rbt_logic_init(&logic_1, "c", range_1);
    rbt_logic_init(&logic_2, "d", range_2);
    rbt_logic_op(logic_1, logic_2, &logic_4, or);
    rbt_logic_deit(&logic_2);
    rbt_logic_deit(&logic_1);
    assert(0 == rbt_logic_op(logic_3, logic_4, &logic_5, and));
    rbt_logic_dump(logic_5);
    rbt_logic_deit(&logic_5);
    rbt_logic_deit(&logic_4);
    rbt_logic_deit(&logic_3);

    /* test or_var_var merge */
    rbt_logic_init(&logic_1, "a", range_1);
    rbt_logic_init(&logic_2, "a", range_3);
    assert(0 == rbt_logic_op(logic_1, logic_2, &logic_3, or));
    rbt_logic_dump(logic_3);
    rbt_logic_deit(&logic_3);
    rbt_logic_deit(&logic_2);
    rbt_logic_deit(&logic_1);

    rbt_range_deit(&range_3);
    rbt_range_deit(&range_2);
    rbt_range_deit(&range_1);
    return 0;
}
