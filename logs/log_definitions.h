#pragma once

#define LOG_PARAMS \
        const char* func_name, const char* file_name, int line, FILE* logs_file

#define LOG_ARGS \
        __FUNCTION__, __FILE__, __LINE__, logs_file

#define LOGS_ARGS_USE \
        func_name, file_name, line, logs_file
;