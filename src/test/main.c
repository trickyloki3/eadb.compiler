#include "assert.h"
#include "rbt_name_range.h"

int main(int argc, char * argv[]) {
    rbt_logic * logic = NULL;
    rbt_range * range = NULL;
    char * name = "getrefine";

    rbt_range_init(&range, 0, 3, 0);
    rbt_range_add(range, 6, 9, NULL);
    rbt_range_add(range, 12, 15, NULL);
    assert(0 == rbt_logic_var_init(&logic, name, range));
    assert(0 == rbt_logic_var_dump(logic));



    assert(0 == rbt_logic_deit(&logic));
    rbt_range_deit(&range);
    return 0;
}
