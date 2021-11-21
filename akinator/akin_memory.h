#pragma once

#include "akinator.h"

//==========================================================================

#define akinator_free_allocated_mem() \
       _akinator_free_allocated_mem(LOG_ARGS)

#define akinator_init_new_node(node, data) \
       _akinator_init_new_node(node, data, LOG_ARGS)

//==========================================================================

int _akinator_init_new_node(struct Node* node, const char* data, LOG_PARAMS);

int _akinator_free_allocated_mem(LOG_PARAMS);