#include <stdlib.h>

#include "diff/diff.h"
#include "general/general.h"

//===================================================================

int main(int, char* argv[]) {

    FILE* logs_file = open_log_file(argv[1]);
    if (logs_file == NULL)
        return -1;

    struct Tree tree = { 0 };

    int err_val = diff_tree_ctor(&tree); $

    char* buffer = NULL;

    #ifdef DIFF_READ_FROM_FILE

        buffer = diff_read_from_file(&tree, argv[2]); $
    #else
        buffer = diff_read_from_comsole(&tree); &

    #endif

    err_val = diff_execute(&tree); $

    err_val = diff_tree_dtor(&tree); $

    #ifdef DIFF_READ_FROM_FILE

        free(buffer);

    #endif

    err_val = close_log_file(); $

    return 0;
}