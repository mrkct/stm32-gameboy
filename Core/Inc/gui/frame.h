#ifndef STM32GB_GUI_FRAME
#define STM32GB_GUI_FRAME

#include <stdbool.h>
#include <stdint.h>

#include "gui/font.h"
#define SCREEN_HEIGHT 144
#define SCREEN_WIDTH 160

#define SCREEN_LINES (SCREEN_HEIGHT / FONT_HEIGHT)
#define SCREEN_COLUMNS (SCREEN_WIDTH / FONT_WIDTH)

struct FrameImp;
typedef struct FrameImp *Frame;

Frame Frame_New (unsigned short int width, unsigned short int height,
                 uint16_t *buffer, uint16_t bg);

void Frame_Clear (Frame frame, uint16_t bg);

unsigned short int Frame_Fits (Frame frame);

void Frame_AddLine (Frame frame, const char *name, unsigned short int line,
                    unsigned short int col, _Bool selected);

uint16_t *Frame_Draw (Frame frame);
void Frame_Delete (Frame frame);

#endif
