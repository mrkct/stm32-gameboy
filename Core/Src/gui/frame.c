#include "gui/frame.h"
#include "display/ili9341.h"
#include "gui/font.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// static uint16_t *unicode_map;

struct FrameImp
{
  unsigned short int width;
  unsigned short int height;
  uint16_t *buffer;
};

static struct FrameImp _globl_frame;

Frame
Frame_New (unsigned short int width, unsigned short int height,
           uint16_t *buffer, uint16_t bg)
{
  //  if (unicode_map == NULL)
  //    {
  //      unicode_map = PSF_Init ();
  //    }
  _globl_frame.width = width;
  _globl_frame.height = height;
  _globl_frame.buffer = buffer;
  for (unsigned int i = 0; i < width * height; i++)
    {
      _globl_frame.buffer[i] = bg;
    }
  return &_globl_frame;
}

void
Frame_Clear (Frame frame, uint16_t bg)
{
  for (unsigned int i = 0; i < frame->width * frame->height; i++)
    {
      frame->buffer[i] = bg;
    }
}

unsigned short int
Frame_Fits (Frame frame)
{

  PSF_font *font = (PSF_font *)font_bytes;
  return frame->height / font->height;
}

static void
frame_putchar (Frame frame, char c, unsigned short int line,
               unsigned short int col, unsigned int bg, unsigned int fg)
{

#ifdef DEBUG
  printf ("print char '%c'\n", c);
#endif

  PSF_font *font = (PSF_font *)font_bytes;

  //  if (unicode_map != NULL)
  //    {
  //      c = unicode_map[c];
  //    }
  unsigned char *glyph
      = (unsigned char *)&font_bytes + font->headersize
        + (c > 0 && c < font->num_glyphs ? c : 0) * font->bytes_per_glyph;
#ifdef DEBUG
  printf ("printing char '%c' with glyph@%p\n", c, glyph);
#endif

  int offs = (line * font->height * frame->width) + (col * font->width);

  int x, y, mask;
  for (y = 0; y < font->height; y++)
    {
      mask = 1 << (font->width - 1);
      for (x = 0; x < font->width; x++)
        {
          uint16_t color = (*glyph & mask) ? fg : bg;
          frame->buffer[offs + x] = color;
#ifdef DEBUG
          printf ("char: '%c', mask: %d, x: %d, offs+x:%d, fb[offs+x]: %d, "
                  "color: %d\n",
                  c, mask, x, offs + x, frame->buffer[offs + x], color);
#endif
          mask >>= 1;
        }
      glyph += 1;
      offs += frame->width;
    }
}

void
Frame_AddLine (Frame frame, char *name, unsigned short int line,
               unsigned short int col, _Bool selected)
{
#ifdef DEBUG
  printf ("frame %p adding name %s in line %d (selected: %d)\n", frame, name,
          line, selected);
#endif

  int name_len = strlen (name);
  if (selected)
    {
      frame_putchar (frame, '>', line, col, (2 << 15) - 1, 0);
      col++;
    }
  int to_print = (col + name_len > (SCREEN_COLUMNS - 1)) ? (SCREEN_COLUMNS - 1)
                                                         : name_len;
  for (int i = 0; i < to_print; i++)
    {
      frame_putchar (frame, name[i], line, col + i, (2 << 15) - 1, 0);
    }
}

uint16_t *
Frame_Draw (Frame frame)
{
  return frame->buffer;
}

void
Frame_Delete (Frame frame)
{
  free (frame->buffer);
  free (frame);
}
