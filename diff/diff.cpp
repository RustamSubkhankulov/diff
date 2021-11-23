#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "diff.h"
#include "../text_processing/text_processing.h"
#include "../general/general.h"

//===================================================================

static int _diff_constant_node(struct Node* orig_node, struct Node* diff_node, 
                                                                  LOG_PARAMS);

static int _diff_variable_node(struct Node* orig_node, struct Node* diff_node, 
                                                                  LOG_PARAMS);

static int  _diff_operand_node(struct Node* orig_node, struct Node* diff_node, 
                                                                  LOG_PARAMS);

static int _diff_operand_add_or_sub(struct Node* orig_node, struct Node* diff_node, 
                                                             int oper, LOG_PARAMS);

static int _diff_operand_mul(struct Node* orig_node, struct Node* diff_node, 
                                                                LOG_PARAMS);

static int _diff_operand_div(struct Node* orig_node, struct Node* diff_node, 
                                                                LOG_PARAMS);

static int _diff_operand_pow(struct Node* orig_node, struct Node* diff_node, 
                                                                LOG_PARAMS);

//===================================================================

int _diff_tree_ctor(struct Tree* tree, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(tree);

    int ret = tree_ctor(tree);
    if (ret == -1)
        return -1;

    ret = node_init_constant(tree->root, 0);
    tree->root->parent = No_parent;

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
    buffer_struct_init(&buffer_struct, buffer, size, 0);

    int ret = node_read_from_buffer(tree->root, &buffer_struct);
    if (ret == -1) {

        free(buffer);
        return NULL;
    }
    
    return buffer;
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
                                  " cellspacing=\"0\" "
                                  " cellpadding=\"4\" "
                                  " border = \"5\" "
                                  " style = \" "
                                  " padding: 15px; "
                                  " background-color: lightgrey;>\"\n");

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
    fflush(logs_file);

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

static int _read_constant(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                         LOG_PARAMS) {

    tree_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    double constant = 0;
    int offset = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %lf %n", 
                                                            &constant, &offset);

    if (scanned == -1 || scanned == 0) {

        error_report(TEXT_PROCESSING_ERR);
        buffer_dump(buffer_struct);
        return -1;
    }

    buffer_struct->pos += offset;
    //
    buffer_dump(buffer_struct);
    //
    return node_init_constant(node, constant);
}

//===================================================================

static int _read_node_data(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                       LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    char symb = 0;
    int offset = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %c %n", 
                                                              &symb, &offset);

    switch (scanned) {

        case -1: {

            error_report(TEXT_PROCESSING_ERR);
            buffer_dump(buffer_struct);
            return -1;
        }

        case 1: {

            if (symb_is_var_name(symb)) {

                int ret = node_init_variable(node, symb);
                if (ret == -1)
                    return -1;

                buffer_struct->pos += offset;
                //
                buffer_dump(buffer_struct);
                //
            }

            else if (symb_is_operand(symb)) {

                int ret = node_init_operand(node, symb);
                if (ret == -1)
                    return -1;

                buffer_struct->pos += offset;
                //
                buffer_dump(buffer_struct);
                //
            }

            else {

                int ret = read_constant(node, buffer_struct);
                if (ret == -1)
                    return -1;

                break;  
            }

            break;
        }

        default: {

            error_report(TREE_TEXT_INV_SYNTAXIS);
            return -1;
        }
    }

    //
    buffer_dump(buffer_struct);
    //

    return 0;
}

//===================================================================

static int _read_node_with_children(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                                LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    node->data_type = OPERAND;

    int ret = node_add_sons(node);
    if (ret == -1)
        return -1;

    ret = node_read_from_buffer(node->left_son, buffer_struct);
    if (ret == -1)
        return -1;

    //
    buffer_dump(buffer_struct);
    //

    ret = read_node_data(node, buffer_struct);
    if (ret == -1)
        return -1;

    //
    buffer_dump(buffer_struct);
    //

    ret = node_read_from_buffer(node->right_son, buffer_struct);
    if (ret == -1)
        return -1;

    //
    buffer_dump(buffer_struct);
    //

    return 0;
}

//===================================================================

