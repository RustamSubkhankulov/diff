#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "../general/general.h"

//===================================================================

static int Graph_counter = 0;

static int Node_counter = 0;

//===================================================================

static const char* Var_color   = "#a5b8ef";

static const char* Const_color = "#e9a0a4";

static const char* Oper_color  = "#d2c1e6";

//===================================================================

int _print_node_data_type(struct Node* node, FILE* output, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (output == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    switch(node->data_type) {

        case constant: {

            fprintf(output, "constant");
            break;
        }

        case variable: {

            fprintf(output, "variable");
            break;
        }

        case operand: {

            fprintf(output, "operand");
            break;
        }

        default: {

            error_report(DIFF_INV_DATA_TYPE);
            break;
        }
    }

    return 0;
}

//===================================================================

int _print_node_data(struct Node* node, FILE* output, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (output == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    switch(node->data_type) {

        case constant: {

            fprintf(output, "%lf", node->data.constant);
            break;
        }

        case variable: {

            fprintf(output, "%c", node->data.variable);
            break;
        }

        case operand: {

            fprintf(output, "%c", node->data.operand);
            break;
        }

        default: {

            error_report(DIFF_INV_DATA_TYPE);
            break;
        }
    }

    return 0;
}

//===================================================================

int _tree_dump(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    fprintf(logs_file, "<div style = \"background-color: lightgrey; font-size: 18\"><pre>");

    fprintf(logs_file, "\n\n\n<p style = \" text-align: center; \"><b> Tree structure </b></p>\n");

    fprintf(logs_file, "\n Address: <%p>\n", tree);

    fprintf(logs_file, "\n ROOT ELEMENT: <%p> Data: <", tree->root);
    print_node_data(tree->root, logs_file);

    fprintf(logs_file, "> Data type: <");
    print_node_data_type(tree->root, logs_file);
    fprintf(logs_file, ">\n");

    fprintf(logs_file, "\n</pre></div>");   

    int ret = tree_print(tree);
    if (ret == -1)
        return -1;

    #ifdef TREE_GRAPHVIZ

        ret = tree_draw_graph(tree);
        if (ret == -1)
            return -1;

    #endif

    return 0; 
}

//===================================================================

int _tree_save_to_file(struct Tree* tree, const char* filename, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    FILE* tree_output = open_file(filename, "wb");
    if (tree_output == NULL)
        return 0;

    int ret = node_save_to_file(tree->root, tree_output);
    if (ret == -1)
        return -1;

    fprintf(tree_output, "\n");

    return close_file(tree_output);
}

//===================================================================

int _node_save_to_file(struct Node* node, FILE* output, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->left_son == NULL && node->right_son == NULL) {

        fprintf(output, " ( ");
        print_node_data(node, output);
        fprintf(output, " ) ");
    }

    else {

        fprintf(output, " ( ");

        if (node->left_son) {

            int ret = node_save_to_file(node->right_son, output);
            if (ret == -1)
                return -1;
        }

        print_node_data(node, output);

        if (node->right_son) {

            int ret = node_save_to_file(node->left_son, output);
            if (ret == -1)
                return -1;
        }

        fprintf(output, " ) ");
    }

    return 0;
}

//===================================================================

int _tree_print(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    return node_visiter(tree->root, _node_print);

    return 0;
}

//===================================================================

int _node_dot_fill(struct Node_dot* node_dot, FILE* graph_output, 
                                          int father, LOG_PARAMS) {

    tree_log_report();
    NODE_DOT_PTR_CHECK(node_dot);

    node_dot->graph   = graph_output;
    node_dot->father  = father;

    return 0;
}

//===================================================================

int _tree_draw_graph(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    FILE* graph = fopen(TEMP_DIR "diff_tree_graph.txt", "wb");

    if (graph == NULL) {

        error_report(FOPEN_ERROR);
        return -1;
    }

    fprintf(graph, "digraph G{\n");
    fprintf(graph, "rankdir=TB;\n");
    fprintf(graph, "node[ shape = doubleoctagon; style = filled ];\n");
    fprintf(graph, "edge[ arrowhead = vee ];\n");

    struct Node_dot node_dot = { 0 };
    node_dot_fill(&node_dot, graph, 0);

    node_draw_graph(tree->root, &node_dot);

    fprintf(graph, "\n}\n");
    fclose(graph);

    int ret = graph_call_dot();
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

int _graph_call_dot(LOG_PARAMS) {

    tree_log_report();

    char command_buf[System_cmnd_buf_size] = { 0 };
    sprintf(command_buf, "dot " TEMP_DIR "diff_tree_graph.txt -Tpng"
                         " -o " TEMP_DIR "diff_images/diff_tree_graph%d.png",
                                                              Graph_counter);

    int ret = system(command_buf);
    if (ret != 0)
        return -1;

    fprintf(logs_file, "\n <img width = 100%% src =" TEMP_DIR "diff_images/diff_tree_graph%d.png"
                                                         " alt = \"Tree graph has not found\">\n",
                                                                                   Graph_counter);

    Graph_counter++;

    return 0;
}

//===================================================================

int _node_draw_graph(struct Node* node, struct Node_dot* node_dot, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    int node_number = Node_counter;
    Node_counter++;

    if (node_dot->father != 0) {

        fprintf(node_dot->graph, "\n NODE%d -> NODE%d;\n", node_dot->father, 
                                                               node_number);
    }

    const char* node_color = 0;

    switch (node->data_type) {

        case constant: {

            node_color = Const_color;
            break;
        }

        case variable: {

            node_color = Var_color;
            break;
        }

        case operand: {

            node_color = Oper_color;
            break;
        }

        default: {

            error_report(DIFF_INV_DATA_TYPE);
            return -1;
        }
    }

    fprintf(node_dot->graph, "\n NODE%d [label = \"", node_number);
    print_node_data(node, node_dot->graph);
    fprintf(node_dot->graph, "\" color = \"%s\" ]; \n", node_color);

    if (node->left_son != NULL) {

        node_dot_fill(node_dot, node_dot->graph, node_number);
        
        int ret = node_draw_graph(node->left_son, node_dot);
        if (ret == -1)
            return -1;
    }

    if (node->right_son != NULL) {

        node_dot_fill(node_dot, node_dot->graph, node_number);
        
        int ret = node_draw_graph(node->right_son, node_dot);
        if (ret == -1)
            return -1;
    }

    return 0;
}

//===================================================================

int _tree_clear_check(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    return is_memory_clear(tree, 1, sizeof(tree));
}

//===================================================================

int _tree_cleaning(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    return clear_memory(tree, 1, sizeof(struct Tree));
}

//===================================================================

int _tree_poisoning(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    unsigned char* base = (unsigned char*)tree;

    for (int counter = 0; 
             counter < (int)sizeof(struct Tree); 
             counter++) {

        base[counter] = Poison_value;
    }

    return 0;
}

//===================================================================

int _tree_poison_check(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    unsigned char* base = (unsigned char*)tree;

    for (int counter = 0; 
             counter < (int)sizeof(struct Tree); 
             counter++) {

        if (base[counter] != Poison_value)
            return 0;
    }

    return 1;
}

//===================================================================

int _node_add_right_son(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->right_son != NULL) {

        error_report(ADD_SON_ERROR);
        return -1;
    }

    int is_ok = node_validator(node);
    if (is_ok == -1)  
        return -1;

    void* node_ptr = node_allocate_memory();

    if (node_ptr == NULL)
        return -1;

    node->right_son = (Node*)node_ptr;

    return 0;
}

//===================================================================

int _node_add_left_son(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->left_son != NULL) {

        error_report(ADD_SON_ERROR);
        return -1;
    }

    void* node_ptr = node_allocate_memory();

    if (node_ptr == NULL)
        return -1;

    node->left_son = (Node*)node_ptr;

    return 0;
}

