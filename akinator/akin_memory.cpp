#include <string.h>
#include <stdlib.h>

#include "akin_memory.h"

//===================================================================

static const char* Allocated_names[Akinator_max_new_objects_count] = { 0 };

static int Allocated_names_counter = 0;

//===================================================================

int _akinator_init_new_node(struct Node* node, const char* data, LOG_PARAMS) {

    akinator_log_report();

    if (Allocated_names_counter == Akinator_max_new_objects_count) {

        error_report(AKINATOR_MAX_NEW_OBJECTS);
        return 0;
    }

    char* new_mem = strdup(data);

    Allocated_names[Allocated_names_counter++] = (const char*)new_mem;

    int ret = node_init(node, (const char*)new_mem);
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

int _akinator_free_allocated_mem(LOG_PARAMS) {

    akinator_log_report();

    for (int counter = 0; counter < Allocated_names_counter; counter++) 
        free((void*)Allocated_names[counter]);

    return 0;
}