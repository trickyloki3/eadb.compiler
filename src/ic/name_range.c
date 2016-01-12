/*
 *    file: name_range.c
 *    date: 12/9/2014
 *    auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#include "name_range.h"

/* generate a new condition node; the leaf node of the logical tree structure */
logic_node_t * make_cond(int var, char * name, range_t * range, logic_node_t * func) {
    int name_len = 0;
    logic_node_t * new_node = NULL;
    /* check null name and range */
    if(NULL == name || NULL == range)
        return NULL;

    /* make the condition node */
    name_len = strlen(name);
    new_node = calloc(1, sizeof(logic_node_t));
    new_node->var = var;
    new_node->type = LOGIC_NODE_COND;
    new_node->name = calloc(name_len + 1, sizeof(char));
    strncpy(new_node->name, name, name_len);
    new_node->range = copyrange(range);
    return new_node;
}

/* dump the condition node struct */
void dmpcond(logic_node_t * cond, FILE * stm) {
    if(stm != NULL)
        fprintf(stm,"dmpcond; %p; var %d; (%s/%d: %d ~ %d)\n",
        (void *) cond, cond->var, cond->name, cond->type,
        cond->range->min, cond->range->max);
}

/* deallocating the condition node */
void free_cond(logic_node_t * cond) {
    if(cond != NULL) {
        if(cond->name != NULL) free(cond->name);
        if(cond->range != NULL) freerange(cond->range);
        free(cond);
    }
}

/* generic logical operator covering all 18 bases; 2 base cases and 16 recursive cases */
logic_node_t * new_cond(int logic_node_type, logic_node_t * left, logic_node_t * right) {
    logic_node_t * root = NULL;

    /* validate paramaters since all handlers go through this function */
    if(INVALID_NODE_TYPE(logic_node_type)) {
        exit_func_safe("invalid node type %d", logic_node_type);
        return NULL;
    }

    if(NULL == left || NULL == right)
        return NULL;

    if(COND_COND_CASE(left, right)) {
        root = base_cond_cond(logic_node_type, left, right);
    } else if(logic_node_type == LOGIC_NODE_AND) {
        if(COND_AND_CASE(left, right))        root = AND_cond_and(left, right);
        else if(AND_COND_CASE(left, right)) root = AND_cond_and(right, left);
        else if(COND_OR_CASE(left,right))    root = AND_cond_or(left, right);
        else if(OR_COND_CASE(left,right))    root = AND_cond_or(right, left);
        else if(AND_OR_CASE(left,right))     root = AND_or_and(right, left);
        else if(OR_AND_CASE(left,right))     root = AND_or_and(left, right);
        else if(AND_AND_CASE(left,right))    root = AND_and_and(left, right);
        else if(OR_OR_CASE(left,right))      root = AND_or_or(left, right);
        else goto new_cond_err;
    } else if(logic_node_type == LOGIC_NODE_OR) {
        if(COND_AND_CASE(left, right))        root = OR_cond_and(left, right);
        else if(AND_COND_CASE(left, right)) root = OR_cond_and(right, left);
        else if(COND_OR_CASE(left,right))    root = OR_cond_or(left, right);
        else if(OR_COND_CASE(left,right))    root = OR_cond_or(right, left);
        else if(AND_OR_CASE(left,right))     root = OR_and_or(left, right);
        else if(OR_AND_CASE(left,right))     root = OR_and_or(right, left);
        else if(AND_AND_CASE(left,right))    root = OR_and_and(left, right);
        else if(OR_OR_CASE(left,right))      root = OR_or_or(left, right);
        else goto new_cond_err;
    }
    return root;

    new_cond_err:
        exit_func_safe("invalid node type in either left %d or right node %d", left->type, right->type);
        return NULL;
}

/* handle the cond-cond base case */
logic_node_t * base_cond_cond(int logic_node_type, logic_node_t * left, logic_node_t * right) {
    range_t * temp = NULL;
    logic_node_t * new_node = NULL;
    /* attempt to merge condition nodes on AND */
    if(logic_node_type == LOGIC_NODE_AND && ncs_strcmp(left->name, right->name) == 0 && left->var == right->var) {
        temp = andrange(left->range, right->range);
        new_node = make_cond(left->var, left->name, temp, NULL);
        freerange(temp);
    } else {
        new_node = calloc(1, sizeof(logic_node_t));
        new_node->type = logic_node_type;
        new_node->left = make_cond(left->var, left->name, left->range, left);
        new_node->right = make_cond(right->var, right->name, right->range, right);
        new_node->left->parent = new_node;
        new_node->right->parent = new_node;
    }
    return new_node;
}

