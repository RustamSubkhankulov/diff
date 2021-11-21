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

int _diff_tree_ctor(struct Tree* tree, LOG_PARAMS);

int _diff_tree_dtor(struct Tree* tree, LOG_PARAMS);

char* _diff_read_from_file(struct Tree* tree, const char* filename, LOG_PARAMS);

char* _diff_read_from_console(struct Tree* tree, LOG_PARAMS);

int _diff_execute(struct Tree* tree, LOG_PARAMS);

//===================================================================

#define diff_tree_ctor(tree) \
       _diff_tree_ctor(tree, LOG_ARGS)

#define diff_tree_dtor(tree) \
       _diff_tree_dtor(tree, LOG_ARGS)

#define diff_read_from_file(tree, filename) \
       _diff_read_from_file(tree, filename, LOG_ARGS)

#define diff_read_from_console(tree) \
       _diff_read_from_console(tree, LOG_ARGS)

#define diff_execute(tree) \
       _diff_execute(tree, LOG_ARGS)





