#pragma once

#include "../diff/diff.h"

//=========================================================

int _tree_simplify(struct Tree* diff, LOG_PARAMS);

//=========================================================

#define tree_simplify(diff) \
       _tree_simplify(diff, LOG_ARGS)

#define constant_folding(node) \
       _constant_folding(node, LOG_ARGS)

#define cut_nodes(diff, node) \
       _cut_nodes(diff, node, LOG_ARGS)

#define calc_operand_value(node) \
       _calc_operand_value(node, LOG_ARGS)

#define calc_function_value(node) \
       _calc_function_value(node, LOG_ARGS)

#define is_zero_node(node) \
       _is_zero_node(node, LOG_ARGS)

#define is_one_node(node) \
       _is_one_node(node, LOG_ARGS)

#define replace_with_const(node, constant) \
       _replace_with_const(node, constant, LOG_ARGS)

#define cut_constant(diff, node, cutted) \
       _cut_constant(diff, node, cutted, LOG_ARGS)  

#define mul_by_one_simp_check(node) \
       _mul_by_one_simp_check(node, LOG_ARGS)

#define mul_by_zero_simp_check(node) \
       _mul_by_zero_simp_check(node, LOG_ARGS)
       
#define sum_with_zero_simp_check(node) \
       _sum_with_zero_simp_check(node, LOG_ARGS)

#define pow_zero_simp_check(node) \
       _pow_zero_simp_check(node, LOG_ARGS)

#define pow_one_simp_check(node) \
       _pow_one_simp_check(node, LOG_ARGS)
