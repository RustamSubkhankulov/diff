#include <stdlib.h>
#include <string.h>

#include "diff_tex.h"
#include "../general/general.h"

const char* Tex_name = "diff.tex";

//===================================================================

int  _tree_latex_execute(struct Tree* tree, FILE* tex, LOG_PARAMS) {

    TREE_PTR_CHECK(tree);
    if (tex == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }


    fprintf(tex, "\n\n \\textbf{Original expression:}\n\n");

    fprintf(tex, "\\begin{math}\n");

    int ret = node_write_latex(tree->root, tex);
    if (ret == -1)
        return -1;

    fprintf(tex, "\\end{math}\n");

    fprintf(tex, "\n");

    return 0;
}

//===================================================================

int _print_latex_operand(struct Node* node, FILE* tex, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    if (!tex) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    

    return 0;
}

//===================================================================

int _print_latex_data(struct Node* node, FILE* tex, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    if (!tex) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    switch(node->data_type) {

        case CONSTANT: {

            fprintf(tex, "%g", node->data.constant);
            break;
        }

        case VARIABLE: {

            fprintf(tex, "%c", node->data.variable);
            break;
        }

        case OPERAND: {

            return print_latex_operand(node, tex);
        }

        default: {

            error_report(DIFF_INV_DATA_TYPE);
            break;
        }
    }

    return 0;
}

//===================================================================

int _node_write_latex(struct Node* node, FILE* tex, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    int is_terminating = 0;
    if (!node->left_son && !node->right_son)
        is_terminating = 1;
    
    if (!is_terminating) 
        fprintf(tex, "(");

    if (node->data_type == OPERAND && is_function_operand(node))
        print_node_data(node, tex);

    if (node->left_son)
        node_write_latex(node->left_son, tex);

    if (node->data_type != OPERAND
    || (node->data_type == OPERAND && !is_function_operand(node)))
        print_node_data(node, tex);

    if (node->right_son)
        node_write_latex(node->right_son, tex);

    if (!is_terminating)
        fprintf(tex, ")");

    return 0;
}

//===================================================================

int _latex_write_title(FILE* tex, LOG_PARAMS) {

    diff_log_report();

    if (!tex) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    fprintf(tex, "\\documentclass[a4paper, 12pt] {article}\n");
    fprintf(tex, "\\usepackage{graphicx}\n");
    fprintf(tex, "\\usepackage[utf8]{inputenc}\n");
    fprintf(tex, "\\usepackage{multirow}\n");
    fprintf(tex, "\\usepackage{mathtext}\n");
    fprintf(tex, "\\usepackage[T2A]{fontenc}\n");
    fprintf(tex, "\\usepackage{titlesec}\n");
    fprintf(tex, "\\usepackage{float}\n");
    fprintf(tex, "\\usepackage{empheq}\n");
    fprintf(tex, "\\usepackage{amsfonts}\n");
    fprintf(tex, "\\usepackage{amsmath}\n");
    fprintf(tex, "\\title{Semester work in mathematical analysis}\n");
    fprintf(tex, "\\author{Rustam Subkhankulov \thanks{sponsored By A.Y.Petrovich}}\n");
    fprintf(tex, "\\date{November 2021}\n");
    fprintf(tex, "\\begin{document} \n");
    fprintf(tex, "\\maketitle\n");

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

FILE* _tree_latex_start(struct Tree* tree, LOG_PARAMS) {

    FILE* tex = open_latex_file(Tex_name);
    if (!tex)
        return NULL;

    int ret = latex_write_title(tex);
    if (ret == -1)
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

int _call_latex(const char* tex_dest, LOG_PARAMS) {

    diff_log_report();

    char buffer[System_cmnd_buf_size] = { 0 };
    sprintf(buffer, "pdflatex %s%s -output-directory=%s%s >/dev/null", 
                                                  LATEX_DIR, Tex_name, 
                                                  LATEX_DIR, tex_dest);

    // int ret =  system(buffer);
    // if (ret != 0)
    //     return -1;

    // clean_buffer(buffer, System_cmnd_buf_size);

    // sprintf(buffer, "evince %s", tex_dest);
    return system(buffer);
}

//===================================================================

int _tree_latex_finish(struct Tree* tree, FILE* tex, const char* tex_dest, 
                                                               LOG_PARAMS) {

    TREE_PTR_CHECK(tree);

    if (!tex_dest) {

        error_report(INV_FILE_NAME);
        return -1;
    }

    fprintf(tex, "\\end{document}\n");

    int ret = close_file(tex);
    if (ret == -1)
        return -1;

    ret = call_latex(tex_dest);
    if (ret != 0)
        return -1;

    return 0;    
}