/* handle the and-cond and cond-and for and case */
logic_node_t * AND_cond_and(logic_node_t * cond, logic_node_t * and) {
    range_t * temp = NULL;
    logic_node_t * result = NULL;
    logic_node_t * new_node = NULL;
    /* search for matching cond identifier in the AND subtree; merge if founded */
    search_identifier(and, cond->var, cond->name, &result);
    if(result != NULL) {
        temp = result->range;
        result->range = andrange(cond->range, result->range);
        freerange(temp);
        return copy_and_tree(and);
    /* add the condition if no merging is possible */
    } else {
        new_node = calloc(1, sizeof(logic_node_t));
        new_node->type = LOGIC_NODE_AND;
        new_node->left = make_cond(cond->var, cond->name, cond->range, cond);
        new_node->right = copy_and_tree(and);
        new_node->left->parent = new_node;
        new_node->right->parent = new_node;
        return new_node;
    }
}

/* handle the or-cond and cond-or for and case */
logic_node_t * AND_cond_or(logic_node_t * cond, logic_node_t * or) {
    logic_node_t * root = calloc(1, sizeof(logic_node_t));
    root->type = LOGIC_NODE_OR;
    if(or->left->type == LOGIC_NODE_OR)
        root->left = AND_cond_or(cond, or->left);
    else
        root->left = new_cond(LOGIC_NODE_AND, cond, or->left);
    root->left->parent = root;

    if(or->right->type == LOGIC_NODE_OR)
        root->right = AND_cond_or(cond, or->right);
    else
        root->right = new_cond(LOGIC_NODE_AND, cond, or->right);
    root->right->parent = root;
    return root;
}

/* handle the and-and for and case */
logic_node_t * AND_and_and(logic_node_t * and_1, logic_node_t * and_2) {
    logic_node_t * new_root = copy_and_tree(and_2);
    AND_and_and_iter(and_1, &new_root);
    return new_root;
}

logic_node_t * AND_and_and_iter(logic_node_t * and_1, logic_node_t ** and_2) {
    logic_node_t * temp = NULL;
    if(and_1->type == LOGIC_NODE_COND) {
        temp = *and_2;
        *and_2 = AND_cond_and(and_1, *and_2);
        freenamerange(temp);
    }
    if(and_1->left != NULL) AND_and_and_iter(and_1->left, and_2);
    if(and_1->right != NULL) AND_and_and_iter(and_1->right, and_2);
    return *and_2;
}

/* handle the or-or for and case; expensive */
logic_node_t * AND_or_or(logic_node_t * or_1, logic_node_t * or_2) {
    logic_node_t * cross_1 = new_cond(LOGIC_NODE_AND, or_1->left, or_2->left);
    logic_node_t * cross_2 = new_cond(LOGIC_NODE_AND, or_1->left, or_2->right);
    logic_node_t * cross_3 = new_cond(LOGIC_NODE_AND, or_1->right, or_2->left);
    logic_node_t * cross_4 = new_cond(LOGIC_NODE_AND, or_1->right, or_2->right);
    logic_node_t * new_root = new_cond(LOGIC_NODE_OR, cross_1, cross_2);
    logic_node_t * new_root2 = new_cond(LOGIC_NODE_OR, new_root, cross_3);
    logic_node_t * new_root3 = new_cond(LOGIC_NODE_OR, new_root2, cross_4);
    freenamerange(cross_1);
    freenamerange(cross_2);
    freenamerange(cross_3);
    freenamerange(cross_4);
    freenamerange(new_root);
    freenamerange(new_root2);
    return new_root3;
}

/* handle the and-or for and case */
logic_node_t * AND_or_and(logic_node_t * or, logic_node_t * and) {
    logic_node_t * new_node = calloc(1, sizeof(logic_node_t));
    new_node->type = LOGIC_NODE_OR;
    new_node->left = new_cond(LOGIC_NODE_AND, or->left, and);
    new_node->left->parent = or;
    new_node->right = new_cond(LOGIC_NODE_AND, or->right, and);
    new_node->right->parent = or;
    return new_node;
}

