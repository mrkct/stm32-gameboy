#ifndef ILI9341_H
#define ILI9341_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

struct ILI9341_Pin_t {
  GPIO_TypeDef *port;
  uint16_t pin;
};

struct ILI9341_t {
  struct ILI9341_Pin_t DATA[8];
  struct ILI9341_Pin_t RST, CS, RS, WR, RD;
};

enum ILI9341_Orientation {
  HORIZONTAL,
  VERTICAL,
  HORIZONTAL_REVERSE,
  VERTICAL_REVERSE
};

int ILI9341_Init(struct ILI9341_t *ili, struct ILI9341_Pin_t D7,
                 struct ILI9341_Pin_t D6, struct ILI9341_Pin_t D5,
                 struct ILI9341_Pin_t D4, struct ILI9341_Pin_t D3,
                 struct ILI9341_Pin_t D2, struct ILI9341_Pin_t D1,
                 struct ILI9341_Pin_t D0, struct ILI9341_Pin_t RST,
                 struct ILI9341_Pin_t CS, struct ILI9341_Pin_t RS,
                 struct ILI9341_Pin_t WR, struct ILI9341_Pin_t RD);

void ILI9341_SendInitializationSequence(struct ILI9341_t *ili);

uint32_t ILI9341_ReadID(struct ILI9341_t *ili);

void ILI9341_DrawFramebufferScaled(struct ILI9341_t *ili,
                                   uint16_t framebuffer[]);

void ILI9341_SetDrawingArea(struct ILI9341_t *ili, uint16_t x1, uint16_t x2,
                            uint16_t y1, uint16_t y2);

void ILI9341_StepProgressBar(struct ILI9341_t *ili);

void ILI9341_SetOrientation(struct ILI9341_t *ili, enum ILI9341_Orientation o);

void ILI9341_FillScreen(struct ILI9341_t *ili, uint16_t fill_color);

static inline uint16_t ILI9341_RgbTo565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

#endif
