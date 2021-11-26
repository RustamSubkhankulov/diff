#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "diff.h"
#include "diff_tex.h"
#include "diff_variables.h"
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

static char _get_var_diff_by(LOG_PARAMS);

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

            if (symb_is_var_name(symb)) {

                int ret = node_init_variable(node, symb);
                if (ret == -1)
                    return -1;

                if (check_new_var(symb))
                    add_new_var(symb);

                buffer_struct->pos += offset;
            }

            else if (symb_is_operand((int)symb)) {

                int ret = node_init_operand(node, (int)symb);
                if (ret == -1)
                    return -1;

                buffer_struct->pos += offset;
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

    return 0;
}

//===================================================================

static int _read_node_with_children(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                                LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

    node->data_type = OPERAND;

    int ret = node_add_sons(node);
    if (ret == -1)
        return -1;

    ret = node_read_from_buffer(node->left_son, buffer_struct);
    if (ret == -1)
        return -1;

    ret = read_node_data(node, buffer_struct);
    if (ret == -1)
        return -1;

    ret = node_read_from_buffer(node->right_son, buffer_struct);
    if (ret == -1)
        return -1;

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

    return 0;
}

//===================================================================

int _node_read_from_buffer(struct Node* node, struct Buffer_struct* buffer_struct, 
                                                                       LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(node);
    BUFFER_STRUCT_PTR_CHECK(buffer_struct);

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

static char _get_var_diff_by(LOG_PARAMS) {

    diff_log_report();

    printf("\n\n Choose a variable: \n\n");

    print_vars(stdout);
    putchar('\n');

    char var = 0;
    int scanned = scanf(" %c", &var);

    while (scanned != 1 && !check_new_var(var)) {

        if (scanned == -1) {

            error_report(INPUT_ERR);
            return -1;
        }

        printf("\n\n Please, try again.");
        scanned = scanf(" %c", &var);
    }

    return var;
}

//===================================================================

int _diff_execute(struct Tree* tree, struct Tree* diff, const char* tex_name, 
                                                                  LOG_PARAMS) {

    diff_log_report();
    TREE_PTR_CHECK(tree);
    TREE_PTR_CHECK(diff);

    int ret = 0;

    do 
    {
        ret = tree_simplify(tree);
        if (ret == -1)
            return -1;

    } while(0);

    char var = get_var_diff_by();

    ret = node_diff_execute(tree->root, 
                            diff->root, var);
    if (ret == -1)
        return -1;

    tree_draw_graph(diff);

    #ifdef DIFF_LATEX

        FILE* tex = tree_latex_execute(diff, tex_name);
        if (!tex)
            return -1;

    #endif

    do 
    {
        ret = tree_simplify(diff);
        if (ret == -1)
            return -1;

        #ifdef DIFF_LATEX

        if (ret)
            tree_latex_add_conspect(tree, tex);

        #endif

    } while(ret == 1);

    #ifdef DIFF_LATEX

        tree_latex_finish(tree);

    #endif

    return 0;
}

//===================================================================

int _node_diff_execute(struct Node* orig, struct Node* diff, char var, 
                                                           bbLOG_PARAMS) {

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
            
            if (node->data.variable == var)
                return node_init_constant(diff, 1);
            else
                return node_init_constant(diff, 0);
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
    diff = diff->R;

    NODE_INIT_OPERAND(diff, MUL);
    ADD_CONSTANT(diff, LEFT,  power_value);
    ADD_OPERAND (diff, RIGHT, POW);

    ADD_LEFT_AND_RIGHT(diff->R);

    NODE_COPY(orig->L, diff->R->L);
    NODE_INIT_CONSTANT(diff->R->R, power_value - 1);

    return 0;
}

//===================================================================

static int _diff_operand_sin(struct Node* orig, struct Node* diff, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    ADD_INSIDE_FUNCTION_DIFF(orig, diff);

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

    ADD_INSIDE_FUNCTION_DIFF(orig, diff);

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

    ADD_INSIDE_FUNCTION_DIFF(orig, diff);

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

    ADD_INSIDE_FUNCTION_DIFF(orig, diff);

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

    ADD_INSIDE_FUNCTION_DIFF(orig, diff);

    NODE_INIT_OPERAND(diff, DIV);

    int sign = 0;
    if (oper == ASIN)
        sign = 1;
    else 
        sign = -1;

    ADD_CONSTANT(diff, LEFT, sign);
    ADD_OPERAND (diff, RIGHT, POW);

    ADD_OPERAND (diff->R, LEFT,  SUB);
    ADD_CONSTANT(diff->R, RIGHT, 0.5);

    ADD_CONSTANT(diff->R->L, LEFT,  1);
    ADD_OPERAND (diff->R->L, RIGHT, POW);

    ADD_LEFT_AND_RIGHT(diff->R->L->R);
    NODE_INIT_CONSTANT(diff->R->L->R->R, 2);
    NODE_COPY(orig->L, diff->R->L->R->L);

    return 0;
}

//===================================================================

static int _diff_operand_arctg_and_arcctg(struct Node* orig, struct Node* diff, 
                                                                   int oper, LOG_PARAMS) {

    diff_log_report();
    NODE_PTR_CHECK(orig);
    NODE_PTR_CHECK(diff);

    ADD_INSIDE_FUNCTION_DIFF(orig, diff);

    NODE_INIT_OPERAND(diff, DIV);

    int sign = 0;
    if (oper == ATG)
        sign = 1;
    else
        sign = -1;

    ADD_CONSTANT(diff, LEFT,  sign);
    ADD_OPERAND (diff, RIGHT, ADD);

    ADD_CONSTANT(diff->R, LEFT,  1);
    ADD_OPERAND (diff->R, RIGHT, POW);

    ADD_LEFT_AND_RIGHT(diff->R->R);
    NODE_INIT_CONSTANT(diff->R->R->R, 2);
    NODE_COPY(orig->L, diff->R->R->L);

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