/* handle the and-cond and cond-and for or case; nothing; make or node */
logic_node_t * OR_cond_and(logic_node_t * cond, logic_node_t * and) {
    logic_node_t * new_node = calloc(1, sizeof(logic_node_t));
    new_node->type = LOGIC_NODE_OR;
    new_node->left = make_cond(cond->var, cond->name, cond->range, cond);
    new_node->right = copy_and_tree(and);
    new_node->left->parent = new_node;
    new_node->right->parent = new_node;
    return new_node;
}

/* handle the or-cond and cond-or for or case; nothing; make or node */
logic_node_t * OR_cond_or(logic_node_t * cond, logic_node_t * or) {
    logic_node_t * new_node = calloc(1, sizeof(logic_node_t));
    new_node->type = LOGIC_NODE_OR;
    new_node->left = make_cond(cond->var, cond->name, cond->range, cond);
    new_node->right = copy_or_tree(or);
    new_node->left->parent = new_node;
    new_node->right->parent = new_node;
    return new_node;
}

/* handle the and-and for or case; nothing; make or node */
logic_node_t * OR_and_and(logic_node_t * and_1, logic_node_t * and_2) {
    logic_node_t * new_node = calloc(1, sizeof(logic_node_t));
    new_node->type = LOGIC_NODE_OR;
    new_node->left = copy_and_tree(and_1);
    new_node->right = copy_and_tree(and_2);
    new_node->left->parent = new_node;
    new_node->right->parent = new_node;
    return new_node;
}

/* handle the and-or for or case; nothing; make or node */
logic_node_t * OR_and_or(logic_node_t * and, logic_node_t * or) {
    logic_node_t * new_node = calloc(1, sizeof(logic_node_t));
    new_node->type = LOGIC_NODE_OR;
    new_node->left = copy_and_tree(and);
    new_node->right = copy_or_tree(or);
    new_node->left->parent = new_node;
    new_node->right->parent = new_node;
    return new_node;
}

/* handle the or-or for or case */
logic_node_t * OR_or_or(logic_node_t * or_1, logic_node_t * or_2) {
    logic_node_t * new_node = calloc(1, sizeof(logic_node_t));
    or_1 = copy_or_tree(or_1);
    or_2 = copy_or_tree(or_2);
    new_node->type = LOGIC_NODE_OR;
    /* inherit the right node from or_1 */
    if(or_1->right->type == LOGIC_NODE_COND ||
        or_1->right->type == LOGIC_NODE_AND) {
        new_node->left = or_1->right;
        new_node->right = or_2;
        new_node->left->parent = new_node;
        new_node->right->parent = new_node;
        or_1->right = new_node;
        or_1->right->parent = or_1;
    } else if(or_1->left->type == LOGIC_NODE_COND ||
              or_1->left->type == LOGIC_NODE_AND) {
        new_node->left = or_1->left;
        new_node->right = or_2;
        new_node->left->parent = new_node;
        new_node->right->parent = new_node;
        or_1->left = new_node;
        or_1->left->parent = or_1;
    } else {
        fprintf(stderr,"fatal: invalid sub logic tree in either left or right.\n");
        exit(EXIT_FAILURE);
    }
    return or_1;
}

/* search subtree for matching id; pre-order traversal */
void search_identifier(logic_node_t * node, int var, char * name, logic_node_t ** result) {
    if(node->type == LOGIC_NODE_COND)
        if(ncs_strcmp(node->name, name) == 0 && node->var == var)
            *result = node;
    /* continue searching if result not found */
    if(node->left != NULL && *result == NULL) search_identifier(node->left, var, name, result);
    if(node->right != NULL && *result == NULL) search_identifier(node->right, var, name, result);
}

/* display the logical tree */
void dmpnamerange(logic_node_t * root, FILE * stm, int level) {
    int i = 0;
    if(root == NULL) return;
    for(i = 0; i < level; i++) fprintf(stm,"\t");
    switch(root->type) {
        case LOGIC_NODE_COND:    fprintf(stm,"%p<-%p:COND:%s;VAR:%d;", (void *) root->parent, (void *) root, root->name, root->var);
                                        dmprange(root->range, stm, "cond ");
                                        break;
        case LOGIC_NODE_AND:     fprintf(stm,"%p<-%p:AND\n", (void *) root->parent, (void *) root); break;
        case LOGIC_NODE_OR:      fprintf(stm,"%p<-%p:OR\n", (void *) root->parent, (void *) root); break;
        default: break;
    }
    if(root->left != NULL) dmpnamerange(root->left, stm, level + 1);
    if(root->right != NULL) dmpnamerange(root->right, stm, level + 1);
}

