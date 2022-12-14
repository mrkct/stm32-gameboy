#ifndef GAME_SELECTOR_H
#define GAME_SELECTOR_H

#include "display/ili9341.h"
#include "fatfs.h"

struct GameChoice
{
  FIL game;
  FIL savefile;
};

void GameSelectionMenu (struct ILI9341_t *display, struct GameChoice *);
void HaltAndShowErrorScreen (struct ILI9341_t *display, const char *,
                             const char *);

#endif