static int _read_opening_bracket(struct Buffer_struct* buffer_struct, LOG_PARAMS) {

    tree_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    int offset = 0;
    char symb = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %c %n", 
                                                          &symb, &offset);

    if (scanned == -1 || scanned == 0) {

        error_report(TREE_TEXT_INV_SYNTAXIS);
        buffer_dump(buffer_struct);
        return -1;
    }

    if (symb == '(') {

        buffer_struct->pos += offset;
        return 0;
    }

    else {

        error_report(TREE_TEXT_INV_SYNTAXIS);
        buffer_dump(buffer_struct);
        return -1;
    } 

    //
    buffer_dump(buffer_struct);
    //

    return 0;

}

//===================================================================

static int _read_closing_bracket(struct Buffer_struct* buffer_struct, LOG_PARAMS) {

    tree_log_report();
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    int offset = 0;
    char symb = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %c %n", 
                                                          &symb, &offset);

    if (scanned == -1 || scanned == 0) {

        error_report(TREE_TEXT_INV_SYNTAXIS);
        buffer_dump(buffer_struct);
        return -1;
    }

    if (symb == ')') {

        buffer_struct->pos += offset;
        return 0;
    }

    else {

        error_report(TREE_TEXT_NO_CLOSING_BRACKET);
        buffer_dump(buffer_struct);
        return -1;
    } 

    //
    buffer_dump(buffer_struct);
    //

    return 0;
}

//===================================================================

int _node_read_from_buffer(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                       LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    //
    buffer_dump(buffer_struct);
    //

    int open_bracket = read_opening_bracket(buffer_struct);
    if (open_bracket == -1)
        return -1;

    int offset = 0;
    char symb = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %c %n", 
                                                              &symb, &offset);
    
    if (scanned == -1) {

        error_report(TEXT_PROCESSING_ERR);
        buffer_dump(buffer_struct);
        return -1;
    }

    if (scanned == 0 || symb != '(') {

        int ret = read_node_data(node, buffer_struct);
        if (ret == -1)
            return -1;
    }

    else if (symb == '(') {

        int ret = read_node_with_children(node, buffer_struct);
        if (ret == -1)
            return -1;
    }

    else {

        error_report(TREE_TEXT_INV_SYNTAXIS);
        buffer_dump(buffer_struct);
        return -1;
    }

    //
    buffer_dump(buffer_struct);
    //

    return read_closing_bracket(buffer_struct);
}

//===================================================================

static const char* _input_skip_blanks(char* buffer, LOG_PARAMS) {

    diff_log_report();

    char* slider = &buffer[strlen(buffer) - 1];

    while (isblank(*slider) || *slider == '\n')
        slider--;

    *(slider + 1) = '\0';

    const char* prepared_input = buffer;

    while (isblank(*prepared_input))
        prepared_input++;

    return prepared_input;
}

//===================================================================

static char* _diff_scan_input(LOG_PARAMS) {

    diff_log_report();

    char buf[Console_input_buf_size] = { 0 };

    char* scanned = fgets(buf, Console_input_buf_size, stdin);
    if (scanned == NULL) {

        error_report(CONSOLE_READ_ERROR);
        return NULL;
    }

    buf[strlen(buf) - 1] = '\0';

    while (strlen(buf) == 0) {

        clean_buffer(buf, Console_input_buf_size);

        printf("Error occurred during reading. Please try again.\n");

        fgets(buf, Console_input_buf_size, stdin);
        buf[strlen(buf) - 1] = '\0';
    }

    const char* prepared = input_skip_blanks(buf);
    return strdup(prepared);
}

//===================================================================

char* _diff_read_from_console(struct Tree* tree, LOG_PARAMS) {

    diff_log_report();
    if (tree == NULL) {

        error_report(INV_TREE_PTR);
        return NULL;
    }

    if (!tree) {

        error_report(INV_TREE_PTR);
        return NULL;
    }

    char* buffer = diff_scan_input();
    if (buffer == NULL)
        return NULL;

    struct Buffer_struct buffer_struct = { 0 };
    buffer_struct_init(&buffer_struct, buffer, (int)strlen(buffer), 0);

    int ret = node_read_from_buffer(tree->root, &buffer_struct);
    if (ret == -1) {

        free(buffer);
        return NULL;
    }

    return buffer;   
}

//===================================================================

