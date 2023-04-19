#include <syscall.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <bordel.h>

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

// __udivdi3 definition
unsigned long long __udivdi3(unsigned long long num, unsigned long long den) {
    unsigned long long quot = 0, qbit = 1;

    if (den == 0) {
        return 0; // divide by zero
    }

    // Left-justify denominator and count shift
    while ((signed long long)den >= 0) {
        den <<= 1;
        qbit <<= 1;
    }

    while (qbit) {
        if (den <= num) {
            num -= den;
            quot += qbit;
        }
        den >>= 1;
        qbit >>= 1;
    }

    return quot;
}

// __divdi3 definition
signed long long __divdi3(signed long long num, signed long long den) {
    int neg = 0;

    if (num < 0) {
        num = -num;
        neg = !neg;
    }
    if (den < 0) {
        den = -den;
        neg = !neg;
    }

    return neg ? -__udivdi3(num, den) : __udivdi3(num, den);
}

// __umoddi3 definition
unsigned long long __umoddi3(unsigned long long num, unsigned long long den) {
    if (den == 0) {
        return num; // divide by zero
    }

    // Left-justify denominator and count shift
    while ((signed long long)den >= 0) {
        den <<= 1;
    }

    while (den > num) {
        den >>= 1;
    }

    return num - den;
}

int str_len(char s[]) {
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

void str_reverse(char s[]) {
    int i = 0;
    int j = str_len(s) - 1;
    char tmp;
    while (i < j) {
        tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
        i++;
        j--;
    }
}

void int2str(int n, char s[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) s[i++] = '-';
    s[i] = '\0';

    str_reverse(s);
}

void hex2str(uint32_t n, char s[]) {
    int i = 0;
    int tmp;
    char hex[] = "0123456789abcdef";
    do {
        tmp = n % 16;
        s[i++] = hex[tmp];
    } while ((n /= 16) > 0);
    s[i] = '\0';
    str_reverse(s);
}

void serial_debug(char *fmt, ...) {
    char *args = (char *) &fmt;
    args += 4;
    int i = 0;
    char *char_buffer = malloc(0x1000);
    int buffer_i = 1;
    char_buffer[0] = ' ';
    while (fmt[i] != '\0') {
        if (fmt[i] == '%') {
            i++;
            if (fmt[i] == 's') {
                char *s = *((char **) args);
                args += 4;
                for (int j = 0; s[j] != '\0'; j++) {
                    char_buffer[buffer_i] = s[j];
                    buffer_i++;
                }                
            } else if (fmt[i] == 'c') {
                char c = *((char *) args);
                args += 4;
                char_buffer[buffer_i] = c;
                buffer_i++;
            } else if (fmt[i] == 'd') {
                int n = *((int *) args);
                args += 4;
                char s[20];
                int2str(n, s);
                for (int j = 0; s[j] != '\0'; j++) {
                    char_buffer[buffer_i] = s[j];
                    buffer_i++;
                }
            } else if (fmt[i] == 'x') {
                uint32_t n = *((int *) args);
                args += 4;
                char s[20];
                hex2str(n, s);
                for (int j = 0; s[j] != '\0'; j++) {
                    char_buffer[buffer_i] = s[j];
                    buffer_i++;
                }
            } else if (fmt[i] == '%') {
                char_buffer[buffer_i] = '%';
                buffer_i++;
            }
        } else {
            char_buffer[buffer_i] = fmt[i];
            buffer_i++;
        }
        i++;
    }
    char_buffer[buffer_i] = '\0';
    c_serial_print(SERIAL_PORT_A, char_buffer);
    free(char_buffer);
}

int isspace(int c) {
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v') {
        return 1;
    }
    return 0;
}

int toupper(int c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }
    return c;
}

int fseek(FILE *stream, long offset, int whence) {
    printf("call fseek file: %s, offset: %d, whence: %d\n", stream->filename, offset, whence);
    // we check if the file is null
    if (stream == NULL) {
        printf("stream is null\n");
        return 0;
    }
    // we check the whence
    switch (whence) {
        case SEEK_SET:
            // we check if the offset is valid
            if (offset < 0 || offset >= stream->buffer_size) {
                return 0;
            }
            // we set the buffer position
            stream->buffer_pos = offset;
            printf("set buffer pos to %d\n", stream->buffer_pos);
            break;
        case SEEK_CUR:
            // we set the buffer position
            stream->buffer_pos += offset;
            printf("set buffer pos to %d\n", stream->buffer_pos);
            break;
        case SEEK_END:
            // we set the buffer position
            stream->buffer_pos = stream->buffer_size + offset;
            printf("set buffer pos to %d\n", stream->buffer_pos);
            break;
        default:
            printf("whence is not valid\n");
            return 0;
    }
    printf("fseek success\n");
    return 0;
}

int mkdir(const char *pathname, uint32_t mode) {
    serial_debug("mkdir: %s\n", (char *) pathname);

    int len = strlen((char *) pathname);
    char *parent_path = malloc(len + 2);
    char *dir_name = malloc(len + 2);

    if (pathname[len - 1] == '/') {
        len--;
    }

    int i = len - 1;
    while (pathname[i] != '/') i--;
    int j = 0;
    while (j < i) {
        parent_path[j] = pathname[j];
        j++;
    }
    parent_path[j] = '\0';
    int k = 0;
    while (j < len) {
        dir_name[k] = pathname[j];
        j++;
        k++;
    }
    dir_name[k] = '\0';

    if (parent_path[0] != '/') {
        char *tmp = malloc(len + 2);
        strcpy(tmp, "/");
        strcat(tmp, parent_path);
        strcpy(parent_path, tmp);
        free(tmp);
    }

    if (dir_name[0] == '/') {
        char *tmp = malloc(len + 2);
        strcpy(tmp, dir_name + 1);
        strcpy(dir_name, tmp);
        free(tmp);
    }

    serial_debug("parent_path: '%s', dir_name: '%s'\n", parent_path, dir_name);

    // create directory
    int ret = c_fs_make_dir(parent_path, dir_name);

    free(parent_path);
    free(dir_name);

    return ret;
}
