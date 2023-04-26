#include <syscall.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <bordel.h>

jmp_buf env;

extern int main(int argc, char **argv);

int entry(int argc, char **argv) {
    return entry2(argc, argv);
}

int entry2(int argc, char **argv) {
    // patch argv

    int new_argc = argc - 1;
    if (new_argc < 1) new_argc = 1;
    char **new_argv = malloc(sizeof(char *) * new_argc + 1);
    serial_debug("new_argc = %d\n", new_argc);
    for (int i = 0; i <= new_argc; i++) {
        if (i == 0) {
            new_argv[i] = malloc(5);
            strcpy(new_argv[i], "doom");
        } else if (i == new_argc) {
            serial_debug("argv[%d] = NULL\n", i);
            new_argv[i] = NULL;
            break;
        } else {
            new_argv[i] = argv[i + 1];
        }
        serial_debug("argv[%d] = %s\n", i, new_argv[i]);
    }

    // call main and setup exit
    int val;

    val = setjmp(env);
    if (!val) {
        val = main(new_argc, new_argv) + 1;
    }

    serial_debug("exit with %d\n", val - 1);

    // free memory
    int freed = c_mem_free_all(c_process_get_pid());
    serial_debug("%d alloc freed\n", freed);

    return val - 1;
}

void exit(int status) {
    longjmp(env, status + 1);
}
