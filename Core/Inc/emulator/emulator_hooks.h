#ifndef EMULATOR_HOOKS_H
#define EMULATOR_HOOKS_H

#include "display/ili9341.h"
#include "gui/game_selector.h"
#include <time.h>

void StartEmulator(struct ILI9341_t *, struct GameChoice *choice,
                   struct tm current_date_and_time);

#endif
