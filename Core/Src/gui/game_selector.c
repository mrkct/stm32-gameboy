#include "gui/game_selector.h"
#include "game_selector_impl.h"
#include "gamepad/gamepad.h"
#include <stdio.h>
#include <string.h>

#define LINES_PADDING 2
#define COLS_PADDING 2
#define GAMES_PER_PAGE (SCREEN_LINES - LINES_PADDING)
#define MAX_GAME_LEN (SCREEN_COLUMNS - COLS_PADDING)

static char games[GAMES_PER_PAGE][MAX_GAME_LEN + 1] = { 0 };
static const uint16_t bg = 0xffff, fg = 0;
static uint16_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT]
    = { [0 ...(SCREEN_WIDTH * SCREEN_HEIGHT) - 1] = bg };

static const unsigned short int FindGames ();
static void LoadGamesPage (const unsigned short int selected_game,
                           const unsigned short int avail_games);
static void AddLine (uint16_t *buffer, const char *name,
                     const unsigned short int line, unsigned short int col,
                     const unsigned short int selected);
static unsigned short int GetGame (struct GameChoice *choice,
                                   const unsigned short int selected);
void
GameSelectionMenu (struct ILI9341_t *display, struct GameChoice *choice)
{
#if DEBUG_JUMP_TO_GAME
  f_open (&choice->game, "KIRBY_DREAMLAND.gb", FA_READ);
  f_open (&choice->savefile, "KIRBY_DREAMLAND.sav",
          FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
  return;
#endif

  unsigned short int selected_game = 0;
  const unsigned short int wait_millis = 250;
  unsigned short int last_command_millis = HAL_GetTick ();
  const unsigned short int avail_games = FindGames ();

  if (avail_games == 0)
    {
      HaltAndShowErrorScreen (display, "  No games found", "   in the card!");
    }

  while (1)
    {

      struct Gamepad gp = ReadGamepadStatus ();

      if (gp.joypad != (1 << 8) - 1
          && (HAL_GetTick () - last_command_millis) >= wait_millis)
        {

          last_command_millis = HAL_GetTick ();
          if (!gp.joypad_bits.down)
            {
              if (selected_game != 0)
                {
                  selected_game -= 1;
                }
            }
          else if (!gp.joypad_bits.up)
            {
              if (selected_game != avail_games)
                {
                  selected_game += 1;
                }
            }
          else if (!gp.joypad_bits.start)
            {
              break;
            }
        }

      LoadGamesPage (selected_game, avail_games);

      for (unsigned short int i = 0; i < GAMES_PER_PAGE; i++)
        {
          AddLine (
              buffer, games[i], i + (LINES_PADDING / 2), COLS_PADDING / 2,
              selected_game
                      == ((selected_game / GAMES_PER_PAGE) * GAMES_PER_PAGE
                          + i)
                  ? 1
                  : 0);
        }

      ILI9341_DrawFramebufferScaled (display, buffer);
      HAL_Delay (250);
    }

  if (!GetGame (choice, selected_game))
    {
      HaltAndShowErrorScreen (display, "        uh-oh!",
                              "something wrong happened!");
    }
}

static const unsigned short int
_FindGamesImpl (char *path)
{
  FRESULT res;
  DIR dir;
  size_t pathlen = 0;
  unsigned short int games_seen = 0;
  static FILINFO fno;

  res = f_opendir (&dir, path);
  if (res != FR_OK)
    return 0;

  while (1)
    {
      res = f_readdir (&dir, &fno);

      // Nothing left to read
      if (res != FR_OK || fno.fname[0] == 0)
        break;

      if (fno.fattrib & AM_DIR)
        {
          pathlen = strlen (path);
          sprintf (&path[pathlen], "/%s", fno.fname);
          games_seen += _FindGamesImpl (path);
          path[pathlen] = 0;
        }
      else
        {
          const char *dot = strrchr (fno.fname, '.');
          if (!strcmp (dot, ".gb") || !strcmp (dot, ".GB")
              || !strcmp (dot, ".gbc") || !strcmp (dot, ".GBC"))
            {
              games_seen++;
            }
        }
    }

  return games_seen;
}

static const unsigned short int
FindGames ()
{
  char path[512] = "";

  return _FindGamesImpl (path);
}

static void
_LoadGamesPageImpl (char *path, const unsigned short int from,
                    unsigned short int *count, unsigned short int max,
                    const unsigned short int avail_games)
{
  FRESULT res;
  DIR dir;
  size_t len = 0;
  static FILINFO fno;

  res = f_opendir (&dir, path);
  if (res != FR_OK)
    return;

  while (1)
    {
      if (*count >= max)
        {
          break;
        }

      res = f_readdir (&dir, &fno);

      // Nothing left to read
      if (res != FR_OK || fno.fname[0] == 0)
        break;

      if (fno.fattrib & AM_DIR)
        {
          len = strlen (path);
          sprintf (&path[len], "/%s", fno.fname);
          _LoadGamesPageImpl (path, from, count, max, avail_games);
          path[len] = 0;
        }
      else
        {
          char *dot = strrchr (fno.fname, '.');

          if (!dot)
            {
              continue;
            }

          if (!strcmp (dot, ".gb") || !strcmp (dot, ".GB")
              || !strcmp (dot, ".gbc") || !strcmp (dot, ".GBC"))
            {
              if (*count < from)
                {
                  (*count)++;
                  continue;
                }
              (*dot) = 0;
              len = strlen (fno.fname);
              for (unsigned short int i = 0; i < MAX_GAME_LEN; i++)
                {
                  if (i < len)
                    {
                      games[(*count) % GAMES_PER_PAGE][i] = fno.fname[i];
                    }
                  else
                    {
                      games[(*count) % GAMES_PER_PAGE][i] = 0;
                      break;
                    }
                }
              games[(*count) % GAMES_PER_PAGE][MAX_GAME_LEN] = 0;
              (*count)++;
            }
        }
    }

  if ((*count) < max && (*count) >= avail_games)
    {
      unsigned short int to_fill = max - *count;
      for (unsigned short int i = 0; i < to_fill; i++)
        {
          games[((*count) + i) % GAMES_PER_PAGE][0] = 0;
        }
    }
  f_closedir (&dir);
}

static void
LoadGamesPage (const unsigned short int selected_game,
               const unsigned short int avail_games)
{
  char path[512] = "";
  unsigned short int count = 0;
  unsigned short int page = (selected_game / GAMES_PER_PAGE);
  unsigned short int next_max = (page + 1) * GAMES_PER_PAGE;
  _LoadGamesPageImpl (path, page * GAMES_PER_PAGE, &count, next_max,
                      avail_games);
}

static void
PutChar (uint16_t *buffer, char c, unsigned short int line,
         unsigned short int col, unsigned int bg, unsigned int fg)
{

  PSF_font *font = (PSF_font *)font_bytes;

  unsigned char *glyph
      = (unsigned char *)&font_bytes + font->headersize
        + (c > 0 && c < font->num_glyphs ? c : 0) * font->bytes_per_glyph;

  int offs = (line * font->height * SCREEN_WIDTH) + (col * font->width);

  int x, y, mask;
  for (y = 0; y < font->height; y++)
    {
      mask = 1 << (font->width - 1);
      for (x = 0; x < font->width; x++)
        {
          uint16_t color = (*glyph & mask) ? fg : bg;

          if (offs + x >= SCREEN_WIDTH * SCREEN_HEIGHT)
            {
              break;
            }

          buffer[offs + x] = color;
          mask >>= 1;
        }
      glyph += 1;
      offs += SCREEN_WIDTH;
    }
}

static void
AddLine (uint16_t *buffer, const char *name, const unsigned short int line,
         unsigned short int col, const unsigned short int selected)
{

  if (line >= SCREEN_LINES)
    {
      return;
    }

  int name_len = strlen (name);

  if (selected)
    {
      PutChar (buffer, '>', line, col, bg, fg);
      col++;
    }

  for (int i = 0; (col + i) < MAX_GAME_LEN; i++)
    {
      PutChar (buffer, (i < name_len) ? name[i] : ' ', line, col + i, bg, fg);
    }
}

static unsigned short int
_GetGameImpl (char *path, struct GameChoice *choice, unsigned short int *count,
              const unsigned short int selected)
{
  FRESULT res;
  DIR dir;
  size_t len = 0;
  static FILINFO fno;

  res = f_opendir (&dir, path);
  if (res != FR_OK)
    return 0;

  while (1)
    {
      res = f_readdir (&dir, &fno);

      // Nothing left to read
      if (res != FR_OK || fno.fname[0] == 0)
        break;

      if (fno.fattrib & AM_DIR)
        {
          len = strlen (path);
          sprintf (&path[len], "/%s", fno.fname);
          if (_GetGameImpl (path, choice, count, selected))
            return 1;
          path[len] = 0;
        }
      else
        {
          char *dot = strrchr (fno.fname, '.');

          if (!dot)
            {
              continue;
            }

          if (!strcmp (dot, ".gb") || !strcmp (dot, ".GB")
              || !strcmp (dot, ".gbc") || !strcmp (dot, ".GBC"))
            {
              if (*count == selected)
                {
                  len = strlen (path);
                  sprintf (&path[len], "/%s", fno.fname);
                  res = f_open (&choice->game, path, FA_READ);

                  if (res != FR_OK)
                    return 0;

                  (*dot) = 0;

                  sprintf (&path[len], "/%s.sav", fno.fname);
                  res = f_open (&choice->savefile, path, FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
                  if (res != FR_OK)
                    return 0;
                  return 1;
                }
              else
                {
                  (*count)++;
                }
            }
        }
    }
  f_closedir (&dir);
  return 0;
}

static unsigned short int
GetGame (struct GameChoice *choice, const unsigned short int selected)
{
  char path[512] = "";
  unsigned short int count = 0;
  return _GetGameImpl (path, choice, &count, selected);
}

void
HaltAndShowErrorScreen (struct ILI9341_t *display, const char *message1,
                        const char *message2)
{
  AddLine (buffer, message1, 2, 1, 0);
  AddLine (buffer, message2, 4, 1, 0);
  const int offy = 55;
  const int offx = (SCREEN_WIDTH - SADGB_WIDTH) / 2;
  for (int y = 0; y < SADGB_HEIGHT; y++)
    {
      for (int x = 0; x < SADGB_WIDTH; x++)
        {
          if (SADGB_DATA[y * SADGB_WIDTH + x])
            buffer[(offy + y) * SCREEN_WIDTH + (offx + x)] = 0;
        }
    }

  AddLine (buffer, "  Please restart", 12, 1, 0);
  AddLine (buffer, "    the console", 14, 1, 0);

  ILI9341_DrawFramebufferScaled (display, buffer);

  while (1)
    ;
}
