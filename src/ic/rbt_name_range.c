#include "rbt_name_range.h"

#define calloc_ptr(x)       (NULL == ((x) = calloc(1, sizeof(*(x)))))
#define free_ptr_null(x)    if(x) { free(x); (x) = NULL; }
#define free_ptr_call(x, y) if(x) { y(&x); }
#define is_nil(x)           ((x) == NULL)
#define is_ptr(x)           ((x) != NULL)

static int rbt_logic_and_copy(struct rbt_logic **, struct rbt_logic *);
static int rbt_logic_or_copy(struct rbt_logic **, struct rbt_logic *);
static int rbt_logic_base(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **, int);
static int rbt_logic_and(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_or(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);

static int str_dup(char * name, char ** buffer, size_t * length) {
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

int rbt_logic_var_init(struct rbt_logic ** logic, char * name, rbt_range * range) {
    struct rbt_logic * object = NULL;

    if( is_nil( logic) ||
        is_ptr(*logic) ||
        calloc_ptr(object) ||
        rbt_range_dup(range, &object->range) ||
        str_dup(name, &object->name, &object->length) ) {
        rbt_logic_deit(&object);
        return 1;
    }

    object->type = var;
    *logic = object;
    return 0;
}

int rbt_logic_var_copy(struct rbt_logic ** object, struct rbt_logic * logic) {
    return rbt_logic_var_init(object, logic->name, logic->range);
}

int rbt_logic_init(struct rbt_logic ** logic, struct rbt_logic * l, struct rbt_logic * r, int type) {
    struct rbt_logic * object = NULL;

    if( is_nil( logic) ||
        is_ptr(*logic) ||
        is_nil(l) ||
        is_nil(r) ||
        calloc_ptr(object) ) {
        rbt_logic_deit(&object);
        return 1;
    }

    object->type = type;
    object->l = l;
    object->r = r;
    l->p = object;
    r->p = object;
    *logic = object;
    return 0;
}

int rbt_logic_deit(struct rbt_logic ** logic) {
    struct rbt_logic * object = NULL;

    if( is_nil( logic) ||
        is_nil(*logic) )
        return 1;

    object = *logic;
    rbt_logic_deit(&object->l);
    rbt_logic_deit(&object->r);
    free_ptr_call(object->range, rbt_range_deit);
    free_ptr_null(object->name);
    free_ptr_null(object);
    *logic = object;

    return 0;
}

int rbt_logic_dump(struct rbt_logic * logic) {
    if(is_nil(logic))
        return 0;

    rbt_logic_dump(logic->l);
    rbt_logic_dump(logic->r);
    return rbt_range_dump(logic->range, logic->name);
}

static int rbt_logic_and_copy(struct rbt_logic ** object, struct rbt_logic * root) {
    rbt_logic * p = NULL;
    rbt_logic * l = NULL;
    rbt_logic * r = NULL;

    if(and == root->type) {
        switch(root->l->type) {
            case var: rbt_logic_var_copy(&l, root->l); break;
            case and: rbt_logic_and_copy(&l, root->l); break;
        }
        switch(root->r->type) {
            case var: rbt_logic_var_copy(&r, root->r); break;
            case and: rbt_logic_and_copy(&r, root->r); break;
        }

        if(rbt_logic_init(&p, l, r, and)) {
            rbt_logic_deit(&l);
            rbt_logic_deit(&r);
            return 1;
        }
    } else {
        return 1;
    }

    *object = p;
    return 0;
}

static int rbt_logic_or_copy(struct rbt_logic ** object, struct rbt_logic * root) {
    rbt_logic * p = NULL;
    rbt_logic * l = NULL;
    rbt_logic * r = NULL;

    if(or == root->type) {
        switch(root->l->type) {
            case var: rbt_logic_var_copy(&l, root->l); break;
            case and: rbt_logic_and_copy(&l, root->l); break;
            case or:  rbt_logic_or_copy(&l, root->l); break;
        }
        switch(root->r->type) {
            case var: rbt_logic_var_copy(&r, root->r); break;
            case and: rbt_logic_and_copy(&r, root->r); break;
            case or:  rbt_logic_or_copy(&r, root->r); break;
        }

        if(rbt_logic_init(&p, l, r, or)) {
            rbt_logic_deit(&l);
            rbt_logic_deit(&r);
            return 1;
        }
    } else {
        return 1;
    }

    *object = p;
    return 0;
}

int rbt_logic_copy(struct rbt_logic ** object, struct rbt_logic * root) {
    switch(root->type) {
        case var: return rbt_logic_var_copy(object, root);
        case and: return rbt_logic_and_copy(object, root);
        case or:  return rbt_logic_or_copy(object, root);
    }
    return 1;
}

static int rbt_logic_base(struct rbt_logic * l, struct rbt_logic * r, struct rbt_logic ** object, int type) {
    rbt_range * range = NULL;
    rbt_logic * logic = NULL;

    if( type == and &&
        0 == strcmp(l->name, r->name)) {
        if( rbt_range_and(l->range, r->range, &range) ||
            rbt_logic_var_init(&logic, l->name, range) )
            goto failed;
    } else {
        if(rbt_logic_init(&logic, l, r, type))
            return 1;
    }

    *object = logic;
    return 0;

failed:
    rbt_range_deit(&range);
    rbt_logic_deit(&logic);
    return 1;
}

static int rbt_logic_and(struct rbt_logic * l, struct rbt_logic * r, struct rbt_logic ** object) {
    switch(l->type) {
        case var:
            switch(r->type) {
                case var: return rbt_logic_base(l, r, object, and);
                case and:
                case or:
                    break;
            }
            break;
        case and:
            switch(r->type) {
                case var:
                case and:
                case or:
                    break;
            }
            break;
        case or:
            switch(r->type) {
                case var:
                case and:
                case or:
                    break;
            }
            break;
    }

    return 1;
}

static int rbt_logic_or(struct rbt_logic * l, struct rbt_logic * r, struct rbt_logic ** object) {
    switch(l->type) {
        case var:
            switch(r->type) {
                case var: return rbt_logic_base(l, r, object, or);
                case and:
                case or:
                    break;
            }
            break;
        case and:
            switch(r->type) {
                case var:
                case and:
                case or:
                    break;
            }
            break;
        case or:
            switch(r->type) {
                case var:
                case and:
                case or:
                    break;
            }
            break;
    }

    return 1;
}

int rbt_logic_op(struct rbt_logic * l, struct rbt_logic * r, struct rbt_logic ** object, int type) {
    switch(type) {
        case and:  return rbt_logic_and(l, r, object);
        case or:   return rbt_logic_or (l, r, object);
    }

    return 1;
}
