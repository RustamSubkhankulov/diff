#include <math.h>

#include "tree_simp.h"
#include "../general/general.h"

//===================================================================

static int _constant_folding(struct Node* node, LOG_PARAMS);

static int _cut_nodes(struct Tree* diff, struct Node** node, LOG_PARAMS);

static int _calc_operand_value(struct Node* node, LOG_PARAMS);

static int _calc_function_value(struct Node* node, LOG_PARAMS);

static Node* _is_zero_node(struct Node* node, LOG_PARAMS);

static Node* _is_one_node(struct Node* node, LOG_PARAMS);

static int _replace_with_const(struct Node* node, double constant, LOG_PARAMS);

static int _cut_constant(struct Tree* diff, struct Node** node, struct Node* cutted, 
                                                                        LOG_PARAMS);

//===================================================================

#define L left_son

#define R right_son

//===================================================================

int _tree_simplify(struct Tree* diff, LOG_PARAMS) {

    diff_log_report();              
    TREE_PTR_CHECK(diff);

    int constant_simp = constant_folding(diff->root);
    if (constant_simp == -1)
        return -1;

    int cut_nodes = cut_nodes(diff, &(diff->root));
    if (cut_nodes == -1)
        return -1;

    tree_draw_graph(diff);

    return (constant_simp || cut_nodes );
}

//===================================================================

static int _calc_operand_value(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    double first_value  = node->L->data.constant;
    double second_value = node->R->data.constant;

    double result = 0;

    switch (node->data.operand) {

        case ADD: {

            result = first_value + second_value;
            break;
        }
        
        case SUB: {

            result = first_value - second_value;
            break;
        }

        case MUL: {

            result = first_value * second_value;
            break;
        }

        case DIV: {

            if (double_is_equal(0, second_value)) {

                error_report(DIV_BY_ZERO);
                return -1;
            }

            result = first_value / second_value;
            break;
        }

        case POW: {

            result = pow(first_value, second_value);
            break;
        }

        default: {

            error_report(DIFF_INV_OPERAND);
            return -1;
        }
    }

    NODE_INIT_CONSTANT(node, result);

    DESTRUCT_BRANCH(node->L);
    DESTRUCT_BRANCH(node->R);

    node->L = NULL;
    node->R = NULL;

    return 1;
}

//===================================================================

static int _calc_function_value(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    double arg = node->L->data.constant;
    double result = 0;

    switch (node->data.operand) {

        case SIN: {

            result = sin(arg);
            break;
        }

        case COS: {

            result = cos(arg);
            break;
        }

        case TG: {

            result = tan(arg);
            break;
        }

        case CTG: {

            result = 1 / tan(arg);
            break;
        }

        case ACOS: {

            result = acos(arg);
            break;
        }

        case ASIN: {

            result = asin(arg);
            break;
        }

        case ATG: {

            result = atan(arg); 
            break;
        }

        case ACTG: {

            result = 1 / atan(arg);
            break;
        }

        default: {

            error_report(DIFF_INV_OPERAND);
            return -1;
        }
    }

    NODE_INIT_CONSTANT(node, result);

    DESTRUCT_BRANCH(node->L);
    node->L = NULL;

    return 1;
}

//===================================================================

