#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "diff.h"
#include "../text_processing/text_processing.h"
#include "../general/general.h"

//===================================================================

static int _read_closing_bracket(struct Buffer_struct* buffer_struct, LOG_PARAMS);

static int _read_opening_bracket(struct Buffer_struct* buffer_struct, LOG_PARAMS);

static int  _diff_operand_node(struct Node* orig, struct Node* diff, LOG_PARAMS);

static int _diff_operand_add_or_sub(struct Node* orig, struct Node* diff, 
                                                   int oper, LOG_PARAMS);

static int _diff_operand_mul(struct Node* orig, struct Node* diff, LOG_PARAMS);

static int _diff_operand_div(struct Node* orig, struct Node* diff, LOG_PARAMS);

static int _diff_operand_pow(struct Node* orig, struct Node* diff, LOG_PARAMS);

static int _diff_operand_sin(struct Node* orig, struct Node* diff, LOG_PARAMS);

static int _diff_operand_cos(struct Node* orig, struct Node* diff, LOG_PARAMS);

static int _diff_operand_tg(struct Node* orig, struct Node* diff, LOG_PARAMS);

static int _diff_operand_ctg(struct Node* orig, struct Node* diff, LOG_PARAMS);

static int _diff_operand_arcsin_and_arccos(struct Node* orig, struct Node* diff, 
                                                          int oper, LOG_PARAMS);

static int _diff_operand_arctg_and_arcctg(struct Node* orig, struct Node* diff, 
                                                         int oper, LOG_PARAMS);

static int _tree_simplify(struct Tree* diff, LOG_PARAMS);

static int _constant_folding(struct Node* node, LOG_PARAMS);

static int _cut_nodes(struct Tree* diff, struct Node** node, LOG_PARAMS);

static int _calc_operand_value(struct Node* node, LOG_PARAMS);

static int _calc_function_value(struct Node* node, LOG_PARAMS);

static Node* _is_zero_node(struct Node* node, LOG_PARAMS);

static Node* _is_one_node(struct Node* node, LOG_PARAMS);

static int _replace_with_const(struct Node* node, double constant, LOG_PARAMS);

