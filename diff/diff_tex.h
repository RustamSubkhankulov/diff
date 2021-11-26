#pragma once

#include "../diff/diff.h"

//===================================================================

FILE*  _tree_latex_execute(struct Tree* tree, const char* tex_name, LOG_PARAMS);

int _tree_write_start(FILE* tex, LOG_PARAMS);

int _tree_latex_add_conspect(struct Tree* tree, FILE* tex, LOG_PARAMS);

int _tree_latex_finish(struct Tree* tree, LOG_PARAMS);

//===================================================================

#define tree_latex_execute(tree, tex_name) \
       _tree_latex_execute(tree, tex_name, LOG_ARGS)

#define tree_write_start(tex) \
       _tree_write_start(tex, LOG_ARGS)    

#define tree_latex_add_conspect(tree, tex) \
       _tree_latex_add_conspect(tree, tex, LOG_ARGS)

#define tree_latex_finish(tree) \
       _tree_latex_finish(tree, LOG_ARGS)