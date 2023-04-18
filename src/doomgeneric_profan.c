//doomgeneric for cross-platform development library 'Simple DirectMedia Layer'

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <syscall.h>

#define KEYQUEUE_SIZE 16


static unsigned char convertToDoomKey(unsigned int key) {
    serial_debug("converting key %d to doom key\n", key);
    return 0;
}

static void addKeyToQueue(int pressed, unsigned int keyCode) {
    serial_debug("adding key to queue\n");
    return;
}

static void handleKeyInput() {
    serial_debug("handling key input\n");
    return;
}


void DG_Init() {
    return;
}

void DG_DrawFrame() {
    serial_debug("drawing frame\n");
    for (int x = 0; x < DOOMGENERIC_RESX; x++) {
        for (int y = 0; y < DOOMGENERIC_RESY; y++) {
            c_vesa_set_pixel(x, y, DG_ScreenBuffer[x + y * DOOMGENERIC_RESX]);
        }
    }
}

void DG_SleepMs(uint32_t ms) {
    serial_debug("sleeping for %d ms\n", ms);
    return;
}

uint32_t DG_GetTicksMs() {
    serial_debug("getting ticks\n");
    return c_timer_get_ms();
}

int DG_GetKey(int* pressed, unsigned char* doomKey) {
    serial_debug("getting key\n");
    return 0;
}

void DG_SetWindowTitle(const char * title) {
    serial_debug("setting window title\n");
    return;
}

int main(int argc, char **argv) {
    doomgeneric_Create(argc, argv);

    for (int i = 0; ; i++) {
        serial_debug("tick %d\n", i);
        doomgeneric_Tick();
    }

    return 0;
}
