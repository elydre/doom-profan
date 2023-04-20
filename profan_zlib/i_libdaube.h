#ifndef LIBDAUBE_ID
#define LIBDAUBE_ID 1015

#include <type.h>

typedef struct libdaude_func_t {
    int func_id;

    uint32_t arg1;
    uint32_t arg2;
    uint32_t arg3;
} libdaude_func_t;

typedef struct window_t {
    void *parent_desktop;
    void **button_array; // button_t **
    int buttons_count;
    uint32_t magic;

    char *name;

    int x;
    int y;
    int width;
    int height;

    int in_x;
    int in_y;
    int in_width;
    int in_height;

    uint8_t changed;

    int old_x;
    int old_y;
    int old_width;
    int old_height;

    uint32_t *buffer;
    uint8_t *visible;

    uint8_t is_lite;    // no border

    int priority;

    uint8_t cant_move;
} window_t;

typedef struct mouse_t {
    int x;
    int y;

    int clicked_window_id;
    int window_x_dec;
    int window_y_dec;

    int already_clicked;
} mouse_t;

typedef struct desktop_t {
    window_t **windows;

    mouse_t *mouse;

    int nb_windows;
    int screen_width;
    int screen_height;
    int max_windows;
    int current_priority;

    libdaude_func_t *func_run_stack;
    int func_run_stack_size;

    uint32_t *screen_buffer;

    uint8_t is_locked;
} desktop_t;

typedef struct clickevent_t {
    void *button; // button_t *
    mouse_t *mouse;
    int x;
    int y;
} clickevent_t;

typedef struct button_t {
    window_t *window;
    int x;
    int y;
    int width;
    int height;
    int is_clicked;
    void (*callback)(clickevent_t *);
} button_t;

#define get_func_addr ((int (*)(int, int)) *(int *) 0x1ffffb)

#ifndef LIBDAUBE_C

#define window_set_pixel(window, x, y, color) window_set_pixel_func(window, x, y, color, 1)
#define window_set_pixel_out(window, x, y, color) window_set_pixel_func(window, x, y, color, 0)

#define desktop_init ((desktop_t *(*)(int, int, int)) get_func_addr(LIBDAUBE_ID, 3))
#define window_create ((window_t *(*)(desktop_t *, char *, int, int, int, int, uint8_t, uint8_t, uint8_t)) get_func_addr(LIBDAUBE_ID, 4))
#define desktop_refresh ((void (*)(desktop_t *)) get_func_addr(LIBDAUBE_ID, 5))
#define window_move ((void (*)(window_t *, int, int)) get_func_addr(LIBDAUBE_ID, 6))
#define window_resize ((void (*)(window_t *, int, int)) get_func_addr(LIBDAUBE_ID, 7))
#define window_set_pixel_func ((void (*)(window_t *, int, int, uint32_t, uint8_t)) get_func_addr(LIBDAUBE_ID, 8))
#define window_fill ((void (*)(window_t *, uint32_t)) get_func_addr(LIBDAUBE_ID, 9))
#define window_refresh ((void (*)(window_t *)) get_func_addr(LIBDAUBE_ID, 10))
#define refresh_mouse ((void (*)(desktop_t *)) get_func_addr(LIBDAUBE_ID, 12))
#define desktop_get_main ((desktop_t *(*)(void)) get_func_addr(LIBDAUBE_ID, 13))
#define window_delete ((void (*)(window_t *)) get_func_addr(LIBDAUBE_ID, 14))
#define create_button ((button_t *(*)(window_t *, int, int, int, int, void (*)(clickevent_t *))) get_func_addr(LIBDAUBE_ID, 15))
#define window_wait_delete ((void (*)(desktop_t *, window_t *)) get_func_addr(LIBDAUBE_ID, 16))
#define desktop_run_stack ((void (*)(desktop_t *)) get_func_addr(LIBDAUBE_ID, 17))

#endif
#endif
