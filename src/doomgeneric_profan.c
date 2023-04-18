//doomgeneric for cross-platform development library 'Simple DirectMedia Layer'

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <syscall.h>

#define KEYQUEUE_SIZE 16


static unsigned char convertToDoomKey(unsigned int key) {
  return 0;
}

static void addKeyToQueue(int pressed, unsigned int keyCode) {
  return;
}

static void handleKeyInput() {
  return;
}


void DG_Init() {
  return;
}

void DG_DrawFrame() {
  return;
}

void DG_SleepMs(uint32_t ms) {
  return;
}

uint32_t DG_GetTicksMs() {
  return c_timer_get_ms();
}

int DG_GetKey(int* pressed, unsigned char* doomKey) {
  return 0;
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
