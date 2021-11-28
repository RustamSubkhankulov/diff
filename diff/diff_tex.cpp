#include "diff_tex.h"
#include "../general/general.h"

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

int _tree_write_title(FILE* tex, LOG_PARAMS) {

    diff_log_report();

    if (!tex) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    return 0;
}

//===================================================================

FILE* _open_latex_file(const char* tex_name, LOG_PARAMS) {

    diff_log_report();

    char dir_name_buffer[Directory_name_buffer_size] = { 0 };
    sprintf(dir_name_buffer, "%s%s", LATEX_DIR, tex_name);

    return open_file(dir_name_buffer, "wb");
}

//===================================================================

FILE* _tree_latex_start(struct Tree* tree, const char* tex_name, LOG_PARAMS) {

    if (!tex_name) {

        error_report(INV_FILE_NAME);
        return NULL;
    }

    FILE* tex = open_latex_file(tex_name);
    if (!tex)
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