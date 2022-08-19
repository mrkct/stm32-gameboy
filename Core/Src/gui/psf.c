#include "font.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

uint16_t *
PSF_Init()
{
#ifdef DEBUG
  printf ("Initializing psf font...\n");
#endif
  PSF_font *font = (PSF_font *)font_bytes;
#ifdef DEBUG
  printf (" magic: %d, version: %d, headersize: %d, has_unicode_table: %d, "
          "num_glyphs: %d, bytes_per_glyph: %d, height: %d, width: %d",
          font->magic, font->version, font->headersize,
          font->has_unicode_table, font->num_glyphs, font->bytes_per_glyph,
          font->height, font->width);
#endif

  unsigned char *p
      = (unsigned char *)(font_bytes + font->headersize
                          + font->num_glyphs * font->bytes_per_glyph);
  uint16_t *unicode_map = calloc (USHRT_MAX, 2);
  uint16_t glyph = 0;
  while (p <= (font_bytes + font_bytes_len))
    {
      uint16_t uc = (uint16_t)(p[0]);
      if (uc == 0xFF)
        {
          glyph++;
          p++;
          continue;
        }
      else if (uc & 128)
        {
          /* UTF-8 to unicode */
          if ((uc & 32) == 0)
            {
              uc = ((p[0] & 0x1F) << 6) + (p[1] & 0x3F);
              p++;
            }
          else if ((uc & 16) == 0)
            {
              uc = ((((p[0] & 0xF) << 6) + (p[1] & 0x3F)) << 6)
                   + (p[2] & 0x3F);
              p += 2;
            }
          else if ((uc & 8) == 0)
            {
              uc = ((((((p[0] & 0x7) << 6) + (p[1] & 0x3F)) << 6)
                     + (p[2] & 0x3F))
                    << 6)
                   + (p[3] & 0x3F);
              p += 3;
            }
          else
            uc = 0;
        }

#ifdef DEBUG
      printf ("mapping uc %c to glyph %d\n", uc, glyph);
#endif
      unicode_map[uc] = glyph;
      p++;
    }
  return unicode_map;
}
