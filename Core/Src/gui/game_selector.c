#include "gui/game_selector.h"
#include "config.h"
#include "display/ili9341.h"
#include "gamepad/gamepad.h"
#include "gui/font.h"
#include "gui/frame.h"
#include <stdio.h>
#include <string.h>
#include "gui/sad_gameboy.h"

#define TRUE 1
#define FALSE 0

static FRESULT find_games (unsigned short int, unsigned short int,
                           unsigned short int *, char *);

static FRESULT choose_game (unsigned short int, struct GameChoice *,
                            unsigned short int *, char *, _Bool *);

static char games[SCREEN_LINES][SCREEN_COLUMNS + 1];

void
HaltAndShowErrorScreen(struct ILI9341_t *display, const char *message1, const char *message2)
{
  uint16_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
  Frame frame = Frame_New (SCREEN_WIDTH, SCREEN_HEIGHT, framebuffer, ILI9341_RgbTo565(255, 255, 255));  

  Frame_AddLine(frame, message1, 2, 1, FALSE);
  Frame_AddLine(frame, message2, 4, 1, FALSE);

  const int offy = 55;
  const int offx = (SCREEN_WIDTH - SADGB_WIDTH) / 2;
  for (int y = 0; y < SADGB_HEIGHT; y++) {
    for (int x = 0; x < SADGB_WIDTH; x++) {
      if (SADGB_DATA[y * SADGB_WIDTH + x])
        framebuffer[(offy + y) * SCREEN_WIDTH + (offx + x)] = 0;
    }
  }

  Frame_AddLine(frame, "Please restart the", 12, 1, FALSE);
  Frame_AddLine(frame, "     console", 14, 1, FALSE);

  ILI9341_DrawFramebufferScaled(display, Frame_Draw(frame));

  while(1);
}

