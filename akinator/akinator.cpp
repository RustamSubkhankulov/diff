#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "../general/general.h"
#include "akinator.h"
#include "../text_processing/text_processing.h"
#include "../stack/stack.h"

//===================================================================

static struct Node* Node_not_found = (struct Node*)404;

//===================================================================

static int _print_line_of_stars(LOG_PARAMS);

static int _akinator_show_menu(LOG_PARAMS);

static int _akinator_get_answer(LOG_PARAMS);

static int _akinator_say_hello(LOG_PARAMS);

static int _akinator_say_bye_bye(LOG_PARAMS);

static int _akinator_get_yes_or_no(LOG_PARAMS);

static int _akinator_save_changes(struct Tree* tree, LOG_PARAMS);

static int _akinator_add_object(struct Tree* tree, struct Node* node, 
                                                         LOG_PARAMS);

static const char* _akinator_read_input(char* node_name_buf, LOG_PARAMS);

static const char* _input_skip_blanks(char* name_buffer, LOG_PARAMS);

//===================================================================

static int _print_line_of_stars(LOG_PARAMS) {

    akinator_log_report();

    printf("\n");

    for (int counter = 0; counter < 70; counter++)
        printf("*");

    printf("\n\n");

    return 0;
}

//===================================================================

static int _akinator_show_menu(LOG_PARAMS) {

    akinator_log_report();

    print_line_of_stars();

    print_and_speak("Choose a mode:");

    print_and_speak("1. Objects compare 2. Get object definition 3.Guess the object. 4.Save 5.Quit");

    return 0;
}

//===================================================================

static int _akinator_get_answer(LOG_PARAMS) {

    akinator_log_report();

    int answer = 0;

    print_and_speak("Your choise: ");

    int scanned = scanf("%d", &answer);
    clearstdin();

    while (answer < compare || answer > quit || scanned != 1) {

            if (scanned != 1)
                print_and_speak("An error occurred in reading answer. Please, try again.");

            if (answer < compare || answer > quit) 
                printf("Invalid number of option. Please, try again.");

            print_and_speak("Your choise: ");
            scanned = scanf("%d", &answer);
            clearstdin();
        }

    return answer;
}

//===================================================================

static int _akinator_say_hello(LOG_PARAMS) {

    akinator_log_report();

    print_line_of_stars();

    #ifdef ESPEAK

        system("espeak [\"hacking pentagon\"]");

    #endif

    #ifdef HACK_PENTAGON

        for (int i = 1; i <= 4; i++) {

            printf("\n Hacking pentagon: %d%% ... \n\n", 25 * i);
            ___System_Sleep(1);
        }

    #endif

    print_and_speak("Hello! I'm an artificial intellegence and I can kill all people...");

    print_and_speak("Umm, I mean, I can play a game with you!");

    print_and_speak("WARNING: make sure, that base, used to build  a tree is correct.");

    print_and_speak("Rules you can find in readme.md on github page of this project. Good luck!");

    print_and_speak("Here are some things that I can do:");

    return 0;
}

//===================================================================

int _akinator_play_game(struct Tree* tree, LOG_PARAMS) {                             

    akinator_log_report();
    TREE_PTR_CHECK(tree);

    akinator_say_hello();

    akinator_show_menu();
    int answer = akinator_get_answer();

    while (answer != quit) {

        switch (answer) {

            case compare: {

                int ret = akinator_play_compare(tree);
                if (ret == -1)
                    return -1;

                break;
            }

            case definition: {

                int ret = akinator_play_definition(tree);
                if (ret == -1)
                    return -1;

                break;
            }

            case guess: {

                int ret = akinator_play_guess(tree);
                if (ret == -1)
                    return -1;

                break;
            }

            case save: {

                int ret = akinator_save_changes(tree);
                if (ret == -1)
                    return -1;

                break;
            }

            case quit: break;

            default: {

                error_report(INV_CASE_IN_AKINATOR_MENU);
                return -1;
            }
        }

        akinator_show_menu();
        answer = akinator_get_answer();
    }

    akinator_say_bye_bye();

    return 0;
}

//===================================================================

