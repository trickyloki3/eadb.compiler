#include "rbt_name_range.h"

#define calloc_ptr(x)       (NULL == ((x) = calloc(1, sizeof(*(x)))))
#define free_ptr_null(x)    if(x) { free(x); (x) = NULL; }
#define free_ptr_call(x, y) if(x) { y(&x); }
#define is_nil(x)           ((x) == NULL)
#define is_ptr(x)           ((x) != NULL)

static str_dup(char * name, char ** buffer, size_t * length) {
    char * _buffer = NULL;
    size_t _length = 0;

    if(is_nil(name))
        return 1;

    /* copy the name */
    _length = strlen(name);
    _buffer = calloc(_length + 1, sizeof(char));
    if(is_nil(_buffer))
        return 1;

    memcpy(_buffer, name, _length);
    *length = _length;
    *buffer = _buffer;
    return 0;
}

int rbt_logic_init(struct rbt_logic ** name_range, char * name, rbt_range * range) {
    struct rbt_logic * object = NULL;

    if( is_nil( name_range) ||
        is_ptr(*name_range) ||
        calloc_ptr(object) ||
        rbt_range_dup(range, &object->range) ||
        str_dup(name, &object->name, &object->length) ) {
        rbt_logic_deit(&object);
        return 1;
    }

    object->type = var;
    *name_range = object;
    return 0;
}

int rbt_logic_deit(struct rbt_logic ** name_range) {
    struct rbt_logic * object = NULL;

    if( is_nil( name_range) ||
        is_nil(*name_range) )
        return 1;

    object = *name_range;
    free_ptr_call(object->range, rbt_range_deit);
    free_ptr_null(object->name);
    free_ptr_null(object);
    *name_range = object;

    return 0;
}