void deepdmpnamerange(logic_node_t * root, FILE * stm, char * tag) {
    logic_node_t * iter = root;
    while(iter != NULL) {
        fprintf(stm,"%s\n", tag);
        dmpnamerange(iter, stm, 0);
        iter = iter->stack;
    }
}

/* free the logical tree; prefix order, dismantle from bottom up */
void freenamerange(logic_node_t * root) {
    if(root == NULL) return;
    if(root->left != NULL) freenamerange(root->left);
    if(root->right != NULL) freenamerange(root->right);
    /* cond and logical nodes are free separately */
    if(root->type == LOGIC_NODE_COND)
        free_cond(root);
    else
        free(root);
}

void deepfreenamerange(logic_node_t * root) {
    logic_node_t * temp = NULL;
    logic_node_t * iter = root;
    while(iter != NULL) {
        temp = iter;
        iter = iter->stack;
        freenamerange(temp);
    }
}

/* copy the AND tree */
logic_node_t * copy_and_tree(logic_node_t * root) {
    logic_node_t * new_root = NULL;
    if(root->type == LOGIC_NODE_AND) {
        new_root = calloc(1, sizeof(logic_node_t));
        new_root->type = LOGIC_NODE_AND;
        if(root->left->type == LOGIC_NODE_COND)
            new_root->left = make_cond(root->left->var, root->left->name, root->left->range, root->left);
        else if(root->left->type == LOGIC_NODE_AND)
            new_root->left = copy_and_tree(root->left);
        new_root->left->parent = new_root;

        if(root->right->type == LOGIC_NODE_COND)
            new_root->right = make_cond(root->right->var, root->right->name, root->right->range, root->right);
        else if(root->right->type == LOGIC_NODE_AND)
            new_root->right = copy_and_tree(root->right);
        new_root->right->parent = new_root;
    }
    return new_root;
}

/* copy the OR tree */
logic_node_t * copy_or_tree(logic_node_t * root) {
    logic_node_t * new_root = NULL;
    if(root->type == LOGIC_NODE_OR) {
        new_root = calloc(1, sizeof(logic_node_t));
        new_root->type = LOGIC_NODE_OR;
        /* copy the left node */
        if(root->left->type == LOGIC_NODE_COND)
            new_root->left = make_cond(root->left->var, root->left->name, root->left->range, root->left);
        else if(root->left->type == LOGIC_NODE_AND)
            new_root->left = copy_and_tree(root->left);
        else if(root->left->type == LOGIC_NODE_OR)
            new_root->left = copy_or_tree(root->left);
        new_root->left->parent = new_root;

        /* copy the right node */
        if(root->right->type == LOGIC_NODE_COND)
            new_root->right = make_cond(root->right->var, root->right->name, root->right->range, root->right);
        else if(root->right->type == LOGIC_NODE_AND)
            new_root->right = copy_and_tree(root->right);
        else if(root->right->type == LOGIC_NODE_OR)
            new_root->right = copy_or_tree(root->right);
        new_root->right->parent = new_root;
    }
    return new_root;
}

logic_node_t * copy_any_tree(logic_node_t * root) {
    if(root->type == LOGIC_NODE_COND)
        return make_cond(root->var, root->name, root->range, root);
    else if(root->type == LOGIC_NODE_AND)
        return copy_and_tree(root);
    else if(root->type == LOGIC_NODE_OR)
        return copy_or_tree(root);
    else {
        fprintf(stderr,"copy_any_tree; invalid node type; cannot copy.\n");
        exit(EXIT_FAILURE);
    }
}

logic_node_t * copy_deep_any_tree(logic_node_t * root) {
    logic_node_t * ret_root = NULL;
    logic_node_t * new_root = NULL;
    logic_node_t * temp = NULL;
    logic_node_t * iter = root;
    while(iter != NULL) {
        temp = copy_any_tree(iter);
        iter = iter->stack;
        if(new_root == NULL)
            ret_root = new_root = temp;
        else
            new_root = new_root->stack = temp;
    }
    return ret_root;
}

