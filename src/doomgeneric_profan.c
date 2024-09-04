//doomgeneric for cross-platform development library 'Simple DirectMedia Layer'

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <profan/syscall.h>
#include <bordel.h>

uint32_t *current_screen;

void DG_Init() {
    current_screen = calloc(DOOMGENERIC_RESX * DOOMGENERIC_RESY, sizeof(uint32_t));
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
    uint32_t *fb = syscall_vesa_fb();
    uint32_t pitch = syscall_vesa_pitch();

    int pos = -1;
    for (int y = 0; y < DOOMGENERIC_RESY * 2; y += 2) {
        for (int x = 0; x < DOOMGENERIC_RESX * 2; x += 2) {
            pos++;
            if (current_screen[pos] == DG_ScreenBuffer[pos]) continue;
            current_screen[pos] = DG_ScreenBuffer[pos];
            fb[y * pitch + x] = current_screen[pos];
            fb[y * pitch + x + 1] = current_screen[pos];
            fb[(y + 1) * pitch + x] = current_screen[pos];
            fb[(y + 1) * pitch + x + 1] = current_screen[pos];
        }
    }
}

void DG_SleepMs(uint32_t ms) {
    syscall_process_sleep(syscall_process_pid(), ms);
    return;
}

uint32_t DG_GetTicksMs() {
    return syscall_timer_get_ms();
}

int DG_GetKey(int* pressed, uint8_t* doomKey) {
    uint8_t scancode, key;

    scancode = (uint8_t) syscall_sc_get();
    if (scancode == 0) return 0;

    if (scancode > 127) {
        scancode -= 128;
        *pressed = 0;
    } else {
        *pressed = 1;
    }

    key = convertToDoomKey(scancode);
    if (key == 0) return 0;

    *doomKey = key;

    return 1;
}

void DG_SetWindowTitle(const char * title) {
    return;
}

int main(int argc, char **argv) {
    doomgeneric_Create(argc, argv);

    for (int i = 0; ; i++) {
        doomgeneric_Tick();
    }

    return 0;
}
