#pragma once

#include "tree_config.h"
#include "../logs/errors_and_logs.h"

//===================================================================

struct Node {

    elem_t data;

    #ifdef TREE_CALCULATE_HASH_FROM_DATA

        int64_t hash;
        
    #endif

    struct Node* left_son;
    struct Node* right_son;

    bool special_flag;
};

//===================================================================

struct Tree {

    struct Node* root;

};

//===================================================================

struct Node_dot {

    FILE* graph;

    int father;
    int depth;
    int is_left;
};

//===================================================================

struct Buffer_struct {

    char* buffer;

    int size;
    int pos;
};

//===================================================================

#ifdef TREE_LOGS

    #define tree_log_report() \
            log_report()

#else 

    #define tree_log_report() ""

#endif

//===================================================================

#ifndef TREE_VERIFICATE

    #define TREE_VERIFICATION(tree) ""

#else

    #define TREE_VERIFICATION(tree) {                                   \
                                                                        \
        do                                                              \
        {                                                               \
            int is_ok = tree_validator(tree);                           \
            if (is_ok == -1)                                            \
                return -1;                                              \
        } while(0);                                                     \
    }

#endif

//===================================================================


//===================================================================

#define TREE_PTR_CHECK(tree_ptr) {                                  \
                                                                    \
    if (tree_ptr == NULL) {                                         \
                                                                    \
        error_report(INV_TREE_PTR);                                 \
        return -1;                                                  \
    }                                                               \
}

//===================================================================

#define NODE_PTR_CHECK(node_ptr) {                                  \
                                                                    \
    if (node_ptr == NULL) {                                         \
                                                                    \
        error_report(INV_NODE_PTR);                                 \
        return -1;                                                  \
    }                                                               \
}

//===================================================================

#define NODE_DOT_PTR_CHECK(node_dot_ptr) {                          \
                                                                    \
    if (node_dot_ptr == NULL) {                                     \
                                                                    \
        error_report(INV_NODE_DOT_PTR);                             \
        return -1;                                                  \
    }                                                               \
}

//===================================================================

int _tree_save_to_file(struct Tree* tree, const char* filename, LOG_PARAMS);

int _node_save_to_file(struct Node* node, FILE* output, LOG_PARAMS);

int _node_dot_fill(struct Node_dot* node_dot, FILE* graph_output, 
                  int father, int depth, int is_left, LOG_PARAMS);

int _tree_clear_check(struct Tree* tree, LOG_PARAMS);

int _tree_poison_check(struct Tree* tree, LOG_PARAMS);

int _tree_ctor(struct Tree* tree, LOG_PARAMS);

int _tree_dtor(struct Tree* tree, LOG_PARAMS);

int _node_visiter(struct Node* node, int (*node_function)(struct Node* node, LOG_PARAMS), 
                                                                             LOG_PARAMS);

int _tree_validator(struct Tree* tree, LOG_PARAMS);

int _node_validator(struct Node* node, LOG_PARAMS);

int _node_destruct(struct Node* node, LOG_PARAMS);

int _node_add_right_son(struct Node* node, LOG_PARAMS);

int _node_add_left_son(struct Node* node, LOG_PARAMS);

int _node_add_sons(struct Node* node, LOG_PARAMS);

void* _node_allocate_memory(LOG_PARAMS);

int _node_init(struct Node* node, elem_t value, LOG_PARAMS);

int _tree_free_memory(struct Tree* tree, LOG_PARAMS);

int _node_print(struct Node* node, LOG_PARAMS);

int _tree_dump(struct Tree* tree, LOG_PARAMS);

int _tree_print(struct Tree* tree, LOG_PARAMS);

int _tree_delete_branch(struct Node** node_ptr, LOG_PARAMS);

int _node_delete_branch(struct Node* node, LOG_PARAMS);

int _tree_draw_graph(struct Tree* tree, LOG_PARAMS);

int _node_draw_graph(struct Node* node, struct Node_dot* node_dot,  LOG_PARAMS);

int _tree_poisoning(struct Tree* tree, LOG_PARAMS);

int _tree_cleaning(struct Tree* tree, LOG_PARAMS);

int _graph_call_dot(LOG_PARAMS);

//===================================================================

#define node_save_to_file(node, output) \
       _node_save_to_file(node, output, LOG_ARGS)

#define tree_save_to_file(tree, filename) \
       _tree_save_to_file(tree, filename, LOG_ARGS)

#define node_dot_fill(node_dot, grapg_output, father, depth, is_left) \
       _node_dot_fill(node_dot, grapg_output, father, depth, is_left, LOG_ARGS)

#define graph_call_dot() \
       _graph_call_dot(LOG_ARGS)

#define node_draw_graph(node, node_dot) \
       _node_draw_graph(node, node_dot, LOG_ARGS)

#define tree_draw_graph(tree) \
       _tree_draw_graph(tree, LOG_ARGS)

#define tree_poisoning(tree) \
       _tree_poisoning(tree, LOG_ARGS)

#define tree_cleaning(tree) \
       _tree_cleaning(tree, LOG_ARGS)

#define tree_print(tree) \
       _tree_print(tree, LOG_ARGS)

#define tree_dump(tree) \
       _tree_dump(tree, LOG_ARGS)

#define node_print(tree) \
       _node_print(tree, LOG_ARGS)

#define tree_free_memory(tree) \
       _tree_free_memory(tree, LOG_ARGS)

#define node_allocate_memory() \
       _node_allocate_memory(LOG_ARGS)

#define tree_clear_check(tree) \
       _tree_clear_check(tree, LOG_ARGS)

#define tree_poison_check(tree) \
       _tree_poison_check(tree, LOG_ARGS)

#define tree_ctor(tree) \
       _tree_ctor(tree, LOG_ARGS)

#define tree_dtor(tree) \
       _tree_dtor(tree, LOG_ARGS) 

#define tree_validator(tree) \
       _tree_validator(tree, LOG_ARGS)

#define node_validator(node) \
       _node_validator(node, LOG_ARGS)

#define node_visiter(node, func) \
       _node_visiter(node, func, LOG_ARGS)

#define node_destruct(node) \
       _node_destruct(node, LOG_ARGS);                              

#define node_add_right_son(node) \
       _node_add_right_son(node, LOG_ARGS)

#define node_add_left_son(node) \
       _node_add_left_son(node, LOG_ARGS)

#define node_add_sons(node) \
       _node_add_sons(node, LOG_ARGS)

#define node_init(node, value) \
       _node_init(node, value, LOG_ARGS)

#define tree_delete_branch(node_ptr) \
       _tree_delete_branch(node_ptr, LOG_ARGS)

#define node_delete_branch(node) \
       _node_delete_branch(node, LOG_ARGS);                         

//===================================================================