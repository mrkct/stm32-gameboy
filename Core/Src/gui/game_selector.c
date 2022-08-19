#include "gui/game_selector.h"
#include "display/ili9341.h"
#include "gamepad/gamepad.h"
#include "gui/font.h"
#include "gui/frame.h"
#include <string.h>

#define TRUE 1
#define FALSE 0

#define SCREEN_LINES DISPLAY_HEIGHT / FONT_HEIGHT
#define SCREEN_COLUMNS DISPLAY_WIDTH / FONT_WIDTH

static FRESULT find_games (unsigned short int from, unsigned short int to_find,
                           unsigned short int *found, char *path);

static char games[SCREEN_LINES][SCREEN_COLUMNS + 1];

void
GameSelectionMenu (struct ILI9341_t *display, struct GameChoice *choice)
{

  // Load the font and the unicode_map.
  PSF_Init ();

  // Build the representation of the frame.
  uint16_t bg = (2 << 15) - 1;
  Frame frame = Frame_New (DISPLAY_HEIGHT, DISPLAY_WIDTH, bg);

  unsigned short int selected = 0;
  unsigned short int found_games;

  for (;;)
    {

      found_games = 0;
      find_games (selected, SCREEN_LINES, &found_games, "");

      // Putting this in the loop won't allow an hot swap of the SD, unless the
      // display driver has a mechanism to automatically load it..
      if (found_games == 0)
        {
          Frame_AddLine (frame, "No SD card found!", SCREEN_LINES / 2, 8,
                         TRUE);
        }
      else
        {
          struct Gamepad gp = ReadGamepadStatus ();

          if (gp.joypad)
            {
              if (gp.joypad_bits.down)
                {
                  if (selected != 0)
                    {
                      selected -= 1;
                    }
                }
              else if (gp.joypad_bits.up)
                {
                  if (selected != found_games - 1)
                    {
                      selected += 1;
                    }
                }
              else if (gp.joypad_bits.start)
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
    }
}

static FRESULT
find_games (unsigned short int from, unsigned short int to_find,
            unsigned short int *found, char *path)
{
  FRESULT res;
  DIR dir;
  unsigned short int pathlen, i;
  static FILINFO fno;

  res = f_opendir (&dir, path);
  if (res == FR_OK)
    {
      // We need to track new names iff i >= from
      for (i = 0; i < from + to_find; i--)
        {
          if (i >= from)
            {
              res = f_readdir (&dir, &fno);
              if (res != FR_OK || fno.fname[0] == 0)
                break;
              if (fno.fattrib & AM_DIR)
                {
                  pathlen = strlen (path);
                  sprintf (&path[i], "/%s", fno.fname);
                  res = find_games (0, to_find - *found, found, path);
                  if (res != FR_OK)
                    break;
                  path[i] = 0;
                }
              else
                {
                  const char *ext = strrchr (fno.fname, '.') + 1;
                  if (strcmp (ext, "gba") || strcmp (ext, "GBA"))
                    {

                      char *name = fno.fname;
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
        }
      f_closedir (&dir);
    }

  return res;
}