static int _constant_folding(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    int ret = 0;
    int is_simplified = 0;

    if (node->L) {

        ret = constant_folding(node->L);
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (node->R) {

        ret = constant_folding(node->R);
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (IS_OPER(node) 
     && !is_function_operand(node)
     && IS_CONST(node->L) 
     && IS_CONST(node->R)) {

        ret = calc_operand_value(node);
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (IS_OPER(node)
    && is_function_operand(node)
    && node->L != NULL
    && IS_CONST(node->L)) {

        ret = calc_function_value(node);
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    return is_simplified;
}

//===================================================================

static Node* _is_zero_node(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    if (!node)
        return NULL;

    if (node->L
     && IS_CONST(node->L)
     && double_is_equal(node->L->data.constant, 0))
        return node->L;
    
    if (node->R
     && IS_CONST(node->R)
     && double_is_equal(node->R->data.constant, 0))
        return node->R;

    return NULL;
}

//===================================================================

static Node* _is_one_node(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    if (!node)
        return NULL;

    if (node->L
     && IS_CONST(node->L)
     && double_is_equal(node->L->data.constant, 1))
        return node->L;
    
    if (node->R
     && IS_CONST(node->R) 
     && double_is_equal(node->R->data.constant, 1))
        return node->R;

    return NULL;
}

//===================================================================

static int _replace_with_const(struct Node* node, double constant, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    DESTRUCT_BRANCH(node->L);
    DESTRUCT_BRANCH(node->R);

    node->L = NULL;
    node->R = NULL;

    NODE_INIT_CONSTANT(node, constant);

    return 1;
}

//===================================================================

static int _cut_constant(struct Tree* diff, struct Node** node_ptr, struct Node* cutted, 
                                                                             LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node_ptr);
    TREE_PTR_CHECK(diff);

    struct Node* node = *node_ptr;
    struct Node* expression = NULL;

    if (node->L != cutted)
        expression = node->L;
    else 
        expression = node->R;

    *node_ptr = expression;
    (*node_ptr)->parent = node->parent;

    if (node->parent == No_parent)
        diff->root = expression;

    else if (node == node->parent->left_son)
        node->parent->left_son  = expression;
        
    else if (node == node->parent->right_son)
        node->parent->right_son = expression;

    DESTRUCT_BRANCH(cutted);
    int ret = node_destruct(node);
    if (ret == -1)
        return -1;

    tree_draw_graph(diff);

    return 1;
}

//===================================================================

static int _mul_by_zero_simp_check(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node)

    if (node->data_type    == OPERAND 
    &&  node->data.operand == MUL 
    &&  is_zero_node(node))

        return 1;
    else 
        return 0;
}

//-------------------------------------------------------------------

static Node* _mul_by_one_simp_check(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    if (!node)
        return NULL;

    struct Node* returning = NULL;

    if  (IS_OPER(node) 
    &&   node->data.operand == MUL 
    &&  (returning = is_one_node(node)))

        return returning;
    else 
        return NULL;
}

//-------------------------------------------------------------------

static Node* _sum_with_zero_simp_check(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    if (!node)
        return NULL;

    struct Node* returning = NULL;

    if (IS_OPER(node)
    && (node->data.operand == ADD
    ||  node->data.operand == SUB)
    && (returning = is_zero_node(node)))

        return returning;
    else
        return NULL;
}

//-------------------------------------------------------------------

static Node* _pow_one_simp_check(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    if (!node)
        return NULL;

    struct Node* returning = NULL;

    if (IS_OPER(node)
    &&  node->data.operand == POW
    && (returning = is_one_node(node)))

        return returning;
    else
        return NULL;
}

//-------------------------------------------------------------------

static int _pow_zero_simp_check(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    if (IS_OPER(node)
    &&  node->data.operand == POW
    &&  is_zero_node(node))

        return 1;
    else
        return 0;
}

//===================================================================

static int _cut_nodes(struct Tree* diff, struct Node** node_ptr, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node_ptr);
    TREE_PTR_CHECK(diff);

    struct Node* node = *node_ptr;

    int is_simplified = 0;
    int ret = 0;
    struct Node* cutted = NULL;

    if (node->L) {

        ret = cut_nodes(diff, &(node->L));
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (node->R) {

        ret = cut_nodes(diff, &(node->R));
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (mul_by_zero_simp_check(node)) {

         ret = replace_with_const(node, 0);
         if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if ((cutted = mul_by_one_simp_check(node))) {

         ret = cut_constant(diff, &node, cutted);
         if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if ((cutted = sum_with_zero_simp_check(node))) {

         ret = cut_constant(diff, &node, cutted);
         if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (pow_zero_simp_check(node)) {

         ret = replace_with_const(node, 1);
         if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;

    }

    if ((cutted = pow_one_simp_check(node))) {

        ret = cut_constant(diff, &node, cutted);
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    return is_simplified;
}

//===================================================================

#undef R
#undef L