#ifndef STM32GB_FONT_H
#define STM32GB_FONT_H

#include <stdint.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 8

typedef struct
{
  uint32_t magic;
  uint32_t version;
  uint32_t headersize;
  uint32_t has_unicode_table;
  uint32_t num_glyphs;
  uint32_t bytes_per_glyph;
  uint32_t height;
  uint32_t width;
} PSF_font;

extern unsigned char font_bytes[];
extern unsigned int font_bytes_len;

// Returns the unicode map or NULL
//uint16_t *PSF_Init ();

#endif
