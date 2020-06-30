#ifndef LIB_MACRO_DBG_MACRO
#define LIB_MACRO_DBG_MACRO

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#define clean_errno() (errno == 0 ? "0" : strerror(errno))

#ifdef NODEBUG
#define trc_debug(M, ...)
#else
#define trc_debug(M, ...)         fprintf(stderr, "[\e[0;97mDBG\e[0m] (%s:%d): " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif
#define trc_info(M, ...)    fprintf(stderr, "[\e[0;32mINF\e[0m] (%s:%d): " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define trc_warning(M, ...) fprintf(stderr, "[\e[1;33mWRN\e[0m] (%s:%d errno: %s): " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define trc_error(M, ...)   fprintf(stderr, "[\e[1;31mERR\e[0m] (%s:%d errno: %s): " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define trc_input(M, ...) fprintf(stderr, "\e[1;34m>>>\e[0m " M "\n", ##__VA_ARGS__)
#define trc_attention(M, ...) fprintf(stderr, "\e[1;33m***\e[0m " M "\n", ##__VA_ARGS__)
#define trc_output(M, ...) fprintf(stderr, "\e[1;32m<<<\e[0m " M "\n", ##__VA_ARGS__)


#endif // LIB_MACRO_DBG_MACRO