range_t * search_tree_dependency(logic_node_t * root, char * name, range_t * range) {
    range_t * result = NULL;
    logic_node_t * iter = root;
    /* search the stack until dependency found
     * stack of logic tree is due to the ? operator*/
    while(iter != NULL) {
        search_tree_name_range(iter, name, range, &result);
        iter = iter->stack;
    }
    return result;
}

void search_tree_name_range(logic_node_t * root, char * name, range_t * range, range_t ** match) {
    range_t * _match = NULL;
    if(root->left != NULL) search_tree_name_range(root->left, name, range, match);
    if(root->right != NULL) search_tree_name_range(root->right, name, range, match);

    /* AND every matching name */
    if(root->type == LOGIC_NODE_COND)
        if(ncs_strcmp(root->name, name) == 0 &&
           (root->range->min != 0 && /* ignore zeros */
            root->range->max != 0)) {

            _match = *match;

            if(NULL == _match) {
                *match = andrange(root->range, range);
            } else {
                *match = andrange(root->range, _match);
                freerange(_match);
            }
        }
}

range_t * search_tree_dependency_or(logic_node_t * root, char * name, range_t * range) {
    range_t * result = NULL;
    logic_node_t * iter = root;
    /* search the stack until dependency found
     * stack of logic tree is due to the ? operator*/
    while(iter != NULL) {
        search_tree_name_range_or(iter, name, range, &result);
        iter = iter->stack;
    }
    return result;
}

void search_tree_name_range_or(logic_node_t * root, char * name, range_t * range, range_t ** match) {
    range_t * temp = NULL;
    if(root->left != NULL) search_tree_name_range_or(root->left, name, range, match);
    if(root->right != NULL) search_tree_name_range_or(root->right, name, range, match);

    /* OR every matching name */
    if(root->type == LOGIC_NODE_COND) {
        if(ncs_strcmp(root->name, name) == 0) {
            /* free the previous result and build the new result */
            if(*match != NULL) temp = *match;
            *match = (*match == NULL)?
                orrange(root->range, range):
                orrange(root->range, *match);
            if(temp != NULL) freerange(temp);
        }
    }
}

logic_node_t * inverse_logic_tree(logic_node_t * root) {
    logic_node_t * new_root = NULL;
    int type = 0;
    logic_node_t * left = NULL;
    logic_node_t * right = NULL;
    range_t * temp_range = NULL;

    /* inverse all the things */
    if(root->type == LOGIC_NODE_AND || root->type == LOGIC_NODE_OR) {
        /* inverse AND -> OR and OR -> AND */
        type = (root->type == LOGIC_NODE_OR)?LOGIC_NODE_AND:LOGIC_NODE_OR;

        if(root->left->type == LOGIC_NODE_AND || root->left->type == LOGIC_NODE_OR)
            left = inverse_logic_tree(root->left);
        else if(root->left->type == LOGIC_NODE_COND) {
            temp_range = notrange(root->left->range);
            left = make_cond(root->left->var, root->left->name, temp_range, root->left);
            freerange(temp_range); /* make_cond makes a copy of supplied range */
        }

        if(root->right->type == LOGIC_NODE_AND || root->right->type == LOGIC_NODE_OR)
            right = inverse_logic_tree(root->right);
        else if(root->right->type == LOGIC_NODE_COND) {
            temp_range = notrange(root->right->range);
            right = make_cond(root->right->var, root->right->name, temp_range, root->right);
            freerange(temp_range); /* make_cond makes a copy of supplied range */
        }

        new_root = new_cond(type, left, right);
        freenamerange(left);
        freenamerange(right);
    } else {
        temp_range = notrange(root->range);
        new_root = make_cond(root->var, root->name, temp_range, root);
        freerange(temp_range); /* make_cond makes a copy of supplied range */
    }

    if(new_root == NULL) {
        fprintf(stderr,"fatal: return null in inverse_logic_tree\n");
        exit(EXIT_FAILURE);
    }
    return new_root;
}

logic_node_t * deep_inverse_logic_tree(logic_node_t * root) {
    logic_node_t * ret_root = NULL;
    logic_node_t * new_root = NULL;
    logic_node_t * temp = NULL;
    logic_node_t * iter = root;
    while(iter != NULL) {
        temp = inverse_logic_tree(iter);
        iter = iter->stack;
        if(new_root == NULL)
            ret_root = new_root = temp;
        else {
            new_root->stack = temp;
            new_root = new_root->stack;
        }
    }
    return ret_root;
}
