#ifndef UNISTD_ID
#define UNISTD_ID 1014

#include <type.h>


#define close ((int (*)(int)) get_func_addr(UNISTD_ID, 6))
#define getcwd ((char *(*)(char *, size_t)) get_func_addr(UNISTD_ID, 27))
#define lseek ((off_t (*)(int, off_t, int)) get_func_addr(UNISTD_ID, 48))
#define unlink ((int (*)(const char *)) get_func_addr(UNISTD_ID, 78))
#define read ((ssize_t (*)(int, void *, size_t)) get_func_addr(UNISTD_ID, 55))
#define write ((ssize_t (*)(int, const void *, size_t)) get_func_addr(UNISTD_ID, 81))

#endif
