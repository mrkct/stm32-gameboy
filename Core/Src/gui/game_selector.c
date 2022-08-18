#include "gui/game_selector.h"
#include "gamepad/gamepad.h"
#include "gui/font.h"
#include "gui/frame.h"
#include <string.h>

#define TRUE 1
#define FALSE 0

static FRESULT find_games (char **games, unsigned short int *num_games,
                           char *path);

void
GameSelectionMenu (struct ILI9341_t *display, struct GameChoice *choice)
{

  // Load the font and the unicode_map.
  PSF_Init ();

  // Build the representation of the frame.
  uint16_t bg = (2 << 15) - 1;
  Frame frame = Frame_New (320, 240, bg);

  char **games = NULL;
  unsigned short int num_games = 0;
  unsigned short int selected = 0;

  find_games (games, &num_games, "");

  if (num_games == 0)
    {
      // @TODO: do actual math for centering
      Frame_AddLine (frame, "No SD card found!", 5, 1, TRUE);
      return;
    }

  for (;;)
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
              if (selected != num_games - 1)
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

      if (Frame_Fits (frame) >= num_games)
        {
          for (int i = 0; i < num_games; i++)
            {
              if (selected == i)
                {
                  Frame_AddLine (frame, games[i], i + 1, 1, TRUE);
                }
              else
                {
                  Frame_AddLine (frame, games[i], i + 1, 1, FALSE);
                }
            }
        }
      else
        {
          // @TODO: Select games to display when the number of available games
          // is greater than can fit on the screen..
        }
    }
}

static FRESULT
find_games (char **games, unsigned short int *num_games, char *path)
{
  FRESULT res;
  DIR dir;
  UINT i;
  static FILINFO fno;

  res = f_opendir (&dir, path);
  if (res == FR_OK)
    {
      for (;;)
        {
          res = f_readdir (&dir, &fno);
          if (res != FR_OK || fno.fname[0] == 0)
            break;
          if (fno.fattrib & AM_DIR)
            {
              i = strlen (path);
              sprintf (&path[i], "/%s", fno.fname);
              res = find_games (games, num_games, path);
              if (res != FR_OK)
                break;
              path[i] = 0;
            }
          else
            {
              const char *ext = strrchr (fno.fname, '.') + 1;
              if (strcmp (ext, "gba") || strcmp (ext, "GBA"))
                {
                  games = realloc (&games, sizeof (char *) * (*num_games + 1));
                  games[*num_games] = fno.fname;
                  *num_games++;
                }
            }
        }
      f_closedir (&dir);
    }

  return res;
}