int _diff_copy_branch(struct Node* orig_node, struct Node* diff_node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig_node);
    NODE_PTR_CHECK(diff_node);

    diff_node->data      = orig_node->data;
    diff_node->data_type = orig_node->data_type;

    if (orig_node->left_son) {

        int ret = node_add_left_son(diff_node);
        if (ret == -1)
            return -1;

        ret = diff_copy_branch(orig_node->left_son, diff_node->left_son);
        if (ret == -1)
            return -1;
    }

    if (orig_node->right_son) {

        int ret = node_add_right_son(diff_node);
        if (ret == -1)
            return -1;

        ret = diff_copy_branch(orig_node->right_son, diff_node->right_son);
        if (ret == -1)
            return -1;
    }

    return 0;
}

//===================================================================

int _diff_execute(struct Tree* tree, struct Tree* diff, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(tree);
    TREE_PTR_CHECK(diff);

    int ret = node_diff_execute(tree->root, diff->root);
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

int _node_diff_execute(struct Node* orig_node, struct Node* diff_node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig_node);
    NODE_PTR_CHECK(diff_node);

    switch(orig_node->data_type) {

        case CONSTANT: {

            return diff_constant_node(orig_node, diff_node);
        }

        case VARIABLE: {

            return diff_variable_node(orig_node, diff_node);
        }

        case OPERAND: {

            return diff_operand_node(orig_node, diff_node);
        }
        default: {

            error_report(DIFF_INV_DATA_TYPE);
            return -1;
        }
    }
}

//===================================================================

static int _diff_constant_node(struct Node* orig_node, struct Node* diff_node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig_node);
    NODE_PTR_CHECK(diff_node);

    int ret = node_init_constant(diff_node, 0);
    if (ret == -1)
        return -1;

    if (orig_node->parent == No_parent)
        diff_node->parent =  No_parent;

    return 0;
}

//===================================================================

static int _diff_variable_node(struct Node* orig_node, struct Node* diff_node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig_node);
    NODE_PTR_CHECK(diff_node);

    int ret = node_init_constant(diff_node, 1);
    if (ret == -1)
        return -1;

    if (orig_node->parent == No_parent)
        diff_node->parent =  No_parent;

    return 0;
}

//===================================================================

static int _diff_operand_node(struct Node* orig_node, struct Node* diff_node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig_node);
    NODE_PTR_CHECK(diff_node);

    char operand = orig_node->data.operand;

    switch(operand) {

        case ADD: [[fallthrough]];
        case SUB: 
            return diff_operand_add_or_sub(orig_node, diff_node, operand);

        case MUL: 
            return diff_operand_mul(orig_node, diff_node);

        case DIV:
            return diff_operand_div(orig_node, diff_node);

        case POW:
            return diff_operand_pow(orig_node, diff_node);

        default: {

            error_report(DIFF_INV_OPERAND);
            return -1;
        }
    }
}

//===================================================================

static int _diff_operand_add_or_sub(struct Node* orig_node, struct Node* diff_node, 
                                                             int oper, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig_node);
    NODE_PTR_CHECK(diff_node);

    if (orig_node->parent == No_parent)
        diff_node->parent =  No_parent;

    int ret = node_init_operand(diff_node, (char)oper);
    if (ret == -1)
        return -1;

    ret = node_add_sons(diff_node);
    if (ret == -1)
        return -1;

    ret = node_diff_execute(orig_node->left_son, diff_node->left_son);
    if (ret == -1)
        return -1;

    ret = node_diff_execute(orig_node->right_son, diff_node->right_son);
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

