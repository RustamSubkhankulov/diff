#pragma once

#include "diff_config.h"

#include "../tree/tree.h"
#include "../logs/errors_and_logs.h"

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

enum operands {

       ADD = '+',
       SUB = '-',
       MUL = '*',
       DIV = '/',
       POW = '^'
};

//===================================================================

const int Operands_number = 5;

const char Operands[Operands_number] = {ADD, SUB, MUL, DIV, POW};

//===================================================================

int _diff_tree_ctor(struct Tree* tree, LOG_PARAMS);

int _diff_tree_dtor(struct Tree* tree, LOG_PARAMS);

int _diff_execute(struct Tree* tree, struct Tree* diff, LOG_PARAMS);

int _node_diff_execute(struct Node* node, struct Node* diff_node, LOG_PARAMS);

int _diff_out_to_file(struct Tree* diff, const char* filename, LOG_PARAMS);

int _diff_out_to_console(struct Tree* diff, LOG_PARAMS);

int _buffer_struct_init(struct Buffer_struct* buffer_struct, char* buffer, 
                                           int size, int pos, LOG_PARAMS);

int _node_read_from_buffer(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                      LOG_PARAMS);

int _buffer_dump(struct Buffer_struct* buffer_struct, LOG_PARAMS);

char* _diff_read_from_file(struct Tree* tree, const char* filename, LOG_PARAMS);

char* _diff_read_from_console(struct Tree* tree, LOG_PARAMS);

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

#define node_diff_execute(orig_node, diff_node) \
       _node_diff_execute(orig_node, diff_node, LOG_ARGS)

#define diff_constant_node(orig_node, diff_node) \
       _diff_constant_node(orig_node, diff_node, LOG_ARGS)

#define diff_variable_node(orig_node, diff_node) \
       _diff_variable_node(orig_node, diff_node, LOG_ARGS)

#define diff_operand_node(orig_node, diff_node) \
       _diff_operand_node(orig_node, diff_node, LOG_ARGS)

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

#define read_closing_bracket(buffer_struct) \
       _read_closing_bracket(buffer_struct, LOG_ARGS)

#define read_opening_bracket(buffer_struct) \
       _read_opening_bracket(buffer_struct, LOG_ARGS)

#define input_skip_blanks(buffer) \
       _input_skip_blanks(buffer, LOG_ARGS)

#define diff_scan_input() \
       _diff_scan_input(LOG_ARGS)