static int _cut_constant(struct Tree* diff, struct Node** node, struct Node* cutted, 
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

    int is_ok = tree_validator(tree);
    if (is_ok == -1)
        return NULL;
    
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

static int _read_function(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                       LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    int prev_pos = buffer_struct->pos;

    // int opening = read_opening_bracket(buffer_struct);
    // if (opening == -1)
    //     return -1;

    char buffer[Function_name_buf_size] = { 0 };
    int offset = 0;

    int scanned = sscanf(buffer_struct->buffer + buffer_struct->pos, " %s %n", 
                                                                buffer, &offset);
    if (!scanned) {

        error_report(TEXT_PROCESSING_ERR);
        return -1;
    }

    int64_t hash = get_hash(buffer, strlen(buffer));

    for (int counter = 0; counter < Functions_number; counter++) {

        if (hash == Functions[counter].hash) {

            int ret = node_init_operand(node, Functions[counter].code);
            if (ret == -1)
                return -1;

            buffer_struct->pos += offset;

            ret = node_add_left_son(node);
            if (ret == -1)
                return -1;

            ret = node_read_from_buffer(node->left_son, buffer_struct);
            if (ret == -1)
                return -1;

            ret =  read_closing_bracket(buffer_struct);
            if (ret == -1)
                return -1;
            
            return 1;
        }
    }

    buffer_struct->pos = prev_pos;
    return 0;
}

//===================================================================

static int _read_constant(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                      LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);
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

    diff_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

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

            // int ret = read_function(node, buffer_struct);
            // if (ret == -1)
            //     return -1;

            // if (ret == 1)
            //     break;

            if (symb_is_var_name(symb)) {

                int ret = node_init_variable(node, symb);
                if (ret == -1)
                    return -1;

                buffer_struct->pos += offset;
                //
                buffer_dump(buffer_struct);
                //
            }

            else if (symb_is_operand((int)symb)) {

                int ret = node_init_operand(node, (int)symb);
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

    diff_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    node->data_type = OPERAND;

    // int ret = read_function(node, buffer_struct);
    // if (ret == -1)
    //     return -1;
    // if (ret == 1)
    //     return 0;

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

    diff_log_report();
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

    diff_log_report();
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

    diff_log_report();
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

    int scan_func = read_function(node, buffer_struct);
    if (scan_func == -1)
        return -1;
    if (scan_func == 1)
        return 1;

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

    int is_ok = tree_validator(tree);
    if (is_ok == -1)
        return NULL;

    return buffer;   
}

//===================================================================

int _diff_copy_branch(struct Node* orig, struct Node* diff, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    diff->data      = orig->data;
    diff->data_type = orig->data_type;

    if (orig->left_son) {

        int ret = node_add_left_son(diff);
        if (ret == -1)
            return -1;

        ret = diff_copy_branch(orig->left_son, diff->left_son);
        if (ret == -1)
            return -1;
    }

    if (orig->right_son) {

        int ret = node_add_right_son(diff);
        if (ret == -1)
            return -1;

        ret = diff_copy_branch(orig->right_son, diff->right_son);
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

    int ret = 0;

    do 
    {
        ret = tree_simplify(diff);
        if (ret == -1)
            return -1;

    } while(0);

    ret = node_diff_execute(tree->root, diff->root);
    if (ret == -1)
        return -1;

    tree_draw_graph(diff);

    do 
    {
        ret = tree_simplify(diff);
        if (ret == -1)
            return -1;

    } while(ret == 1);

    return 0;
}

//===================================================================

static int _tree_simplify(struct Tree* diff, LOG_PARAMS) {

    diff_log_report();              //изменить на tree
    TREE_PTR_CHECK(diff);

    int constant_simp = constant_folding(diff->root);
    if (constant_simp == -1)
        return -1;

    int cut_nodes = cut_nodes(diff, &(diff->root));
    if (cut_nodes == -1)
        return -1;

    tree_draw_graph(diff);

    return (constant_simp || cut_nodes );
}

//===================================================================

static int _calc_operand_value(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    double first_value  = node->left_son ->data.constant;
    double second_value = node->right_son->data.constant;

    double result = 0;

    switch (node->data.operand) {

        case ADD: {

            result = first_value + second_value;
            break;
        }
        
        case SUB: {

            result = first_value - second_value;
            break;
        }

        case MUL: {

            result = first_value * second_value;
            break;
        }

        case DIV: {

            if (double_is_equal(0, second_value)) {

                error_report(DIV_BY_ZERO);
                return -1;
            }

            result = first_value / second_value;
            break;
        }

        case POW: {

            result = pow(first_value, second_value);
            break;
        }

        default: {

            error_report(DIFF_INV_OPERAND);
            return -1;
        }
    }

    int ret = node_init_constant(node, result);
    if (ret == -1)
        return -1;

    ret = node_visiter(node->left_son,  _node_destruct);
    if (ret == -1)
        return -1;

    ret = node_visiter(node->right_son, _node_destruct);
    if (ret == -1)
        return -1;

    node->left_son  = NULL;
    node->right_son = NULL;

    return 1;
}

//===================================================================

static int _calc_function_value(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    double arg = node->left_son->data.constant;
    double result = 0;

    switch (node->data.operand) {

        case SIN: {

            result = sin(arg);
            break;
        }

        case COS: {

            result = cos(arg);
            break;
        }

        case TG: {

            result = tan(arg);
            break;
        }

        case CTG: {

            result = 1 / tan(arg);
            break;
        }

        case ACOS: {

            result = acos(arg);
            break;
        }

        case ASIN: {

            result = asin(arg);
            break;
        }

        case ATG: {

            result = atan(arg); 
            break;
        }

        case ACTG: {

            result = 1 / atan(arg);
            break;
        }

        default: {

            error_report(DIFF_INV_OPERAND);
            return -1;
        }
    }

    int ret = node_init_constant(node, result);
    if (ret == -1)
        return -1;

    ret = node_visiter(node->left_son, _node_destruct);
    if (ret == -1)
        return -1;

    ret = node_visiter(node->right_son, _node_destruct);
    if (ret == -1)
        return -1;

    node->left_son  = NULL;
    node->right_son = NULL;

    return 1;
}

//===================================================================

static int _constant_folding(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    int ret = 0;
    int is_simplified = 0;

    if (node->left_son) {

        ret = constant_folding(node->left_son);
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (node->right_son) {

        ret = constant_folding(node->right_son);
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (node->data_type == OPERAND 
     && !is_function_operand(node)
     && node->left_son ->data_type == CONSTANT 
     && node->right_son->data_type == CONSTANT) {

        ret = calc_operand_value(node);
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (node->data_type == OPERAND
    && is_function_operand(node)
    && node->left_son != NULL
    && node->left_son->data_type == CONSTANT) {

        ret = calc_function_value(node);
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    return is_simplified;
}

//===================================================================

static Node* _is_zero_node(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    if (!node)
        return NULL;

    if (node->left_son->data_type == CONSTANT 
     && double_is_equal(node->left_son->data.constant, 0))
        return node->left_son;
    
    if (node->right_son->data_type == CONSTANT 
     && double_is_equal(node->right_son->data.constant, 0))
        return node->right_son;

    return NULL;
}

//===================================================================

static Node* _is_one_node(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    if (!node)
        return NULL;

    if (node->left_son->data_type == CONSTANT 
     && double_is_equal(node->left_son->data.constant, 1))
        return node->left_son;
    
    if (node->right_son->data_type == CONSTANT 
     && double_is_equal(node->right_son->data.constant, 1))
        return node->right_son;

    return NULL;
}

//===================================================================

static int _replace_with_const(struct Node* node, double constant, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    int ret = node_visiter(node->left_son, _node_destruct);
    if (ret == -1)
        return -1;
    
    ret = node_visiter(node->right_son, _node_destruct);
    if (ret == -1)
        return -1;

    ret = node_init_constant(node, constant);
    if (ret == -1)
        return -1;

    node->left_son  = NULL;
    node->right_son = NULL;

    return 1;
}

//===================================================================

static int _cut_constant(struct Tree* diff, struct Node** node_ptr, struct Node* cutted, 
                                                                             LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node_ptr);
    TREE_PTR_CHECK(diff);

    struct Node* node = *node_ptr;

    struct Node* expression = NULL;

    if (node->left_son != cutted)
        expression = node->left_son;
    else 
        expression = node->right_son;

    *node_ptr = expression;
    (*node_ptr)->parent = node->parent;

    if ((*node_ptr)->parent == No_parent)
        diff->root = expression;

    else if (node == node->parent->left_son)
        node->parent->left_son  = expression;
        
    else if (node == node->parent->right_son)
        node->parent->right_son = expression;

    int ret = node_destruct(cutted);
    if (ret == -1)
        return -1;

    ret = node_destruct(node);
    if (ret == -1)
        return -1;

    tree_draw_graph(diff);

    return 1;
}

//==================================================================

static int _mul_by_zero_simp_check(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node)

    if (node->data_type    == OPERAND 
    &&  node->data.operand == MUL 
    &&  is_zero_node(node))

        return 1;
    else 
        return 0;
}

static Node* _mul_by_one_simp_check(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    if (!node)
        return NULL;

    struct Node* returning = NULL;

    if  (node->data_type    == OPERAND 
    &&   node->data.operand == MUL 
    &&  (returning = is_one_node(node)))

        return returning;
    else 
        return NULL;
}

static Node* _sum_with_zero_simp_check(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    if (!node)
        return NULL;

    struct Node* returning = NULL;

    if (node->data_type    == OPERAND
    && (node->data.operand == ADD
    ||  node->data.operand == SUB)
    && (returning = is_zero_node(node)))

        return returning;
    else
        return NULL;
}

static Node* _pow_one_simp_check(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    if (!node)
        return NULL;

    struct Node* returning = NULL;

    if (node->data_type    == OPERAND
    &&  node->data.operand == POW
    && (returning = is_one_node(node)))

        return returning;
    else
        return NULL;
}

static int _pow_zero_simp_check(struct Node* node, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);

    if (node->data_type    == OPERAND
    &&  node->data.operand == POW
    &&  is_zero_node(node))

        return 1;
    else
        return 0;
}

//===================================================================

static int _cut_nodes(struct Tree* diff, struct Node** node_ptr, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node_ptr);
    TREE_PTR_CHECK(diff);

    struct Node* node = *node_ptr;

    int is_simplified = 0;
    int ret = 0;
    struct Node* cutted = NULL;

    if (node->left_son) {

        ret = cut_nodes(diff, &(node->left_son));
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (node->right_son) {

        ret = cut_nodes(diff, &(node->right_son));
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (mul_by_zero_simp_check(node)) {

         ret = replace_with_const(node, 0);
         if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if ((cutted = mul_by_one_simp_check(node))) {

         ret = cut_constant(diff, &node, cutted);
         if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if ((cutted = sum_with_zero_simp_check(node))) {

         ret = cut_constant(diff, &node, cutted);
         if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    if (pow_zero_simp_check(node)) {

         ret = replace_with_const(node, 1);
         if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;

    }

    if ((cutted = pow_one_simp_check(node))) {

        ret = cut_constant(diff, &node, cutted);
        if (ret == -1)
            return -1;

        else if (ret)
            is_simplified++;
    }

    return is_simplified;
}

//===================================================================

int _node_diff_execute(struct Node* orig, struct Node* diff, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    if (orig->parent == No_parent)
        diff->parent =  No_parent;

    switch(orig->data_type) {

        case CONSTANT: {

            return node_init_constant(diff, 0);
        }

        case VARIABLE: {

            return node_init_constant(diff, 1);
        }

        case OPERAND: {

            return diff_operand_node(orig, diff);
        }
        default: {

            error_report(DIFF_INV_DATA_TYPE);
            return -1;
        }
    }
}

//===================================================================

static int _diff_operand_node(struct Node* orig, struct Node* diff, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    int operand = orig->data.operand;

    switch(operand) {

        case ADD: [[fallthrough]];
        case SUB: 
            return diff_operand_add_or_sub(orig, diff, operand);

        case MUL: 
            return diff_operand_mul(orig, diff);

        case DIV:
            return diff_operand_div(orig, diff);

        case POW:
            return diff_operand_pow(orig, diff);

        case SIN:
            return diff_operand_sin(orig, diff);

        case COS:
            return diff_operand_cos(orig, diff);

        case TG:
            return diff_operand_tg(orig, diff);

        case CTG:
            return diff_operand_ctg(orig, diff);

        case ASIN: [[fallthrough]];
        case ACOS:
            return diff_operand_arcsin_and_arccos(orig, diff, operand);

        case ATG: [[fallthrough]];
        case ACTG:
            return diff_operand_arctg_and_arcctg(orig, diff, operand);

        default: {

            error_report(DIFF_INV_OPERAND);
            return -1;
        }
    }
}

//===================================================================

#define L left_son

#define R right_son

//===================================================================

static int _diff_operand_add_or_sub(struct Node* orig, struct Node* diff, 
                                                             int oper, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);            //занести init и add в один макрос

    NODE_INIT_OPERAND(diff, (char)oper);

    ADD_LEFT_AND_RIGHT(diff);

    NODE_DIFF(orig->L, diff->L);
    NODE_DIFF(orig->R, diff->R);

    return 0;
}

//===================================================================

static int _diff_operand_mul(struct Node* orig, struct Node* diff, 
                                                                LOG_PARAMS) {
    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    NODE_INIT_OPERAND(diff, ADD);

    ADD_OPERAND(diff, LEFT,  MUL);
    ADD_OPERAND(diff, RIGHT, MUL);

    ADD_LEFT_AND_RIGHT(diff->L);
    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_DIFF(orig->L, diff->L->L);
    NODE_COPY(orig->R, diff->L->R);

    NODE_COPY(orig->L, diff->R->L);
    NODE_DIFF(orig->R, diff->R->R);

    return 0;
}

//===================================================================

static int _diff_operand_div(struct Node* orig, struct Node* diff, 
                                                                LOG_PARAMS) {
    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    NODE_INIT_OPERAND(diff, DIV);

    ADD_OPERAND(diff, LEFT,  SUB);
    ADD_OPERAND(diff, RIGHT, POW);

    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_COPY(orig->R, diff->R->L);
    NODE_INIT_CONSTANT(diff->R->R, 2);

    ADD_OPERAND(diff->L, LEFT,  MUL);
    ADD_OPERAND(diff->L, RIGHT, MUL);

    ADD_LEFT_AND_RIGHT(diff->L->L);
    ADD_LEFT_AND_RIGHT(diff->L->R);

    NODE_DIFF(orig->L, diff->L->L->L);
    NODE_COPY(orig->R, diff->L->L->R);

    NODE_COPY(orig->L, diff->L->R->L);
    NODE_DIFF(orig->R, diff->L->R->R);

    return 0;
}

//===================================================================

static int _diff_operand_pow(struct Node* orig, struct Node* diff, 
                                                                LOG_PARAMS) {
    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    if (orig->L->data_type == CONSTANT 
     && orig->R->data_type == CONSTANT)

        return node_init_constant(diff, 0);

    double power_value = orig->R->data.constant;

    NODE_INIT_OPERAND (diff, MUL);
    ADD_LEFT_AND_RIGHT(diff);

    NODE_DIFF(orig->L, diff->L);

    NODE_INIT_OPERAND(diff->R, MUL);
    ADD_CONSTANT(diff->R, LEFT,  power_value);
    ADD_OPERAND (diff->R, RIGHT, POW);

    ADD_LEFT_AND_RIGHT(diff->R->R);

    NODE_COPY(orig->L, diff->R->R->L);
    NODE_INIT_CONSTANT(diff->R->R->R, power_value - 1);

    return 0;
}

//===================================================================

static int _diff_operand_sin(struct Node* orig, struct Node* diff, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    NODE_INIT_OPERAND(diff, MUL);
    ADD_LEFT_AND_RIGHT(diff);

    NODE_DIFF(orig->L, diff->R);

    NODE_INIT_OPERAND(diff->L, COS);
    ADD_LEFT (diff->L);
    NODE_COPY(orig->L, diff->L->L);
   
    return 0;
}

//===================================================================

static int _diff_operand_cos(struct Node* orig, struct Node* diff, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    NODE_INIT_OPERAND(diff, MUL);
    ADD_CONSTANT(diff, LEFT, -1);
    ADD_OPERAND(diff, RIGHT, MUL);

    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_DIFF(orig->L, diff->R->L);

    NODE_INIT_OPERAND( diff->R->R, SIN);
    ADD_LEFT( diff->R->R);
    NODE_COPY(orig->L, diff->R->R->L);

    return 0;
} 

//===================================================================

static int _diff_operand_tg(struct Node* orig, struct Node* diff, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    NODE_INIT_OPERAND(diff, DIV);

    ADD_CONSTANT(diff, LEFT,  1);
    ADD_OPERAND (diff, RIGHT, POW);
    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_INIT_OPERAND(diff->R->L, COS);
    ADD_LEFT(diff->R->L);
    NODE_COPY(orig->L, diff->R->L->L);

    NODE_INIT_CONSTANT(diff->R->R, 2);

    return 0;
}

//===================================================================

static int _diff_operand_ctg(struct Node* orig, struct Node* diff, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    NODE_INIT_OPERAND(diff, DIV);

    ADD_CONSTANT(diff, LEFT,  -1);
    ADD_OPERAND (diff, RIGHT, POW);
    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_INIT_OPERAND(diff->R->L, SIN);
    ADD_LEFT(diff->R->L);
    NODE_COPY(orig->L, diff->R->L->L);

    NODE_INIT_CONSTANT(diff->R->R, 2);

    return 0;
}

//===================================================================

static int _diff_operand_arcsin_and_arccos(struct Node* orig, struct Node* diff, 
                                                                    int oper, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    oper++;

    return 0;
}

//===================================================================

static int _diff_operand_arctg_and_arcctg(struct Node* orig, struct Node* diff, 
                                                                   int oper, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    oper++;

    return 0;
}

//===================================================================

#undef L
#undef R

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