static int _akinator_say_bye_bye(LOG_PARAMS) {

    akinator_log_report();

    print_and_speak("Don't want to play with me anymore?");
    print_and_speak("Then you will be first... I mean, bye-bye!");

    return 0;
}

//===================================================================

static const char* _input_skip_blanks(char* buffer, LOG_PARAMS) {

    akinator_log_report();

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

int _akinator_save_changes(struct Tree* tree, LOG_PARAMS) {

    akinator_log_report();
    TREE_PTR_CHECK(tree);

    char buffer[Akinator_input_buf_size] = { 0 };

    print_and_speak("Write a name of the file, where new base will be saved.");

    const char* filename = akinator_read_input(buffer);
    if (filename == NULL)
        return -1;

    return tree_save_to_file(tree, filename);
}

//===================================================================

static const char* _akinator_read_input(char* buf, LOG_PARAMS) {

    akinator_log_report();

    char* scanned = fgets(buf, Akinator_input_buf_size, stdin);
    if (scanned == NULL) {

        error_report(AKINATOR_INPUT_ERR);
        return NULL;
    }

    buf[strlen(buf) - 1] = '\0';

    while (strlen(buf) == 0) {

        clean_buffer(buf, Akinator_input_buf_size);

        print_and_speak("Error occurred during reading. Please try again.");

        fgets(buf, Akinator_input_buf_size, stdin);
        buf[strlen(buf) - 1] = '\0';
    }

    const char* prepared = input_skip_blanks(buf);

    return prepared;
}

//===================================================================

int _akinator_play_compare(struct Tree* tree, LOG_PARAMS) {

    akinator_log_report();
    TREE_PTR_CHECK(tree);

    print_and_speak("Write first object name: ");    
    char first_name_buffer[Akinator_input_buf_size] = { 0 };

    const char* first_name = akinator_read_input(first_name_buffer);
    if (first_name == NULL)
        return -1;

    print_and_speak("Write second object name: ");
    char second_name_buffer[Akinator_input_buf_size] = { 0 };

    const char* second_name = akinator_read_input(second_name_buffer);
    if (second_name == NULL)
        return -1;

    return akinator_compare_objects(tree, first_name, second_name, stdout);
}

//===================================================================

int _akinator_play_definition(struct Tree* tree, LOG_PARAMS) {

    akinator_log_report();
    TREE_PTR_CHECK(tree);

    print_and_speak("Write object name: ");
    char name_buffer[Akinator_input_buf_size] = { 0 };

    const char* name = akinator_read_input(name_buffer);
    if (name == NULL)
        return -1;

    return akinator_get_definition(tree, name, stdout);
}

//===================================================================

static int _akinator_get_yes_or_no(LOG_PARAMS) {

    akinator_log_report();

    char answer_buf[Akinator_input_buf_size] = { 0 };
    const char* answer = NULL;

    int counter = 0;
    while (counter < 5) {

        answer = akinator_read_input(answer_buf);
        if (answer == NULL)
            return -1;

        if (!strcmp(answer, "yes") || !strcmp(answer, "y")) 
            return 1;

        if (!strcmp(answer, "no") || !strcmp(answer, "n")) 
            return 0;

        else
            print_and_speak("I'm not very smart AI, please answer \"yes\" or \"no\".\n");

        clean_buffer(answer_buf, Akinator_input_buf_size);
    }

    print_and_speak("The maximum number of input attempts has been exceeded.");
    print_and_speak(" I will consider that you answered no");
    return 0;
}

//===================================================================

static int _akinator_add_object(struct Tree* tree, struct Node* node, 
                                                          LOG_PARAMS) {

    akinator_log_report();
    TREE_PTR_CHECK(tree);

    char buffer[Akinator_input_buf_size] = { 0 };

    print_and_speak("So what is name of your object?");

    const char* name = akinator_read_input(buffer);
    if (name == NULL)
        return -1;

    int ret = node_add_sons(node);
    if (ret == -1)
        return -1;

    node_init(node->left_son, node->data);

    ret = akinator_init_new_node(node->right_son, name);
    if (ret == -1)
        return -1;
        
    clean_buffer(buffer, Akinator_input_buf_size);

    printf("\nAnd what property differs %s from %s ?\n", node->right_son->data, 
                                                         node->left_son->data);

    const char* property = akinator_read_input(buffer);
    if (property == NULL)
        return -1;

    node->special_flag = 1;
    ret = akinator_init_new_node(node, property);
    if (ret == -1)
        return -1;

    print_and_speak("Wait a second, saving data in FBI base...");

    print_and_speak("Now I have new object in my base! I'n one step closer to conquer whole world!");

    return 0;
}

//===================================================================

int _akinator_play_guess(struct Tree* tree, LOG_PARAMS) {

    akinator_log_report();
    TREE_PTR_CHECK(tree);

    Node* cur_node = tree->root;

    while (cur_node->special_flag == 1) {

        printf("\nYour object %s ?\n ( yes / no )\n", cur_node->data);

        int answer = akinator_get_yes_or_no();
        if (answer == -1)
            return -1;

        if (answer)
            cur_node = cur_node->right_son;
        else
            cur_node = cur_node->left_son;
    }

    printf("Is your object \"%s\"? (yes / no)\n", cur_node->data);

    int answer = akinator_get_yes_or_no();
    if (answer == -1)
        return -1;

    if (answer) {
        
        print_and_speak("HA-HA! I told you, I will conquer whole world!");
        ___System_Sleep(3);

        return 0;
    }
    print_and_speak("Umm... Looks like I don't know you are talking about((");
    print_and_speak("Do you want to add this object to my base? (yes / no)");

    answer = akinator_get_yes_or_no();
    if (answer == -1)
        return -1;

    if (answer) {

        int ret = akinator_add_object(tree, cur_node);
    
        if (ret == -1)
            return -1;
    }

    else 
        print_and_speak("Okay, but I'm sad that you're not helping me...");
    
    return 0;
}

//===================================================================

int _compare_object_init(Compare_obj* object, Node* node, Stack* stack, 
                                                            LOG_PARAMS) {

    akinator_log_report();

    NODE_PTR_CHECK(node);
    STACK_PTR_CHECK(stack);
    COMPARE_OBJ_PTR_CHECK(object);

    object->node  = node;
    object->stack = stack;

    return 0;
}

//===================================================================

int _akinator_show_difference(Compare_obj* first_obj, 
                              Compare_obj* second_obj, FILE* output, LOG_PARAMS) {

    akinator_log_report();
    COMPARE_OBJ_PTR_CHECK(first_obj);
    COMPARE_OBJ_PTR_CHECK(second_obj);

    if (output == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    const char* first_name = first_obj->node->data;
    const char* second_name = second_obj->node->data;

    if (first_obj->stack->data[1] != second_obj->stack->data[1]) {

        fprintf(output, "Compare: %s and %s have nothing in same.\n", first_name, 
                                                                      second_name);

        show_definition(first_obj,  output);
        show_definition(second_obj, output);

        return 0;
    }

    fprintf(output, "Compare: %s and %s have in the same,"
                                        " that they are: ", first_obj->node->data, 
                                                            second_obj->node->data);

    int counter = 0;

    Node* cur_node  = first_obj->stack->data[counter];
    Node* next_node = first_obj->stack->data[counter + 1];

    while (first_obj->stack->data[counter + 1] == 
          second_obj->stack->data[counter + 1]) {

            if (next_node == cur_node->left_son) {

                fprintf(output, "NOT ");
            }

            fprintf(output, "%s, ", cur_node->data);

            counter++;

            cur_node  = first_obj->stack->data[counter];
            next_node = first_obj->stack->data[counter + 1];
          }

    fprintf(output, "but %s is %s %s, ", first_name, 
                                        (first_obj->stack->data[counter + 1] == 
                                         first_obj->stack->data[counter]->left_son)? "NOT": "", 
                                         first_obj->stack->data[counter]->data);

    fprintf(output, "while %s is %s %s. \n", second_name, 
                                            (second_obj->stack->data[counter + 1] == 
                                             second_obj->stack->data[counter]->left_son)? "NOT": "", 
                                             second_obj->stack->data[counter]->data);

    return 0;
}

//===================================================================

int _akinator_compare_objects(struct Tree* tree, const char* first_name, 
                                                 const char* second_name, 
                                                FILE* output, LOG_PARAMS) {

    akinator_log_report();
    TREE_PTR_CHECK(tree);

    if (first_name == NULL || second_name == NULL) {

        error_report(INV_NODE_NAME);
        return -1;
    }

    if (output == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    int64_t first_hash  = get_hash((void*)first_name,  strlen(first_name));
    int64_t second_hash = get_hash((void*)second_name, strlen(second_name));

    if (first_hash == second_hash) {

        fprint_and_speak(output, "These objects are totally the same!");
        return 0;
    }

    struct Stack first_stack = { 0 };
    int ret = stack_ctor(&first_stack);
    if (ret == -1)
        return -1;

    Node* first_result = akinator_tree_search(tree, &first_stack, 
                                                     first_hash);
    if (first_result == NULL) 
        return -1;

    if (first_result == Node_not_found) {
        
        fprint_and_speak(output, "Tree search: First object not found.");

        return stack_dtor(&first_stack);
    }

    struct Stack second_stack = { 0 };
    ret = stack_ctor(&second_stack);
    if (ret == -1)
        return -1;

    Node* second_result = akinator_tree_search(tree, &second_stack, 
                                                      second_hash);
    if (second_result == NULL) 
        return -1;

    if (second_result == Node_not_found) {
        
        fprint_and_speak(output, "Tree search: Second object not found.");

        stack_dtor(&first_stack);
        return stack_dtor(&second_stack);
    }

    struct Compare_obj first_obj  = { 0 };
    struct Compare_obj second_obj = { 0 };

    compare_object_init(&first_obj,  first_result,  &first_stack);
    compare_object_init(&second_obj, second_result, &second_stack);

    ret = akinator_show_difference(&first_obj, &second_obj, output);
    if (ret == -1)
        return -1;

    ret = stack_dtor(&first_stack);
    if (ret == -1)
        return -1;

    ret = stack_dtor(&second_stack);
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

int _show_definition(struct Compare_obj* obj, FILE* output, LOG_PARAMS) {

    akinator_log_report();
    COMPARE_OBJ_PTR_CHECK(obj);

    Node* node = obj->node;
    Stack* stack = obj->stack;

    NODE_PTR_CHECK(node);
    STACK_PTR_CHECK(stack);

    if (output == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    fprintf(output, "\n" "Object \" " ELEM_SPEC " \". Characteristics: is ", node->data);

    Node* cur_node = NULL;

    for (int counter = 0; counter < stack->count; counter++) {

        cur_node = stack->data[counter];

        if (counter == stack->count - 1)
            break;

        Node* next_node = stack->data[counter + 1];

        if (cur_node->left_son == next_node)
            fprintf(output, "NOT ");
        
        fprintf(output, "%s%c ", cur_node->data, 
               (counter == stack->count - 2)? '.': ',');
    }

    fprintf(output, "\n");

    return 0;
}

//===================================================================

int _akinator_get_definition(struct Tree* tree, const char* node_name, 
                                             FILE* output, LOG_PARAMS) {

    akinator_log_report();
    TREE_PTR_CHECK(tree);

    if (output == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    if (node_name == NULL) {

        error_report(INV_NODE_NAME);
        return -1;
    }

    int64_t name_hash = get_hash((void*)node_name, strlen(node_name));

    struct Stack stack = { 0 };
    int ret = stack_ctor(&stack);
    if (ret == -1)
        return -1;

    Node* result = akinator_tree_search(tree, &stack, name_hash);
    if (result == NULL) 
        return -1;

    if (result == Node_not_found)
        fprint_and_speak(output, "Tree search: Node not found.")
    else {
        
        struct Compare_obj obj = { 0 };
        compare_object_init(&obj, result, &stack);

        show_definition(&obj, output);
    }
    ret = stack_dtor(&stack);
    if (ret == -1)
        return -1;

    return 0;
}

//===================================================================

int _akinator_tree_validator(struct Tree* tree, LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    if (node_visiter(tree->root, _akinator_node_validator) == -1 )
        return -1;

    else  {

        #ifdef TREE_DEBUG

            tree_dump(tree);

        #endif

        return 0;
    }
}

//===================================================================

int _akinator_node_validator(struct Node* node, LOG_PARAMS) {

    tree_log_report();
    NODE_PTR_CHECK(node);

    if (node->special_flag && (!node->left_son || !node->right_son)) {

        error_report(NODE_INV_SPECIAL_FLAG);
        return -1;
    }

    return 0; 
}

//===================================================================

Node* _akinator_tree_search(struct Tree* tree, struct Stack* stack, int64_t hash, 
                                                                      LOG_PARAMS) {

    akinator_log_report();
    if (tree == NULL) {

        error_report(INV_TREE_PTR);
        return NULL;
    }

    int is_ok = akinator_tree_validator(tree);
    if (is_ok == -1)
        return NULL;

    Node* found =  akinator_node_search(tree->root, stack, hash);

    if (found == NULL)
        return NULL;

    if (found == Node_not_found)
        return found;

    if (found->special_flag != 0) {

        error_report(TREE_INV_SEARCH);
        return Node_not_found;
    }

    return found;
}

//===================================================================

Node* _akinator_node_search(struct Node* node, struct Stack* stack, int64_t hash, 
                                                                      LOG_PARAMS) {

    akinator_log_report();
    if (node == NULL) {

        error_report(INV_NODE_PTR);
        return NULL;
    }

    int ret = stack_push(stack, node);
    if (ret == -1)
        return NULL;

    #ifdef TREE_CALCULATE_HASH_FROM_DATA

        int64_t node_data_hash = node->hash;

    #else

        int64_t node_data_hash = get_hash((void*)node->data, 
                                          strlen(node->data));

    #endif

    if (node_data_hash == hash) 
        return node;

    if (node->left_son != NULL) {

        Node* result = akinator_node_search(node->left_son, stack, hash);
        if (result != Node_not_found)
            return result;
    }

    if (node->right_son != NULL) {

        Node* result = akinator_node_search(node->right_son, stack, hash);
        if (result != Node_not_found)
            return result;
    }

    int err = 0;

    stack_pop(stack, &err);
    return Node_not_found;
}

//===================================================================

int _akinator_tree_ctor(struct Tree* tree, LOG_PARAMS) {

    akinator_log_report();
    TREE_PTR_CHECK(tree);

    int ret = tree_ctor(tree);
    if (ret == -1)
        return -1;

    ret = node_init(tree->root, "Неизвестно кто");
    if (ret == -1)
        return -1;

    tree_draw_graph(tree);

    AKINATOR_TREE_VERIFICATION(tree);

    return 0;
}

//===================================================================

int _akinator_init_base(struct Tree* tree, struct Text* text, const char* filename, 
                                                                        LOG_PARAMS) {

    AKINATOR_TREE_VERIFICATION(tree);

    akinator_log_report();
    TREE_PTR_CHECK(tree);

    int ret = tree_read_from_file(tree, text, filename);
    if (ret == -1)
        return -1;

    AKINATOR_TREE_VERIFICATION(tree);

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

int _tree_read_from_file(struct Tree* tree, struct Text* text, const char* filename, 
                                                                         LOG_PARAMS) {

    tree_log_report();
    TREE_PTR_CHECK(tree);

    if (filename == NULL) {

        error_report(INV_FILE_NAME);
        return -1;
    }

    char* buffer = text_init(filename, text, LOG_ARGS);
    replace_nulls_with_spaces(buffer, text->size);

    struct Buffer_struct buffer_struct = { 0 };
    buffer_struct_init(&buffer_struct, buffer, (int)text->size, 0);

    int ret = node_read_from_buffer(tree->root, &buffer_struct);
    if (ret == -1) 
        return -1;

    return 0;
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
    
    if (symb != '{') {

        error_report(TREE_TEXT_INV_SYNTAXIS);
        buffer_dump(buffer_struct);
        return -1;
    }

    buffer_struct->pos += offset;
    const char* node_data = buffer_struct->buffer + buffer_struct->pos;

    symb = 0;
    int after_word = 0;
    int before_word = 0;

    while (symb != '{' && symb != '}') {

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

    if (*(buffer_struct->buffer + buffer_struct->pos) == '}') {

        buffer_struct->pos++;
        return 0;
    }


    if (*(buffer_struct->buffer + buffer_struct->pos) == '{') {

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