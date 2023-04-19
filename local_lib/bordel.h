#ifndef BORDEL_H
#define BORDEL_H

#include <type.h>

// uintptr_t
#define uintptr_t unsigned long

// errno variable
static int errno;

#define O_RDONLY 0
#define O_WRONLY 1
#define O_CREAT 2
#define O_TRUNC 4
#define O_BINARY 8

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// define jmp_buf type
typedef int jmp_buf[6];

#ifdef exit
#undef exit
#endif

// entry.c
void exit(int status);

// bordel.c
void init_bordel();

void serial_debug(char *fmt, ...);

int fseek(FILE *stream, long offset, int whence);

int isspace(int c);
int toupper(int c);

int mkdir(const char *pathname, uint32_t mode);

#endif
