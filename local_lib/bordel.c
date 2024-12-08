#include <profan/filesys.h>
#include <string.h>
#include <stdlib.h>
#include <profan.h>
#include <stdio.h>

#include <bordel.h>

int mkdir(const char *pathname, uint32_t mode) {
    // if pathname don't start with '/' add it
    char *tmp = malloc(strlen(pathname) + 2);
    if (pathname[0] != '/') {
        tmp[0] = '/';
        strcpy(tmp + 1, pathname);
    } else {
        strcpy(tmp, pathname);
    }

    if (fu_path_to_sid(SID_ROOT, tmp)) {
        free(tmp);
        return 0;
    }

    uint32_t output = fu_dir_create(0, (char *) tmp);
    free(tmp);
    return output ? 0 : -1;
}
