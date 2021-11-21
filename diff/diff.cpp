#include "diff.h"

//===================================================================

int _diff_tree_ctor(struct Tree* tree, LOG_PARAMS) {

    diff_tree_report();
    TREE_PTR_CHECK(tree);

    int ret = tree_ctor(tree);
    if (ret == -1)
        return -1;

    tree->root = (struct Node*)node_allocate_memory();
    if (!tree->root)
        return -1;

    DIFF_TREE_VERIFICATION(tree);
}

//===================================================================

int _diff_tree_dtor(struct Tree* tree, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(tree);

    return tree_dtor(tree);
}

//===================================================================

char* _diff_read_from_file(struct Tree* tree, const char* filename, LOG_PARAMS);

//===================================================================

char* _diff_read_from_console(struct Tree* tree, LOG_PARAMS);

//===================================================================

int _diff_execute(struct Tree* tree, LOG_PARAMS);

