#include "gui/game_selector.h"

void GameSelectionMenu(struct ILI9341_t *display, struct GameChoice *choice) {
  FRESULT fres;

  fres = f_open(&choice->game, "POKEMON_GOLD.gbc", FA_READ);
  if (fres != FR_OK) {
    while (1)
      ;
  }

  fres = f_open(&choice->savefile, "POKEMON_GOLD.sav",
                FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
  if (fres != FR_OK) {
    while (1)
      ;
  }
}