//===================================================================

int _node_add_sons(struct Node* node, LOG_PARAMS) {

    tree_log_report();

    int ret = node_add_left_son(node);
    if (ret == -1)
        return -1;

    ret = node_add_right_son(node);
    if (ret == -1) 
        return -1;

    return 0;
}

//===================================================================

int _node_init_constant(struct Node* node, double value, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    node->data_type = constant;
    node->data.constant = value;

    return 0;
}

//===================================================================

int _node_init_variable(struct Node* node, char var, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    node->data_type = variable;
    node->data.variable = var;

    return 0;
}

//===================================================================

int _node_init_operand(struct Node* node, char oper, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    node->data_type = operand;
    node->data.operand = oper;

    return 0;
}

//===================================================================

// int _node_init(struct Node* node, elem_t value, LOG_PARAMS) {

//     tree_log_report();
//     NODE_PTR_CHECK(node);

//     node->data = value;

//     #ifdef TREE_CALCULATE_HASH_FROM_DATA

//         #ifdef HASH_POINTER_TO_CHAR

//             node->hash = get_hash((void*)value, strlen(value));

//         #else

//             node->hash = get_hash((void*)node->data, sizeof(elem_t));

//         #endif

//     #endif

//     return 0;
// }

//===================================================================

int _tree_delete_branch(struct Node** node_ptr, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node_ptr);

    int ret = node_delete_branch(*node_ptr);
    if (ret == -1)
        return -1;

    *node_ptr = NULL;

    return 0;
}

