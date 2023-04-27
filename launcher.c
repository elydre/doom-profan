#include <syscall.h>

int entry(int argc, char **argv) {
    // we need this launcher to give more virtual 
    c_run_ifexist_full("/bin/other/doom-core.bin", argc, argv, 0xC0000000, 0x100000, 0x10000);
}
