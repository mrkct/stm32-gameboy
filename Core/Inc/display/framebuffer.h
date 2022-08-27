#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define BACKGROUND 0xffff
#define FOREGROUND 0x0000

extern uint16_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

#endif
