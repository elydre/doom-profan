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

typedef struct fd_to_path_t {
    int fd;
    char path[128];
    int pos;
    int size;
} fd_to_path_t;

fd_to_path_t *fd_to_path;

void init_bordel() {
    // init fd_to_path
    fd_to_path = malloc(sizeof(fd_to_path_t) * 256);
    for (int i = 0; i < 256; i++) {
        fd_to_path[i].fd = -1;
    }
    serial_debug("bordel initialized\n");
}

int open(const char *path, ...) {
    serial_debug("open: %s\n", path);
    // add the path to the fd_to_path
    int dispo = -1;
    for (int i = 0; i < 256; i++) {
        if (fd_to_path[i].fd == -1) {
            dispo = i;
            break;
        }
    }

    if (dispo == -1) {
        serial_debug("open: no more fd available\n");
        return -1;
    }

    // if the start of the path is not /, add it
    char *new_path = malloc(strlen(path) + 2);
    if (path[0] != '/') {
        new_path[0] = '/';
        strcpy(new_path + 1, path);
    } else {
        strcpy(new_path, path);
    }

    // check if the file exists
    if (!c_fs_does_path_exists(new_path)) {
        serial_debug("open: creating %s in /\n", new_path + 1);
        c_fs_make_file("/", new_path + 1);
    }

    fd_to_path[dispo].fd = dispo;
    strcpy(fd_to_path[dispo].path, new_path);
    fd_to_path[dispo].pos = 0;
    fd_to_path[dispo].size = c_fs_get_file_size(new_path);

    serial_debug("open: %s -> fd %d\n", new_path, dispo);
    return dispo;
}

off_t lseek(int fd, off_t offset, int whence) {
    if (fd_to_path[fd].fd == -1) {
        serial_debug("lseek: fd %d not found\n", fd);
        return -1;
    }
    char *path = fd_to_path[fd].path;

    // change the position
    switch (whence) {
        case SEEK_SET:
            fd_to_path[fd].pos = offset;
            break;
        case SEEK_CUR:
            fd_to_path[fd].pos += offset;
            break;
        case SEEK_END:
            fd_to_path[fd].pos = fd_to_path[fd].size + offset;
            break;
        default:
            return -1;
    }
    return fd_to_path[fd].pos;
}

int close(int fd) {
    serial_debug("close: fd %d\n", fd);
    // remove the path from the fd_to_path
    if (fd_to_path[fd].fd == -1) {
        return -1;
    }

    fd_to_path[fd].fd = -1;
    return 0;
}

ssize_t read(int fd, void *buf, size_t count) {
    serial_debug("read: fd %d, count %d\n", fd, count);

    if (fd_to_path[fd].fd == -1) {
        return -1;
    }

    char *path = fd_to_path[fd].path;
    int pos = fd_to_path[fd].pos;
    int size = fd_to_path[fd].size;

    if (pos >= size) {
        return 0;
    }

    int read = min(count, size - pos);
    serial_debug("read: %d bytes read\n", read);

    uint8_t *buffer = malloc(size + 1);
    c_fs_read_file(path, buffer);
    
    for (int i = 0; i < read; i++) {
        ((uint8_t *) buf)[i] = buffer[pos + i];
    }

    fd_to_path[fd].pos += read;
    free(buffer);
    return read;
}

int unlink(const char *pathname) {
    serial_debug("unlink: %s\n", (char *) pathname);
    // file deletion is not implemented in FSv2
    return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    for (int i = 0; i < n; i++) {
        if (s1[i] != s2[i]) {
            return s1[i] - s2[i];
        }
    }
    return 0;
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

int fputs(const char *s, FILE *stream) {
    if (stream == stdout) {
        c_kprint((char *) s);
        return 0;
    }
    
    fwrite(s, 1, str_len((char *) s), stream);
    return 0;
}

int fputc(int c, FILE *stream) {
    char s[2];
    s[0] = c;
    s[1] = '\0';

    if (stream == stdout) {
        c_kprint(s);
        return 0;
    }

    // use fwrite to save the file content
    fwrite(s, 1, 1, stream);
    return 0;
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

long double strtold(const char *theString, char **end) {
    c_kprint("strtold is not implemented yet...\n");
    return 0;
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

// fdopen
FILE *fdopen(int fd, const char * mode) {
    // get path from fd
    char *path = fd_to_path[fd].path;
    serial_debug("fdopen: %s\n", path);
    // open file
    FILE *file = fopen(path, mode);
    return file;
}

// gettimeofday
int GetTickCount() {
    return c_timer_get_ms();
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