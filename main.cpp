#include <stdlib.h>

#include "diff/diff.h"
#include "general/general.h"

//===================================================================

int main(int, char* argv[]) {

    FILE* logs_file = open_log_file(argv[1]);
    if (logs_file == NULL)
        return -1;

    for (int ct = 0; ct < Functions_number; ct++)
        printf("\n\n %s %d \n\n", Functions[ct].name, Functions[ct]. code);

    struct Tree tree = { 0 };

    int err_val = diff_tree_ctor(&tree); $

    char* buffer = NULL;

    #ifdef DIFF_READ_FROM_FILE
        buffer = diff_read_from_file(&tree, argv[2]); 
    #else
        buffer = diff_read_from_console(&tree); 
    #endif

    if (buffer == NULL)
        return -1;

    struct Tree diff = { 0 };
    err_val = diff_tree_ctor(&diff); $

    err_val = diff_execute(&tree, &diff); $

    #ifdef DIFF_OUT_TO_FILE
        err_val = diff_out_to_file(&diff, argv[3]);
    #else
        err_val = diff_out_to_console(&diff);
    #endif

    err_val = diff_tree_dtor(&tree); $
    err_val = diff_tree_dtor(&diff); $

    free(buffer);
    err_val = close_log_file(); $

    return 0;
}