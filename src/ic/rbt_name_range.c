#include "rbt_name_range.h"

#define calloc_ptr(x)       (NULL == ((x) = calloc(1, sizeof(*(x)))))
#define free_ptr_null(x)    if(x) { free(x); (x) = NULL; }
#define free_ptr_call(x, y) if(x) { y(&x); }
#define is_nil(x)           ((x) == NULL)
#define is_ptr(x)           ((x) != NULL)

static int rbt_logic_and_copy(struct rbt_logic **, struct rbt_logic *);
static int rbt_logic_or_copy(struct rbt_logic **, struct rbt_logic *);
static int rbt_logic_and_search(struct rbt_logic *, char *, struct rbt_logic **);
static int rbt_logic_and_and_and_re(struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_and_and_and(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_and_var_and(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_and_var_or(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_and_and_or(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_and_or_or(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_or_var_and(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_or_and_and(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_or_var_or(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_or_and_or(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
static int rbt_logic_or_or_or(struct rbt_logic *, struct rbt_logic *, struct rbt_logic **);
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
        is_nil(name) ||
        is_nil(range) ||
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
    if(var != logic->type)
        return 1;

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

    if(logic->type != var)
        fprintf(stderr, "[%s]\n", logic->type == and ? "and" : "or");

    rbt_logic_dump(logic->l);
    rbt_logic_dump(logic->r);

    if(logic->range)
        return rbt_range_dump(logic->range, logic->name);

    return 0;
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

static int rbt_logic_and_search(struct rbt_logic * root, char * name, struct rbt_logic ** object) {
    if(var == root->type && 0 == strcmp(root->name, name)) {
        *object = root;
    } else {
        if(root->l && !(*object))
            rbt_logic_and_search(root->l, name, object);
        if(root->r && !(*object))
            rbt_logic_and_search(root->r, name, object);
    }

    return *object ? 0 : 1;
}

static int rbt_logic_and_and_and_re(struct rbt_logic * l, struct rbt_logic ** object) {
    rbt_logic * logic = NULL;
    rbt_logic * result = NULL;
    rbt_range * range = NULL;

    if(var == l->type) {
        if(rbt_logic_and_search(*object, l->name, &logic)) {
            if( rbt_logic_var_copy(&logic, l) ||
                rbt_logic_init(&result, logic, *object, and) ) {
                free_ptr_call(logic, rbt_logic_deit);
                return 1;
            }

            *object = result;
        } else {
            if(rbt_range_and(logic->range, l->range, &range))
                return 1;

            rbt_range_deit(&logic->range);
            logic->range = range;
        }

        return 0;
    }

    return  rbt_logic_and_and_and_re(l->l, object) ||
            rbt_logic_and_and_and_re(l->r, object);
}

static int rbt_logic_and_and_and(struct rbt_logic * l_and, struct rbt_logic * r_and, struct rbt_logic ** object) {
    return  rbt_logic_and_copy(object, l_and) ||
            rbt_logic_and_and_and_re(r_and, object);
}

static int rbt_logic_and_var_and(struct rbt_logic * l_var, struct rbt_logic * r_and, struct rbt_logic ** object) {
    return  rbt_logic_and_copy(object, r_and) ||
            rbt_logic_and_and_and_re(l_var, object);
}

static int rbt_logic_and_var_or(struct rbt_logic * l_var, struct rbt_logic * r_or, struct rbt_logic ** object) {
    rbt_logic * l = NULL;
    rbt_logic * r = NULL;

    switch(r_or->l->type) {
        case or: rbt_logic_and_var_or(l_var, r_or->l, &l); break;
        default: rbt_logic_op(l_var, r_or->l, &l, and); break;
    }

    switch(r_or->r->type) {
        case or: rbt_logic_and_var_or(l_var, r_or->r, &r); break;
        default: rbt_logic_op(l_var, r_or->r, &r, and); break;
    }

    if(rbt_logic_init(object, l, r, or)) {
        free_ptr_call(l, rbt_logic_deit);
        free_ptr_call(r, rbt_logic_deit);
        return 1;
    }

    return 0;
}

static int rbt_logic_and_and_or(struct rbt_logic * l_and, struct rbt_logic * r_or, struct rbt_logic ** object) {
    return rbt_logic_and_var_or(l_and, r_or, object);
}

/* =.=; */
static int rbt_logic_and_or_or(struct rbt_logic * l_or, struct rbt_logic * r_or, struct rbt_logic ** object) {
    int status = 0;
    rbt_logic * cross_1 = NULL;
    rbt_logic * cross_2 = NULL;
    rbt_logic * cross_3 = NULL;
    rbt_logic * cross_4 = NULL;
    rbt_logic * logic_1 = NULL;
    rbt_logic * logic_2 = NULL;

    if( rbt_logic_op(l_or->l, r_or->l, &cross_1, and) ||
        rbt_logic_op(l_or->l, r_or->r, &cross_2, and) ||
        rbt_logic_op(l_or->r, r_or->l, &cross_3, and) ||
        rbt_logic_op(l_or->r, r_or->r, &cross_4, and) ||
        rbt_logic_op(cross_1, cross_2, &logic_1, or) ||
        rbt_logic_op(logic_1, cross_3, &logic_2, or) ||
        rbt_logic_op(logic_1, cross_4, object, or) )
        status = 1;

    free_ptr_call(logic_2, rbt_logic_deit);
    free_ptr_call(logic_1, rbt_logic_deit);
    free_ptr_call(cross_4, rbt_logic_deit);
    free_ptr_call(cross_3, rbt_logic_deit);
    free_ptr_call(cross_2, rbt_logic_deit);
    free_ptr_call(cross_1, rbt_logic_deit);
    return status;
}

static int rbt_logic_or_var_and(struct rbt_logic * l_var, struct rbt_logic * r_and, struct rbt_logic ** object) {
    rbt_logic * l = NULL;
    rbt_logic * r = NULL;

    if( rbt_logic_var_copy(&l, l_var) ||
        rbt_logic_and_copy(&r, r_and) ||
        rbt_logic_init(object, l, r, or) ) {
        free_ptr_call(l, rbt_logic_deit);
        free_ptr_call(r, rbt_logic_deit);
        return 1;
    }

    return 0;
}

static int rbt_logic_or_and_and(struct rbt_logic * l_and, struct rbt_logic * r_and, struct rbt_logic ** object) {
    rbt_logic * l = NULL;
    rbt_logic * r = NULL;

    if( rbt_logic_and_copy(&l, l_and) ||
        rbt_logic_and_copy(&r, r_and) ||
        rbt_logic_init(object, l, r, or) ) {
        free_ptr_call(l, rbt_logic_deit);
        free_ptr_call(r, rbt_logic_deit);
        return 1;
    }

    return 0;
}

static int rbt_logic_or_var_or(struct rbt_logic * l_var, struct rbt_logic * r_or, struct rbt_logic ** object) {
    rbt_logic * l = NULL;
    rbt_logic * r = NULL;

    if( rbt_logic_var_copy(&l, l_var) ||
        rbt_logic_or_copy(&r, r_or) ||
        rbt_logic_init(object, l, r, or) ) {
        free_ptr_call(l, rbt_logic_deit);
        free_ptr_call(r, rbt_logic_deit);
        return 1;
    }

    return 0;
}

static int rbt_logic_or_and_or(struct rbt_logic * l_and, struct rbt_logic * r_or, struct rbt_logic ** object) {
    rbt_logic * l = NULL;
    rbt_logic * r = NULL;

    if( rbt_logic_and_copy(&l, l_and) ||
        rbt_logic_or_copy(&r, r_or) ||
        rbt_logic_init(object, l, r, or) ) {
        free_ptr_call(l, rbt_logic_deit);
        free_ptr_call(r, rbt_logic_deit);
        return 1;
    }

    return 0;
}

static int rbt_logic_or_or_or(struct rbt_logic * l_or, struct rbt_logic * r_or, struct rbt_logic ** object) {
    rbt_logic * l = NULL;
    rbt_logic * r = NULL;
    rbt_logic * p = NULL;

    if( rbt_logic_or_copy(&l, l_or) ||
        rbt_logic_or_copy(&r, r_or) )
        goto failed;

    if(or != l->l->type) {
        if(rbt_logic_init(&p, l->l, r, or))
            goto failed;

        l->l = p;
    } else if(or != l->r->type) {
        if(rbt_logic_init(&p, l->r, r, or))
            goto failed;
        l->r = p;
    } else {
        goto failed;
    }

    p->p = l;
    *object = l;
    return 0;

failed:
    free_ptr_call(l, rbt_logic_deit);
    free_ptr_call(r, rbt_logic_deit);
    return 1;
}

static int rbt_logic_base(struct rbt_logic * l, struct rbt_logic * r, struct rbt_logic ** object, int type) {
    rbt_range * range = NULL;
    rbt_logic * logic_1 = NULL;
    rbt_logic * logic_2 = NULL;
    if( type != var &&
        0 == strcmp(l->name, r->name)) {
        switch(type) {
            case or:    rbt_range_or (l->range, r->range, &range); break;
            case and:   rbt_range_and(l->range, r->range, &range); break;
        }
        if(rbt_logic_var_init(&logic_1, l->name, range))
            goto failed;

        rbt_range_deit(&range);
        *object = logic_1;
    } else {
        if( rbt_logic_var_copy(&logic_1, l) ||
            rbt_logic_var_copy(&logic_2, r) ||
            rbt_logic_init(object, logic_1, logic_2, type))
            goto failed;
    }

    return 0;

failed:
    rbt_range_deit(&range);
    rbt_logic_deit(&logic_2);
    rbt_logic_deit(&logic_1);
    return 1;
}

static int rbt_logic_and(struct rbt_logic * l, struct rbt_logic * r, struct rbt_logic ** object) {
    switch(l->type) {
        case var:
            switch(r->type) {
                case var: return rbt_logic_base(l, r, object, and);
                case and: return rbt_logic_and_var_and(l, r, object);
                case or:  return rbt_logic_and_var_or(l, r, object);
            }
            break;
        case and:
            switch(r->type) {
                case var: return rbt_logic_and_var_and(r, l, object);
                case and: return rbt_logic_and_and_and(l, r, object);
                case or:  return rbt_logic_and_and_or(l, r, object);
            }
            break;
        case or:
            switch(r->type) {
                case var: return rbt_logic_and_var_or(r, l, object);
                case and: return rbt_logic_and_and_or(r, l, object);
                case or:  return rbt_logic_and_or_or(r, l, object);
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
                case and: return rbt_logic_or_var_and(l, r, object);
                case or:  return rbt_logic_or_var_or(l, r, object);
            }
            break;
        case and:
            switch(r->type) {
                case var: return rbt_logic_or_var_and(r, l, object);
                case and: return rbt_logic_or_and_and(l, r, object);
                case or:  return rbt_logic_or_and_or(l, r, object);
            }
            break;
        case or:
            switch(r->type) {
                case var: return rbt_logic_or_var_or(r, l, object);
                case and: return rbt_logic_or_and_or(r, l, object);
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
