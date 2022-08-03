#ifndef EMULATOR_HOOKS_H
#define EMULATOR_HOOKS_H

#include "display/ili9341.h"
#include <time.h>

void StartEmulator(struct ILI9341_t *, uint8_t const *rom, uint8_t *savefile,
                   struct tm current_date_and_time);

#endif
