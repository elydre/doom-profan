#include <filesys.h>
#include <string.h>
#include <stdlib.h>
#include <profan.h>
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

int mkdir(const char *pathname, uint32_t mode) {
    // if pathname don't start with '/' add it
    char *tmp = malloc(strlen(pathname) + 2);
    if (pathname[0] != '/') {
        tmp[0] = '/';
        strcpy(tmp + 1, pathname);
    } else {
        strcpy(tmp, pathname);
    }

    serial_debug("mkdir: %s\n", (char *) tmp);
    sid_t output = fu_dir_create(0, (char *) tmp);
    serial_debug("mkdir: d%ds%d\n", output.device, output.sector);
    free(tmp);
    return IS_NULL_SID(output) ? -1 : 0;
}