static int _diff_operand_mul(struct Node* orig_node, struct Node* diff_node, 
                                                                LOG_PARAMS) {
    diff_log_report();
    NODE_PTR_CHECK(orig_node);
    NODE_PTR_CHECK(diff_node);

    if (orig_node->parent == No_parent)
        diff_node->parent =  No_parent;

    int ret = node_init_operand(diff_node, ADD);
    if (ret == -1)
        return -1;
        
    ret = node_add_sons(diff_node);
    if (ret == -1)
        return -1;

    struct Node* left  = diff_node->left_son;
    struct Node* right = diff_node->right_son;

    ret = node_init_operand(left, MUL);
    if (ret == -1)
        return -1;

    ret = node_init_operand(right, MUL);
    if (ret == -1)
        return -1;

    ret = node_add_sons(right);
    if (ret == -1)
        return -1;

    ret = node_add_sons(left);
    if (ret == -1)
        return -1;

    ret = node_diff_execute(orig_node-> left_son, left-> left_son);
    if (ret == -1)
        return -1;

    ret = diff_copy_branch (orig_node->right_son, left->right_son);
    if (ret == -1)
        return -1;

    ret = diff_copy_branch (orig_node-> left_son, right-> left_son);
    if (ret == -1)
        return -1;

    ret = node_diff_execute(orig_node->right_son, right->right_son);
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

static int _diff_operand_div(struct Node* orig_node, struct Node* diff_node, 
                                                                LOG_PARAMS) {
    diff_log_report();
    NODE_PTR_CHECK(orig_node);
    NODE_PTR_CHECK(diff_node);

    if (orig_node->parent == No_parent)
        diff_node->parent =  No_parent;

    int ret = node_init_operand(diff_node, DIV);
    if (ret == -1)
        return -1;

    ret = node_add_sons(diff_node);
    if (ret == -1)
        return -1;

    struct Node* numerator   = diff_node-> left_son;
    struct Node* denominator = diff_node->right_son;

    ret = node_init_operand(numerator, SUB);
    if (ret == -1)
        return -1;

    ret = node_init_operand(denominator, POW);
    if (ret == -1)
        return -1;

    ret = node_add_sons(denominator);
    if (ret == -1)
        return -1;

    ret = diff_copy_branch(orig_node->right_son, denominator->left_son);
    if (ret == -1)
        return -1;

    ret = node_init_constant(denominator->right_son, 2);
    if (ret == -1)
        return -1;

    ret = node_add_sons(numerator);
    if (ret == -1)
        return -1;

    struct Node* numer_first  = numerator-> left_son;
    struct Node* numer_second = numerator->right_son;

    ret = node_init_operand(numer_first, MUL);
    if (ret == -1)
        return -1;

    ret = node_init_operand(numer_second, MUL);
    if (ret == -1)
        return -1;

    ret = node_add_sons(numer_first);
    if (ret == -1)
        return -1;

    ret = node_add_sons(numer_second);
    if (ret == -1)
        return -1;

    ret = node_diff_execute(orig_node-> left_son, numer_first-> left_son);
    if (ret == -1)
        return -1;

    ret = diff_copy_branch (orig_node->right_son, numer_first->right_son);
    if (ret == -1)
        return -1;

    ret = diff_copy_branch( orig_node-> left_son, numer_second-> left_son);
    if (ret == -1)
        return -1;

    ret = node_diff_execute(orig_node->right_son, numer_second->right_son);
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

static int _diff_operand_pow(struct Node* orig_node, struct Node* diff_node, 
                                                                LOG_PARAMS) {
    diff_log_report();
    NODE_PTR_CHECK(orig_node);
    NODE_PTR_CHECK(diff_node);

    struct Node* base  = orig_node-> left_son;
    struct Node* power = orig_node->right_son;
    double power_value = power->data.constant;

    if (base->data_type == CONSTANT && power->data_type == CONSTANT) {

        return node_init_constant(diff_node, 0);
    }

    int ret = node_init_operand(diff_node, MUL);
    if (ret == -1)
        return -1;

    ret = node_add_sons(diff_node);
    if (ret == -1)
        return -1;

    struct Node* base_diff = diff_node-> left_son;
    struct Node* diff      = diff_node->right_son;

    ret = node_diff_execute(base, base_diff);
    if (ret == -1)
        return -1;

    ret = node_init_operand(diff, MUL);
    if (ret == -1)
        return -1;

    ret = node_add_sons(diff);
    if (ret == -1)
        return -1;

    ret = node_init_constant(diff->left_son, power_value);
    if (ret == -1)
        return -1;

    ret = node_init_operand(diff->right_son, POW);
    if (ret == -1)
        return -1;

    ret = node_add_sons(diff->right_son);
    if (ret == -1)
        return -1;

    ret = diff_copy_branch(base, diff->right_son->left_son);
    if (ret == -1)
        return -1;

    ret = node_init_constant(diff->right_son->right_son, power_value - 1);
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

int _diff_out_to_file(struct Tree* diff, const char* filename, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(diff);

    return tree_save_to_file(diff, filename);
}

//===================================================================

int _diff_out_to_console(struct Tree* diff, LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(diff);

    return node_save_to_file(diff->root, stdout);
}

