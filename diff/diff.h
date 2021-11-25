#pragma once

#include "diff_config.h"

#include "../tree/tree.h"
#include "../logs/errors_and_logs.h"

//===================================================================

#define NODE_INIT_CONSTANT(node, value) {                           \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_init_constant(node, value);                  \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define NODE_INIT_VARIABLE(node, value) {                           \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_init_variable(node, value);                  \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define NODE_INIT_OPERAND(node, value) {                            \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_init_operand(node, value);                   \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_LEFT(node) {                                            \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_add_left_son(node);                          \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_RIGHT(node) {                                           \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_add_right_son(node);                         \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_LEFT_AND_RIGHT(node) {                                  \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_add_sons(node);                              \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_OPERAND(node, son, code) {                              \
                                                                    \
    do                                                              \
    {                                                               \
        switch(son) {                                               \
                                                                    \
            case LEFT: {                                            \
                                                                    \
                int ret = node_add_left_son(node);                  \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                ret = node_init_operand(node->left_son, code);      \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                break;                                              \
            }                                                       \
                                                                    \
            case RIGHT: {                                           \
                                                                    \
                int ret = node_add_right_son(node);                 \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                ret = node_init_operand(node->right_son, code);     \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                break;                                              \
            }                                                       \
                                                                    \
            default: {                                              \
                                                                    \
                error_report(INV_ADD_PARAMETER);                    \
                return -1;                                          \
            }                                                       \
        }                                                           \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_CONSTANT(node, son, constant) {                         \
                                                                    \
    do                                                              \
    {                                                               \
        switch(son) {                                               \
                                                                    \
            case LEFT: {                                            \
                                                                    \
                int ret = node_add_left_son(node);                  \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                ret = node_init_constant(node->left_son, constant); \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                break;                                              \
            }                                                       \
                                                                    \
            case RIGHT: {                                           \
                                                                    \
                int ret = node_add_right_son(node);                 \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                ret = node_init_constant(node->right_son, constant);\
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                break;                                              \
            }                                                       \
                                                                    \
            default: {                                              \
                                                                    \
                error_report(INV_ADD_PARAMETER);                    \
                return -1;                                          \
            }                                                       \
        }                                                           \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define ADD_VARIABLE(node, son, var) {                              \
                                                                    \
    do                                                              \
    {                                                               \
        switch(son) {                                               \
                                                                    \
            case LEFT: {                                            \
                                                                    \
                int ret = node_add_left_son(node);                  \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                ret = node_init_variable(node->left_son, var);      \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                break;                                              \
            }                                                       \
                                                                    \
            case RIGHT: {                                           \
                                                                    \
                int ret = node_add_right_son(node);                 \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                ret = node_init_variable(node->right_son, var);     \
                if (ret == -1)                                      \
                    return -1;                                      \
                                                                    \
                break;                                              \
            }                                                       \
                                                                    \
            default: {                                              \
                                                                    \
                error_report(INV_ADD_PARAMETER);                    \
                return -1;                                          \
            }                                                       \
        }                                                           \
    } while(0);                                                     \
}

//-------------------------------------------------------------------

#define NODE_DIFF(src, dest) {                                      \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = node_diff_execute(src, dest);                     \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}   

#define NODE_COPY(src, dest) {                                      \
                                                                    \
    do                                                              \
    {                                                               \
        int ret = diff_copy_branch(src, dest);                      \
        if (ret == -1)                                              \
            return -1;                                              \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#ifdef DIFF_LOGS

    #define diff_log_report() \
            log_report()

#else

    #define diff_log_report() ""

#endif

//===================================================================

#ifdef DIFF_TREE_VERIFICATE

    #define DIFF_TREE_VERIFICATION(tree) {                          \
                                                                    \
        do                                                          \
        {                                                           \
            int is_ok = tree_validator(tree);                       \
            if (is_ok == -1)                                        \
                return -1;                                          \
                                                                    \
        } while(0);                                                 \
    }                                           

#else 

    #define DIFF_TREE_VERIFICATION(tree) ""

#endif

//===================================================================

#define BUFFER_STRUCT_PTR_CHECK(buffer_struct_ptr) {                \
                                                                    \
    if (buffer_struct_ptr == NULL) {                                \
                                                                    \
        error_report(BUFFER_STRUCT_INV_PTR);                        \
        return -1;                                                  \
    }                                                               \
}

//===================================================================

struct Function {

       int code;
       int64_t hash;
       const char* name;
};

static struct Function Functions[] {

       { 7563630   , 3141473398, "sin"  },
       { 6516595   , 2519927297, "cos"  },
       { 29799     , 1173106379, "tg"   },
       { 6517863   , 3865010701, "ctg"  },
       { 1633906547, 1322787422, "acos" },
       { 1634953582, 2736860584, "asin" },
       { 6386791   , 3546035762, "atg"  },
       { 1633907815, 815273638 , "actg" }
};

const int Functions_number = 8;

//-------------------------------------------------------------------

enum Operands {

       ADD    = '+'        ,
       SUB    = '-'        ,
       MUL    = '*'        ,
       DIV    = '/'        ,
       POW    = '^'        ,
       SIN    = 7563630    ,
       COS    = 6516595    ,
       TG     = 29799      ,
       CTG    = 6517863    ,
       ACOS   = 1633906547 ,
       ASIN   = 1634953582 ,
       ATG    = 6386791    ,
       ACTG   = 1633907815
};

//===================================================================

const int Operands_number = 13;

const int Operands[Operands_number] = {ADD , SUB , MUL, DIV, POW, 
                                       SIN , COS , TG , CTG, 
                                       ACOS, ASIN, ATG, ACTG};

//===================================================================

enum Node_add_parameters {

    LEFT   = 1488,
    RIGHT  = 1489
};

//===================================================================

int _diff_tree_ctor(struct Tree* tree, LOG_PARAMS);

int _diff_tree_dtor(struct Tree* tree, LOG_PARAMS);

int _diff_execute(struct Tree* tree, struct Tree* diff, LOG_PARAMS);

int _node_diff_execute(struct Node* node, struct Node* diff, LOG_PARAMS);

int _diff_out_to_file(struct Tree* diff, const char* filename, LOG_PARAMS);

int _diff_out_to_console(struct Tree* diff, LOG_PARAMS);

int _buffer_struct_init(struct Buffer_struct* buffer_struct, char* buffer, 
                                           int size, int pos, LOG_PARAMS);

int _node_read_from_buffer(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                      LOG_PARAMS);

int _buffer_dump(struct Buffer_struct* buffer_struct, LOG_PARAMS);

char* _diff_read_from_file(struct Tree* tree, const char* filename, LOG_PARAMS);

char* _diff_read_from_console(struct Tree* tree, LOG_PARAMS);

int _diff_copy_branch(struct Node* orig, struct Node* diff, LOG_PARAMS);

//===================================================================

#define diff_tree_ctor(tree) \
       _diff_tree_ctor(tree, LOG_ARGS)

#define diff_tree_dtor(tree) \
       _diff_tree_dtor(tree, LOG_ARGS)

#define diff_read_from_file(tree, filename) \
       _diff_read_from_file(tree, filename, LOG_ARGS)

#define diff_read_from_console(tree) \
       _diff_read_from_console(tree, LOG_ARGS)

#define diff_execute(tree, diff) \
       _diff_execute(tree, diff, LOG_ARGS)

#define node_diff_execute(orig, diff) \
       _node_diff_execute(orig, diff, LOG_ARGS)

#define diff_operand_node(orig, diff) \
       _diff_operand_node(orig, diff, LOG_ARGS)

#define diff_out_to_file(diff, filename) \
       _diff_out_to_file(diff, filename, LOG_ARGS)

#define diff_out_to_console(diff) \
       _diff_out_to_console(diff, LOG_ARGS)

#define buffer_struct_init(buffer_struct, buffer, size, pos) \
       _buffer_struct_init(buffer_struct, buffer, size, pos, LOG_ARGS)

#define buffer_dump(buffer_struct) \
       _buffer_dump(buffer_struct, LOG_ARGS)

#define node_read_from_buffer(node, buffer_struct) \
       _node_read_from_buffer(node, buffer_struct, LOG_ARGS)

#define read_node_data(node, buffer_struct) \
       _read_node_data(node, buffer_struct, LOG_ARGS)

#define read_node_with_children(node, buffer_struct) \
       _read_node_with_children(node, buffer_struct, LOG_ARGS) 

#define read_constant(node, buffer_struct) \
       _read_constant(node, buffer_struct, LOG_ARGS)

#define read_function(node, buffer_struct) \
       _read_function(node, buffer_struct, LOG_ARGS)

#define read_closing_bracket(buffer_struct) \
       _read_closing_bracket(buffer_struct, LOG_ARGS)

#define read_opening_bracket(buffer_struct) \
       _read_opening_bracket(buffer_struct, LOG_ARGS)

#define input_skip_blanks(buffer) \
       _input_skip_blanks(buffer, LOG_ARGS)

#define diff_scan_input() \
       _diff_scan_input(LOG_ARGS)

#define diff_copy_branch(orig, diff) \
       _diff_copy_branch(orig, diff, LOG_ARGS)

#define diff_operand_mul(orig, diff) \
       _diff_operand_mul(orig, diff, LOG_ARGS)

#define diff_operand_div(orig, diff) \
       _diff_operand_div(orig, diff, LOG_ARGS)

#define diff_operand_pow(orig, diff) \
       _diff_operand_pow(orig, diff, LOG_ARGS)

#define diff_operand_add_or_sub(orig, diff, oper) \
       _diff_operand_add_or_sub(orig, diff, oper, LOG_ARGS)

#define diff_operand_sin(orig, diff) \
       _diff_operand_sin(orig, diff, LOG_ARGS)

#define diff_operand_cos(orig, diff) \
       _diff_operand_cos(orig, diff, LOG_ARGS)

#define diff_operand_tg(orig, diff) \
       _diff_operand_tg(orig, diff, LOG_ARGS)

#define diff_operand_ctg(orig, diff) \
       _diff_operand_ctg(orig, diff, LOG_ARGS)

#define diff_operand_arcsin_and_arccos(orig, diff, oper) \
       _diff_operand_arcsin_and_arccos(orig, diff, oper, LOG_ARGS)

#define diff_operand_arctg_and_arcctg(orig, diff, oper) \
       _diff_operand_arctg_and_arcctg(orig, diff, oper, LOG_ARGS)

#define node_simplify(diff) \
       _node_simplify(diff, LOG_ARGS)

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

#define replace_with_const(node) \
       _replace_with_const(node, LOG_ARGS)

#define cut_constant(diff, node) \
       _cut_constant(diff, node, LOG_ARGS)  

