#ifndef EMULATOR_HOOKS_H
#define EMULATOR_HOOKS_H

#include "display/ili9341.h"

void StartEmulator(struct ILI9341_t*, uint8_t *rom, uint8_t *savefile);

#endif
