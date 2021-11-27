#include "diff_tex.h"

//===================================================================

int  _tree_latex_execute(struct Tree* tree, FILE* tex, LOG_PARAMS) {

    TREE_PTR_CHECK(tree);
    if (tex == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }



    return 0;
}

//===================================================================

FILE* _tree_latex_start(const char*  tex_name, LOG_PARAMS) {

    if (!tex_name) {

        error_report(INV_FILE_NAME);
        return NULL;
    }

    FILE* tex = open_log_file(tex_name);
    if (!tex_name)
        return NULL;

    return tex;
}

//===================================================================

int _tree_latex_add_conspect(struct Tree* tree, FILE* tex, LOG_PARAMS) {

    TREE_PTR_CHECK(tree);
    if (!tex)
        return -1;

    return 0;
}

//===================================================================

int _tree_latex_finish(struct Tree* tree, LOG_PARAMS) {

    TREE_PTR_CHECK(tree)

    return 0;    
}