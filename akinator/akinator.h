#pragma once

#include "akinator_config.h"
#include "../tree/tree.h"
#include "akin_memory.h"

//===================================================================

struct Compare_obj {

    struct Node* node;
    struct Stack* stack;
};

//===================================================================

#define clean_buffer(buffer, size) {                                \
    do                                                              \
    {                                                               \
        memset(buffer, 0, size * sizeof(char));                     \
                                                                    \
    } while(0);                                                     \
}

//===================================================================

#ifdef AKINATOR_LOGS

    #define akinator_log_report() \
            log_report()

#else

    #define akinator_log_report() ""

#endif

#ifdef AKINATOR_TREE_VERIFICATE

    #define AKINATOR_TREE_VERIFICATION(tree) {                      \
                                                                    \
        do                                                          \
        {                                                           \
            int is_ok = akinator_tree_validator(tree);              \
            if (is_ok == -1)                                        \
                return -1;                                          \
        } while(0);                                                 \
    }                                           

#else 

    #define AKINATOR_TREE_VERIFICATION(tree) ""

#endif

//===================================================================

enum akinator_menu_answers {

    compare    = 1,
    definition = 2,
    guess      = 3,
    save       = 4,
    quit       = 5
};

//===================================================================

#define COMPARE_OBJ_PTR_CHECK(compare_obj_ptr) {                    \
                                                                    \
    if (compare_obj_ptr == NULL) {                                  \
                                                                    \
        error_report(INV_COMPARE_OBJ_PTR);                          \
        return -1;                                                  \
    }                                                               \
}

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

#define akinator_play_game(tree) \
       _akinator_play_game(tree, LOG_ARGS)

#define akinator_say_hello() \
       _akinator_say_hello(LOG_ARGS)

#define akinator_say_bye_bye() \
       _akinator_say_bye_bye(LOG_ARGS)

#define akinator_play_compare(tree) \
       _akinator_play_compare(tree, LOG_ARGS)

#define akinator_play_definition(tree) \
       _akinator_play_definition(tree, LOG_ARGS)

#define akinator_play_guess(tree) \
       _akinator_play_guess(tree, LOG_ARGS)

#define akinator_show_menu() \
       _akinator_show_menu(LOG_ARGS)

#define print_line_of_stars() \
       _print_line_of_stars(LOG_ARGS)

#define akinator_play_game(tree) \
       _akinator_play_game(tree, LOG_ARGS)

#define akinator_get_answer() \
       _akinator_get_answer(LOG_ARGS)

#define akinator_read_input(buf) \
       _akinator_read_input(buf, LOG_ARGS)

#define input_skip_blanks(buffer) \
       _input_skip_blanks(buffer, LOG_ARGS)

#define akinator_get_yes_or_no() \
       _akinator_get_yes_or_no(LOG_ARGS)

#define akinator_add_object(tree, node) \
       _akinator_add_object(tree, node, LOG_ARGS)

#define akinator_save_changes(tree) \
       _akinator_save_changes(tree, LOG_ARGS)

//===================================================================

#define buffer_struct_init(buffer_struct, buffer, size, pos) \
       _buffer_struct_init(buffer_struct, buffer, size, pos, LOG_ARGS)

#define buffer_dump(buffer_struct) \
       _buffer_dump(buffer_struct, LOG_ARGS)

#define tree_read_from_file(tree, text, filename) \
       _tree_read_from_file(tree, text, filename, LOG_ARGS)

#define node_read_from_buffer(node, buffer_struct) \
       _node_read_from_buffer(node, buffer_struct, LOG_ARGS)

#define akinator_init_base(tree, text, filename) \
       _akinator_init_base(tree, text, filename, LOG_ARGS)

#define akinator_tree_ctor(tree) \
       _akinator_tree_ctor(tree, LOG_ARGS)

#define akinator_tree_validator(tree) \
       _akinator_tree_validator(tree, LOG_ARGS)

#define akinator_node_validator(node) \
       _akinator_node_validator(node, LOG_ARGS)

#define akinator_tree_search(tree, stack, hash) \
       _akinator_tree_search(tree, stack, hash, LOG_ARGS)

#define akinator_node_search(node, stack, hash) \
       _akinator_node_search(node, stack, hash, LOG_ARGS)      

#define akinator_get_definition(tree, node_name, output) \
       _akinator_get_definition(tree, node_name, output, LOG_ARGS)

#define show_definition(obj, output) \
       _show_definition(obj, output, LOG_ARGS)

#define akinator_compare_objects(tree, first_name, second_name, output) \
       _akinator_compare_objects(tree, first_name, second_name, output, LOG_ARGS)

#define compare_object_init(obj, node, stack) \
       _compare_object_init(obj, node, stack, LOG_ARGS)

#define akinator_show_difference(first, second, output) \
       _akinator_show_difference(first, second, output, LOG_ARGS)

//===================================================================

int _akinator_play_compare(struct Tree* tree, LOG_PARAMS);

int _akinator_play_guess(struct Tree* tree, LOG_PARAMS);

int _akinator_play_definition(struct Tree* tree, LOG_PARAMS);

int _akinator_play_game(struct Tree* tree, LOG_PARAMS);

//===================================================================

int _compare_object_init(struct Compare_obj* object, struct Node* node, struct Stack* stack, 
                                                                                LOG_PARAMS);

int _akinator_show_difference(struct Compare_obj* first_obj, 
                              struct Compare_obj* second_obj, FILE* output, LOG_PARAMS);

int _akinator_compare_objects(struct Tree* tree, const char* first_name, const char* second_name, 
                                                                        FILE* output, LOG_PARAMS);

int _show_definition(struct Compare_obj* obj, FILE* output, LOG_PARAMS);

int _akinator_get_definition(struct Tree* tree, const char* node_name, 
                                            FILE* output, LOG_PARAMS);

Node* _akinator_tree_search(struct Tree* tree, struct Stack* stack, int64_t hash, LOG_PARAMS);

Node* _akinator_node_search(struct Node* node, struct Stack* stack, int64_t hash, LOG_PARAMS);

int _akinator_tree_validator(struct Tree* tree, LOG_PARAMS);

int _akinator_node_validator(struct Node* node, LOG_PARAMS);

int _akinator_tree_ctor(struct Tree* tree, LOG_PARAMS);

int _buffer_dump(struct Buffer_struct* buffer_struct, LOG_PARAMS);

int _akinator_init_base(struct Tree* tree, struct Text* text, const char* filename, 
                                                                       LOG_PARAMS);

int _buffer_struct_init(struct Buffer_struct* buffer_struct, char* buffer, 
                                           int size, int pos, LOG_PARAMS);

int _tree_read_from_file(struct Tree* tree, struct Text* text, const char* filename, 
                                                                        LOG_PARAMS);

int _node_read_from_buffer(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                      LOG_PARAMS);

//===================================================================