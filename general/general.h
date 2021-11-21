#pragma once

#include "../logs/errors_and_logs.h"
#include "../logs/log_definitions.h"
#include "../akinator/akinator_config.h"

//===================================================================

#define GENERAL_LOGS

//===================================================================

#ifdef ESPEAK

    #define print_and_speak(string) {                                   \
                                                                        \
        printf("\n\n%s\n\n", string);                                   \
                                                                        \
        system("espeak [" #string "]");                                 \
    }

//-------------------------------------------------------------------

    #define fprint_and_speak(output, string) {                          \
                                                                        \
        fprintf(output, "\n\n%s\n\n", string);                          \
                                                                        \
        if (output == stdout)                                           \
            system("espeak [" #string "]");                             \
    }

#else

    #define print_and_speak(string) \
            printf("\n\n%s\n\n", string);

    #define fprint_and_speak(output, string) \
            fprintf(output, "\n\n%s\n\n", string);

#endif

//===================================================================

#ifdef GENERAL_LOGS

       #define general_log_report() \
               log_report()

#else

       #define general_log_report() ""

#endif


//===================================================================

#define clearstdin() \
       _clearstdin(LOG_ARGS)

#define open_file(filename, mode) \
       _open_file(filename, mode, LOG_ARGS)

#define close_file(file_ptr) \
       _close_file(file_ptr, LOG_ARGS)

#define my_recalloc(ptr, number, prev_number, size_of_elem) \
       _my_recalloc(ptr, number, prev_number, size_of_elem, \
                                                 LOG_ARGS)

#define clear_memory(base, num, size) \
       _clear_memory(base, num, size, LOG_ARGS)

#define is_memory_clear(base, num, size) \
       _is_memory_clear(base, num, size, LOG_ARGS)

#define get_hash(base, len) \
        get_hash_(base, len, LOG_ARGS)

//===================================================================

int _clearstdin(LOG_PARAMS);

void* _my_recalloc(void* ptr, size_t number, size_t prev_number, size_t size_of_elem,
                                                                         LOG_PARAMS);

int _clear_memory(void* base, size_t num, size_t size, LOG_PARAMS);

int _is_memory_clear(void* base, size_t num, size_t size, LOG_PARAMS);

void ___System_Sleep(float _Duration);

int my_swap(void* first_, void* second_, size_t size);

int64_t get_hash_(void* void_base, long unsigned int len, LOG_PARAMS);

//===================================================================

FILE* _open_file(const char* filename, const char* mode, LOG_PARAMS);

int _close_file(FILE* file_prt, LOG_PARAMS);