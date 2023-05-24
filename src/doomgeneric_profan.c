//doomgeneric for cross-platform development library 'Simple DirectMedia Layer'

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <syscall.h>
#include <bordel.h>

#include <i_libdaube.h>

window_t *window;


void DG_Init() {
    // wake up the parent process
    // c_process_wakeup(c_process_get_ppid(c_process_get_pid()));

    // get the main desktop
    desktop_t *main_desktop = desktop_get_main();

    // create a window and add an exit button
    window = window_create(main_desktop, "doom generic", 100, 100, DOOMGENERIC_RESX, DOOMGENERIC_RESY, 0, 0, 0);
    desktop_refresh(main_desktop);

    window_refresh(window);
}

uint8_t convertToDoomKey(uint8_t scancode) {
    uint8_t key = 0;

    switch (scancode) {
    case 0x9C:
    case 0x1C:
        key = KEY_ENTER;
        break;
    case 0x01:
        key = KEY_ESCAPE;
        break;
    case 0xCB:
    case 0x4B:
        key = KEY_LEFTARROW;
        break;
    case 0xCD:
    case 0x4D:
        key = KEY_RIGHTARROW;
        break;
    case 0xC8:
    case 0x48:
        key = KEY_UPARROW;
        break;
    case 0xD0:
    case 0x50:
        key = KEY_DOWNARROW;
        break;
    case 0x1D:
        key = KEY_FIRE;
        break;
    case 0x39:
        key = KEY_USE;
        break;
    case 0x2A:
    case 0x36:
        key = KEY_RSHIFT;
        break;
    case 0x15:
        key = 'y';
        break;
    case 0x02:
        key = '1';
        break;
    case 0x03:
        key = '2';
        break;
    case 0x04:
        key = '3';
        break;
    case 0x05:
        key = '4';
        break;
    case 0x06:
        key = '5';
        break;
    case 0x07:
        key = '6';
        break;
    case 0x08:
        key = '7';
        break;
    case 0x09:
        key = '8';
        break;
    default:
        break;
    }

    return key;
}


void DG_DrawFrame() {
    uint32_t pos = 0;
    for (int y = 0; y < DOOMGENERIC_RESY; y++) {
        for (int x = 0; x < DOOMGENERIC_RESX; x++) {
            window_display_pixel(window, x, y, DG_ScreenBuffer[pos++]);
        }
    }
    // window_refresh(window);
}

void DG_SleepMs(uint32_t ms) {
    // serial_debug("sleeping for %d ms\n", ms);
    c_process_sleep(c_process_get_pid(), ms);
    return;
}

uint32_t DG_GetTicksMs() {
    return c_timer_get_ms();
}

int DG_GetKey(int* pressed, uint8_t* doomKey) {
    uint8_t scancode, key;

    scancode = (uint8_t) c_kb_get_scfh();
    if (scancode == 0) return 0;

    // serial_debug("scancode: %d\n", scancode);

    if (scancode > 127) {
        scancode -= 128;
        *pressed = 0;
    } else {
        *pressed = 1;
    }

    key = convertToDoomKey(scancode);
    if (key == 0) return 0;

    *doomKey = key;

    // serial_debug("key: %d, pressed: %d\n", *doomKey, *pressed);

    return 1;
}

void DG_SetWindowTitle(const char * title) {
    serial_debug("setting window title\n");
    return;
}

int main(int argc, char **argv) {
    doomgeneric_Create(argc, argv);

    for (int i = 0; ; i++) {
        doomgeneric_Tick();
    }

    return 0;
}
