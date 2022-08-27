#include "display/framebuffer.h"


uint16_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT] = { [0 ...(SCREEN_WIDTH * SCREEN_HEIGHT) - 1] = BACKGROUND };
