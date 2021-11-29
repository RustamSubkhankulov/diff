#pragma once

#include "../diff/diff.h"

//===================================================================

int  _tree_latex_execute(struct Tree* tree, FILE* tex, LOG_PARAMS);

FILE*  _tree_latex_start(struct Tree* tree, const char* tex_name, LOG_PARAMS);

int _tree_latex_add_conspect(struct Tree* tree, FILE* tex, LOG_PARAMS);

int _tree_latex_finish(struct Tree* tree, FILE* tex, LOG_PARAMS);

int _latex_write_title(FILE* tex, LOG_PARAMS);

FILE* _open_latex_file(const char* tex_name, LOG_PARAMS);

//===================================================================

#define open_latex_file(tex_name) \
       _open_latex_file(tex_name, LOG_ARGS)

#define latex_write_title(tex) \
       _latex_write_title(tex, LOG_ARGS)

#define tree_latex_execute(tree, tex_name) \
       _tree_latex_execute(tree, tex_name, LOG_ARGS)

#define tree_latex_start(tree, tex) \
       _tree_latex_start(tree, tex, LOG_ARGS)    

#define tree_latex_add_conspect(tree, tex) \
       _tree_latex_add_conspect(tree, tex, LOG_ARGS)

#define tree_latex_finish(tree, tex) \
       _tree_latex_finish(tree, tex, LOG_ARGS)
