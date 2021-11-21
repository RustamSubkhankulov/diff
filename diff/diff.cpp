#include "diff.h"
#include "../text_processing/text_processing.h"
#include "../general/general.h"

//===================================================================

int _diff_tree_ctor(struct Tree* tree, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(tree);

    int ret = tree_ctor(tree);
    if (ret == -1)
        return -1;

    tree->root = (struct Node*)node_allocate_memory();
    if (!tree->root)
        return -1;

    DIFF_TREE_VERIFICATION(tree);

    return 0;
}

//===================================================================

int _diff_tree_dtor(struct Tree* tree, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(tree);

    return tree_dtor(tree);
}

//===================================================================

char* _diff_read_from_file(struct Tree* tree, const char* filename, LOG_PARAMS) {

    diff_log_report();

    if (!tree) {

        error_report(INV_TREE_PTR);
        return NULL;
    }

    int size = 0;
    char* buffer = copy_from_file_to_buffer(filename, &size);
    if (buffer == NULL)
        return NULL;

    struct Buffer_struct buffer_struct = { 0 };
    buffer_struct_init(&biffer_struct, buffer, size, 0);

    return node_read_from_buffer(tree->root, &buffer_struct);
}

//===================================================================

int _buffer_dump(struct Buffer_struct* buffer_struct, LOG_PARAMS) {

    tree_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    const char* buffer = buffer_struct->buffer;
    int size = buffer_struct->size;
    int pos = buffer_struct->pos;

    if (buffer == NULL) {

        error_report(INV_BUF_PTR);
        return -1;
    }

    fprintf(logs_file, "<style> td\n { padding: 7px;"
                                  " border: 1px solid black;"
                                  " border-collapse: collapse;}\n</style>");

    fprintf(logs_file, "<table width = \" 100%% \" "
                              "cellspacing=\"0\" "
                              "cellpadding=\"4\" "
                              "border = \"5\" "
                              "style = \" "
                              "padding: 15px; "
                              "background-color: lightgrey;>\"\n");

    int counter = 0;
    while (counter < size) {

        int second_counter = counter;
        fprintf(logs_file, "\n<tr><td style = \"background-color: #c2e7c2\"><b>Index</b></td>");

        for(int ct = 0; ct < 16 && second_counter < size; ct++, second_counter++) {

            if (second_counter == pos)
                fprintf(logs_file, "<td style = \"background-color: green;"
                                                 "color: white;\"> %d </td>", 
                                                             second_counter);
            else
                fprintf(logs_file,"<td style = \"background-color: #c2e7c2\"> %d </td>", 
                                                                        second_counter);
        }

        fprintf(logs_file, "</tr><tr><td><b> Symbol </b></td>");

        for (int ct = 0; ct < 16 && counter < size; ct++, counter++) {

            if (counter == pos)
                fprintf(logs_file, "<td style = \"background-color: green;"
                                                 "color: white;\">");
            else 
                fprintf(logs_file, "<td>");

            if (isblank(buffer[counter]) || buffer[counter] == 0)
                fprintf(logs_file, " %d </td> ", buffer[counter]);
            else
                fprintf(logs_file, " %c </td> ", buffer[counter]);
        }

        fprintf(logs_file, "</tr>");

    }

    fprintf(logs_file, "</table> \n");

    return 0;
}

//===================================================================

int _buffer_struct_init(struct Buffer_struct* buffer_struct, char* buffer, 
                                            int size, int pos, LOG_PARAMS)  {

    tree_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    if (buffer == NULL) {

        error_report(INV_BUF_PTR);
        return -1;
    }

    buffer_struct->buffer = buffer;
    buffer_struct->size   = size;
    buffer_struct->pos    = pos;

    return 0;
}

//===================================================================

int _node_read_from_buffer(struct Node* node, struct Buffer_struct* buffer_struct, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    int offset = 0;
    char symb = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %c %n", 
                                                              &symb, &offset);
    if (scanned == -1 || scanned == 0) {

        error_report(TEXT_PROCESSING_ERR);
        buffer_dump(buffer_struct);
        return -1;
    }
    
    if (symb != '(') {

        error_report(TREE_TEXT_INV_SYNTAXIS);
        buffer_dump(buffer_struct);
        return -1;
    }

    buffer_struct->pos += offset;

    symb = 0;
    int after_word = 0;
    int before_word = 0;

    while (symb != '(' && symb != ')') {

        int ret = sscanf(buffer_struct->buffer + buffer_struct->pos, " %n%*s%n %n%c", 
                                                                        &before_word, 
                                                                         &after_word, 
                                                                             &offset, 
                                                                              &symb);

        if (ret != 1) {

            error_report(TREE_TEXT_INV_SYNTAXIS);
            buffer_dump(buffer_struct);
            return -1;
        }
        
        if (*(buffer_struct->buffer + buffer_struct->pos + before_word) == '{' 
         || *(buffer_struct->buffer + buffer_struct->pos + before_word) == '}') {

            error_report(TREE_TEXT_EMPTY_NODE_NAME);
            buffer_dump(buffer_struct);
            return -1;
        }

        buffer_struct->pos += offset;

    }

    *(buffer_struct->buffer + buffer_struct->pos - offset + after_word) = '\0';
    node_init(node, node_data);

    if (*(buffer_struct->buffer + buffer_struct->pos) == ')') {

        buffer_struct->pos++;
        return 0;
    }


    if (*(buffer_struct->buffer + buffer_struct->pos) == '(') {

        node->special_flag = 1;

        int ret = node_add_sons(node);
        if (ret == -1)
            return -1;

        ret = node_read_from_buffer(node->right_son, buffer_struct);
        if (ret == -1)
            return -1;

        ret = node_read_from_buffer(node->left_son, buffer_struct);
        if (ret == -1)
            return -1;

        sscanf(buffer_struct->buffer + buffer_struct->pos, " %c %n", 
                                                    &symb, &offset);

        if (symb == '}') {

            buffer_struct->pos += offset;
            return 0;
        }

        else {

            error_report(TREE_TEXT_NO_CLOSING_BRACKET);
            buffer_dump(buffer_struct);
            return -1;
        } 

    }

    return 0;
}

//===================================================================

char* _diff_read_from_console(struct Tree* tree, LOG_PARAMS) {

    diff_log_report();

    if (!tree) {

        error_report(INV_TREE_PTR);
        return NULL;
    }

    return 0;
}

//===================================================================

int _diff_execute(struct Tree* tree, struct Tree* diff, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(tree);
    TREE_PTR_CHECK(diff);

    return 0;
}

//===================================================================

int _diff_out_to_file(struct Tree* diff, const char* filename, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(diff);

    return 0;
}

//===================================================================

int _diff_out_to_console(struct Tree* diff, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(diff);

    return 0;
}