void
GameSelectionMenu (struct ILI9341_t *display, struct GameChoice *choice)
{
#if DEBUG_JUMP_TO_GAME
  f_open (&choice->game, "KIRBY_DREAMLAND.gb", FA_READ);
  f_open (&choice->savefile, "KIRBY_DREAMLAND.sav",
          FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
  return;
#endif

  // Load the font and the unicode_map.
  // PSF_Init ();

  // Build the representation of the frame.
  uint16_t bg = (2 << 15) - 1;
  uint16_t buf[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };
  char path[256] = "";
  Frame frame = Frame_New (SCREEN_WIDTH, SCREEN_HEIGHT, buf, bg);

  unsigned short int selected = 0;
  unsigned short int found_games = 0;
  unsigned short int wait_millis = 250;
  unsigned int last_command_millis = HAL_GetTick ();

  for (;;)
    {
      found_games = 0;
      find_games (selected, SCREEN_LINES - 2, &found_games, path);

      // Putting this in the loop won't allow an hot swap of the SD, unless the
      // display driver has a mechanism to automatically load it..
      if (found_games == 0)
        {
          HaltAndShowErrorScreen(display, "No games found in", "    the card");
        }
      else
        {
          struct Gamepad gp = ReadGamepadStatus ();

          if (gp.joypad != (1 << 8) - 1
              && (HAL_GetTick () - last_command_millis) >= wait_millis)
            {

              last_command_millis = HAL_GetTick ();
              if (!gp.joypad_bits.down)
                {
                  if (selected != 0)
                    {
                      selected -= 1;
                    }
                }
              else if (!gp.joypad_bits.up)
                {
                  if (selected != found_games - 1)
                    {
                      selected += 1;
                    }
                }
              else if (!gp.joypad_bits.start)
                {
                  break;
                }
            }

          Frame_Clear (frame, bg);

          for (int i = 0; i < found_games; i++)
            {
              if (i == 0)
                {
                  // The first Line will be the selected game.
                  Frame_AddLine (frame, games[i], i + 1, 1, TRUE);
                }
              else
                {
                  Frame_AddLine (frame, games[i], i + 1, 1, FALSE);
                }
            }
        }

      ILI9341_DrawFramebufferScaled (display, Frame_Draw (frame));
      HAL_Delay (250);
    }

  found_games = 0;
  _Bool found = 0;
  choose_game (selected, choice, &found_games, path, &found);
}

static FRESULT
find_games (unsigned short int from, unsigned short int to_find,
            unsigned short int *found, char *path)
{
  FRESULT res;
  DIR dir;
  unsigned short int pathlen, gb_games_seen = 0;
  static FILINFO fno;

  res = f_opendir (&dir, path);
  if (!res)
    return res;

  while (1)
  {
    res = f_readdir (&dir, &fno);

    // Nothing left to be read
    if (res != FR_OK || fno.fname[0] == 0)
      break;

    if (fno.fattrib & AM_DIR)
      {
        pathlen = strlen (path);
        sprintf (&path[pathlen], "/%s", fno.fname);
        res = find_games (0, to_find - *found, found, path);
        if (res != FR_OK)
          break;
        path[pathlen] = 0;
      }
    else
      {
        const char *ext = strrchr (fno.fname, '.') + 1;
        if (!(strcmp (ext, "gb") || strcmp (ext, "GB")
            || strcmp (ext, "gbc") || strcmp (ext, "GBC")))
          {
            char *name = fno.fname;

            // Save the name only iff it's the "from"-th game or more..
            if (gb_games_seen < from)
              {
                gb_games_seen++;
                continue;
              }
            char *ext_dot = strrchr (name, '.');
            if (ext_dot && (*ext_dot) == '.')
              {
                (*ext_dot) = 0;
              }

            unsigned short int len = strlen (name);
            for (int i = 0; i < SCREEN_COLUMNS; i++)
              {

                if (i < len)
                  {
                    games[*found][i] = name[i];
                  }
                else
                  {
                    games[*found][i] = 0;
                  break;
                  }
              }
            games[*found][SCREEN_COLUMNS] = 0;
            (*found)++;
          }
      }

    if (*found == to_find)
      {
        break;
      }
  }
  f_closedir (&dir);

  return res;
}

static FRESULT
choose_game (unsigned short int selected, struct GameChoice *choice,
             unsigned short int *games_found, char *path, _Bool *found)
{
  FRESULT res;
  DIR dir;
  unsigned short int pathlen;
  static FILINFO fno;

  res = f_opendir (&dir, path);
  if (res == FR_OK)
    {
      while (1)
        {
          res = f_readdir (&dir, &fno);

          // Nothing left to be read
          if (res != FR_OK || fno.fname[0] == 0)
            break;

          if (fno.fattrib & AM_DIR)
            {
              pathlen = strlen (path);
              sprintf (&path[pathlen], "/%s", fno.fname);
              res = choose_game (selected, choice, games_found, path, found);
              if (res != FR_OK)
                break;
              path[pathlen] = 0;
            }
          else
            {
              const char *ext = strrchr (fno.fname, '.') + 1;
              if (!strcmp (ext, "gb") || !strcmp (ext, "GB")
                  || !strcmp (ext, "gbc") || !strcmp (ext, "GBC"))
                {
                  // Save the name only iff it's the "from"-th game or more..
                  if (*games_found != selected)
                    {
                      (*games_found)++;
                      continue;
                    }

                  *found = 1;
                  pathlen = strlen (path);
                  sprintf (&path[pathlen], "/%s", fno.fname);
                  res = f_open (&choice->game, path, FA_READ);

                  if (res != FR_OK)
                    return res;

                  sprintf (&path[pathlen], "/%s.sav", strchr (fno.fname, '.'));
                  res = f_open (&choice->savefile, path, FA_READ);
                  if (res != FR_OK)
                    return res;
                  path[pathlen] = 0;
                }
            }

          if (*found)
            {
              break;
            }
        }
      f_closedir (&dir);
    }

  return res;
}