//===================================================================

int _node_delete_branch(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->left_son != NULL) {

        int ret = node_delete_branch(node->left_son);
        if (ret ==  -1)
            return -1;
    }

    if (node->right_son != NULL) {

        int ret = node_delete_branch(node->right_son);
        if (ret == -1)
            return -1;        
    }

    if (node == NULL) {

        error_report(INV_NODE_PTR);
        return -1;
    }

    free(node);

    return 0;
}

//===================================================================

void* _node_allocate_memory(LOG_PARAMS) {

    tree_log_report();

    void* memory = calloc(1, sizeof(struct Node));

    if (memory == NULL) 
        error_report(CANNOT_ALLOCATE_MEM);

    return memory;
}

//===================================================================

int _tree_ctor(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    int is_poison = tree_poison_check(tree);
    int is_clear  = tree_clear_check(tree);

    if (is_poison == 0 && is_clear == 0) {

        error_report(TREE_STRUCT_CTOR_UNPREPARED);
        return -1;
    }

    if (is_poison)
        tree_cleaning(tree);

    tree->root = (struct Node*)node_allocate_memory();
    if (tree->root == NULL)
        return -1;

    TREE_VERIFICATION(tree);

    return 0;
}

//===================================================================

int _tree_dtor(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    TREE_VERIFICATION(tree);

    int ret = tree_free_memory(tree);
    if (ret == -1)
        return -1;

    tree_poisoning(tree);

    return 0;
}

//===================================================================

int _tree_free_memory(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    return node_visiter(tree->root, _node_destruct);
}

//===================================================================

int _node_visiter(struct Node* node, int (*node_function)(struct Node* node, LOG_PARAMS), 
                                                                             LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->left_son != NULL) {
        
        if (node_visiter(node->left_son, node_function) == -1)
            return -1;
    }

    if (node->right_son != NULL) {
        
        if (node_visiter(node->right_son, node_function) == -1)
            return -1;
    }

    return node_function(node, LOG_ARGS);
}

//===================================================================

int _tree_validator(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    if (node_visiter(tree->root, _node_validator) == -1 )
        return -1;

    else  {

        #ifdef TREE_DEBUG

            tree_dump(tree);

        #endif

        return 0;
    }
}

//===================================================================

int symb_is_var_name(char symb) {

    return (symb < 'a' || symb > 'z')? 0: 1;
}

//===================================================================

int symb_is_operand(char symb) {

    for (int counter = 0; 
             counter < Operands_number; 
             counter++) {

        if (symb == Operands[counter])
            return 1;
    }

    return 0;
}

//===================================================================

int _node_validator(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->data_type == operand 
    && (node->left_son == NULL && node->right_son == NULL)) {

        error_report(OPER_NO_SONS);
        return -1;
    }

    if (node->data_type == variable && symb_is_var_name(node->data.variable)) {

        error_report(INV_VAR_NAME);
        return -1;
    }


    return 0; 
}

//===================================================================

int _node_print(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    fprintf(logs_file, "<div style = \"background-color: lightgrey; font-size: 18\"><pre>");

    fprintf(logs_file, "\n<b>Node: <%p> </b> Data: <", node);
    print_node_data(node, logs_file);
    fprintf(logs_file, "> Data type: <");
    print_node_data_type(node, logs_file); 
    fprintf(logs_file, ">\n ""Left son: <%p> Right son: <%p> \n",
                                                  node->left_son, 
                                                 node->right_son);
    fprintf(logs_file, "</pre></div>");

    return 0;
}

//===================================================================

int _node_destruct(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    free(node);

    return 0;
}