#pragma once

#include <cstddef>

#define TEMP_DIR "/tmp/"

//===================================================================

 const int Dir_name_size = 100;

 const int System_cmnd_buf_size = 200;

//===================================================================

//TREE PARAMETERS

//Element type of the list structure
typedef const char * elem_t;

//Name of type using in list(for logs)
#define TYPE_NAME "const char*"

//Specificator for type using in list
#define ELEM_SPEC "%s"

//===================================================================

//TREE OPTIONS

#define TREE_VERIFICATE

//Logs for tree
//Generates .png files in /images and list_logs.html in text_files/
#define TREE_LOGS

//Calls dump of the tree every time validator is called
#define TREE_DEBUG

//GRAPHVIZ
#define TREE_GRAPHVIZ

#define TREE_CALCULATE_HASH_FROM_DATA

//Definition for hash calculating
#define HASH_POINTER_TO_CHAR

//==================================================================

//SUPPORTING DEFINITIONS

const unsigned Poison_value = 228;

//==================================================================