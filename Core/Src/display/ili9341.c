#include "display/ili9341.h"
#include "stm32f4xx_hal.h"

#define PIN_LOW(p) HAL_GPIO_WritePin(p.port, p.pin, GPIO_PIN_RESET)
#define PIN_HIGH(p) HAL_GPIO_WritePin(p.port, p.pin, GPIO_PIN_SET)

#define DELAY(x) HAL_Delay(x)

#define RD_ACTIVE(ili) PIN_LOW(ili->RD)
#define RD_IDLE(ili) PIN_HIGH(ili->RD)

#define WR_ACTIVE(ili) PIN_LOW(ili->WR)
#define WR_IDLE(ili) PIN_HIGH(ili->WR)

#define CD_COMMAND(ili) PIN_LOW(ili->RS)
#define CD_DATA(ili) PIN_HIGH(ili->RS)

#define CS_ACTIVE(ili) PIN_LOW(ili->CS)
#define CS_IDLE(ili) // PIN_HIGH(ili->CS)

#define WR_STROBE(ili)                                                         \
  {                                                                            \
    WR_ACTIVE(ili);                                                            \
    WR_IDLE(ili);                                                              \
  }

enum Commands {
  CMD_SOFTWARE_RESET = 0x01,
  CMD_READ_DISPLAY_IDENTIFICATION_INFORMATION = 0x04,
  CMD_SLEEP_OUT = 0x11,
  CMD_DISPLAY_INVERSION_ON = 0x21,
  CMD_DISPLAY_INVERSION_OFF = 0x22,
  CMD_DISPLAY_OFF = 0x28,
  CMD_DISPLAY_ON = 0x29,
  CMD_COLUMN_ADDRESS_SET = 0x2a,
  CMD_PAGE_ADDRESS_SET = 0x2b,
  CMD_MEMORY_WRITE = 0x2c,
  CMD_MEMORY_ACCESS_CONTROL = 0x36,
  CMD_PIXEL_FORMAT_SET = 0x3a,
  CMD_FRAME_RATE_CONTROL = 0xb1,
  CMD_ENTRY_MODE_SET = 0xb7,
  CMD_POWER_CONTROL_1 = 0xc0,
  CMD_POWER_CONTROL_2 = 0xc1,
  CMD_VCOM_CONTROL_1 = 0xc5,
  CMD_VCOM_CONTROL_2 = 0xc7,
  CMD_READ_ID4 = 0xd3
};

enum CommandParams {
  PARAM_FLAG_MEMORY_ACCESS_CONTROL_ROW_ADDRESS_ORDER = 0x80,
  PARAM_FLAG_MEMORY_ACCESS_CONTROL_COLUMN_ADDRESS_ORDER = 0x40,
  PARAM_FLAG_MEMORY_ACCESS_CONTROL_ROW_COLUMN_EXCHANGE = 0x20,
  PARAM_FLAG_MEMORY_ACCESS_CONTROL_ROW_BGR = 0x08,
  PARAM_PIXEL_FORMAT_RGB_16_BITS_PER_PIXEL = 0x50,
  PARAM_PIXEL_FORMAT_BGR_16_BITS_PER_PIXEL = 0x05,
  PARAM_FLAG_ENTRY_MODE_LOW_VOLTAGE_DETECTION_DISABLED = 0x1,
  PARAM_FLAG_ENTRY_MODE_NORMAL_DISPLAY = 0x6
};

inline static void ILI9341_ConfigurePinForOutput(struct ILI9341_Pin_t p) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pin = p.pin;
  HAL_GPIO_Init(p.port, &GPIO_InitStruct);
}

inline static void ILI9341_ConfigurePinForInput(struct ILI9341_Pin_t p) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pin = p.pin;
  HAL_GPIO_Init(p.port, &GPIO_InitStruct);
}

inline static void ILI9341_PrepareDataPinsForReading(struct ILI9341_t *ili) {
  for (int i = 0; i < 8; i++) {
    ILI9341_ConfigurePinForInput(ili->DATA[i]);
  }
}

inline static void ILI9341_PrepareDataPinsForWriting(struct ILI9341_t *ili) {
  for (int i = 0; i < 8; i++) {
    ILI9341_ConfigurePinForOutput(ili->DATA[i]);
  }
}

inline static void ILI9341_WriteToDataPins(struct ILI9341_t *ili, uint8_t b) {
  for (int i = 0; i < 8; i++) {
    if (b & (1 << i))
      PIN_HIGH(ili->DATA[i]);
    else
      PIN_LOW(ili->DATA[i]);
  }
}

inline static uint8_t ILI9341_ReadFromDataPins(struct ILI9341_t *ili) {
  uint8_t b = 0;
  for (int i = 0; i < 8; i++) {
    if (HAL_GPIO_ReadPin(ili->DATA[i].port, ili->DATA[i].pin) == GPIO_PIN_SET)
      b |= 1 << i;
  }

  return b;
}

inline static void ILI9341_WriteCommand(struct ILI9341_t *ili,
                                        uint8_t command) {
  CD_COMMAND(ili);

  WR_ACTIVE(ili);
  ILI9341_WriteToDataPins(ili, command);
  WR_IDLE(ili);
}

inline static void ILI9341_WriteData(struct ILI9341_t *ili, uint8_t data) {
  CD_DATA(ili);

  WR_ACTIVE(ili);
  ILI9341_WriteToDataPins(ili, data);
  WR_IDLE(ili);
}

inline static void ILI9341_WriteCommandWithParameter(struct ILI9341_t *ili,
                                                     uint8_t command,
                                                     uint8_t param1) {
  ILI9341_WriteCommand(ili, command);
  ILI9341_WriteData(ili, param1);
}

inline static void ILI9341_WriteCommandWith2Parameters(struct ILI9341_t *ili,
                                                       uint8_t command,
                                                       uint8_t param1,
                                                       uint8_t param2) {
  ILI9341_WriteCommand(ili, command);
  ILI9341_WriteData(ili, param1);
  ILI9341_WriteData(ili, param2);
}

inline static void ILI9341_WriteCommandWith3Parameters(struct ILI9341_t *ili,
                                                       uint8_t command,
                                                       uint8_t param1,
                                                       uint8_t param2,
                                                       uint8_t param3) {
  ILI9341_WriteCommand(ili, command);
  ILI9341_WriteData(ili, param1);
  ILI9341_WriteData(ili, param2);
  ILI9341_WriteData(ili, param3);
}

inline static void
ILI9341_WriteCommandWith4Parameters(struct ILI9341_t *ili, uint8_t command,
                                    uint8_t param1, uint8_t param2,
                                    uint8_t param3, uint8_t param4) {
  ILI9341_WriteCommand(ili, command);
  ILI9341_WriteData(ili, param1);
  ILI9341_WriteData(ili, param2);
  ILI9341_WriteData(ili, param3);
  ILI9341_WriteData(ili, param4);
}

inline static uint8_t ILI9341_ReadData(struct ILI9341_t *ili) {
  CD_DATA(ili);

  RD_ACTIVE(ili);
  DELAY(5);
  uint8_t temp = ILI9341_ReadFromDataPins(ili);
  RD_IDLE(ili);

  return temp;
}

int ILI9341_Init(struct ILI9341_t *ili, struct ILI9341_Pin_t D7,
                 struct ILI9341_Pin_t D6, struct ILI9341_Pin_t D5,
                 struct ILI9341_Pin_t D4, struct ILI9341_Pin_t D3,
                 struct ILI9341_Pin_t D2, struct ILI9341_Pin_t D1,
                 struct ILI9341_Pin_t D0, struct ILI9341_Pin_t RST,
                 struct ILI9341_Pin_t CS, struct ILI9341_Pin_t RS,
                 struct ILI9341_Pin_t WR, struct ILI9341_Pin_t RD) {
  ili->RST = RST;
  ili->CS = CS;
  ili->RS = RS;
  ili->WR = WR;
  ili->RD = RD;
  ili->DATA[0] = D0;
  ili->DATA[1] = D1;
  ili->DATA[2] = D2;
  ili->DATA[3] = D3;
  ili->DATA[4] = D4;
  ili->DATA[5] = D5;
  ili->DATA[6] = D6;
  ili->DATA[7] = D7;

  ILI9341_ConfigurePinForOutput(ili->RST);
  ILI9341_ConfigurePinForOutput(ili->CS);
  ILI9341_ConfigurePinForOutput(ili->RS);
  ILI9341_ConfigurePinForOutput(ili->WR);
  ILI9341_ConfigurePinForOutput(ili->RD);

  CS_ACTIVE(ili);
  PIN_HIGH(ili->RST);
  WR_IDLE(ili);
  RD_IDLE(ili);
  CD_COMMAND(ili);

  // Hardware Reset
  PIN_HIGH(ili->RST);
  DELAY(50);
  PIN_LOW(ili->RST);
  DELAY(150);
  PIN_HIGH(ili->RST);
  DELAY(200);

  CS_IDLE(ili);

  return 0;
}

void ILI9341_SendInitializationSequence(struct ILI9341_t *ili) {
  ILI9341_PrepareDataPinsForWriting(ili);

  // Data transfer sync
  {
    CS_ACTIVE(ili);
    CD_COMMAND(ili);
    ILI9341_WriteData(ili, 0x00);
    for (int i = 0; i < 3; i++) {
      WR_ACTIVE(ili);
      WR_IDLE(ili);
    }
    CS_IDLE(ili);
  }

  CS_ACTIVE(ili);

  ILI9341_WriteCommand(ili, CMD_SOFTWARE_RESET);
  DELAY(150);
  ILI9341_WriteCommand(ili, CMD_DISPLAY_OFF);

  ILI9341_WriteCommandWithParameter(ili, CMD_POWER_CONTROL_1, 0x23);
  ILI9341_WriteCommandWithParameter(ili, CMD_POWER_CONTROL_2, 0x10);
  ILI9341_WriteCommandWith2Parameters(ili, CMD_VCOM_CONTROL_1, 0x2B, 0x2B);
  ILI9341_WriteCommandWithParameter(ili, CMD_VCOM_CONTROL_2, 0xC0);
  ILI9341_WriteCommandWithParameter(
      ili, CMD_MEMORY_ACCESS_CONTROL,
      PARAM_FLAG_MEMORY_ACCESS_CONTROL_ROW_ADDRESS_ORDER |
          PARAM_FLAG_MEMORY_ACCESS_CONTROL_ROW_BGR);
  ILI9341_WriteCommandWithParameter(
      ili, CMD_PIXEL_FORMAT_SET,
      PARAM_PIXEL_FORMAT_RGB_16_BITS_PER_PIXEL |
          PARAM_PIXEL_FORMAT_BGR_16_BITS_PER_PIXEL);
  ILI9341_WriteCommandWith2Parameters(ili, CMD_FRAME_RATE_CONTROL, 0x00, 0x1b);

  ILI9341_WriteCommandWithParameter(
      ili, CMD_ENTRY_MODE_SET,
      PARAM_FLAG_ENTRY_MODE_LOW_VOLTAGE_DETECTION_DISABLED |
          PARAM_FLAG_ENTRY_MODE_NORMAL_DISPLAY);

  ILI9341_WriteCommand(ili, CMD_SLEEP_OUT);
  DELAY(150);
  ILI9341_WriteCommand(ili, CMD_DISPLAY_ON);
  DELAY(500);

  CS_IDLE(ili);
}

void ILI9341_SetDrawingArea(struct ILI9341_t *ili, uint16_t x1, uint16_t x2,
                            uint16_t y1, uint16_t y2) {
  ILI9341_WriteCommand(ili, CMD_COLUMN_ADDRESS_SET);

  ILI9341_WriteData(ili, x1 >> 8);
  ILI9341_WriteData(ili, x1 & 0xff);

  ILI9341_WriteData(ili, x2 >> 8);
  ILI9341_WriteData(ili, x2 & 0xff);

  ILI9341_WriteCommand(ili, CMD_PAGE_ADDRESS_SET);

  ILI9341_WriteData(ili, y1 >> 8);
  ILI9341_WriteData(ili, y1 & 0xff);

  ILI9341_WriteData(ili, y2 >> 8);
  ILI9341_WriteData(ili, y2 & 0xff);
}

void ILI9341_DrawFramebufferScaled(struct ILI9341_t *ili,
                                   uint16_t framebuffer[]) {

  // Data transfer sync
  {
    CD_COMMAND(ili);
    ILI9341_WriteData(ili, 0x00);
    for (int i = 0; i < 3; i++) {
      WR_ACTIVE(ili);
      WR_IDLE(ili);
    }
  }

// WARN: This assumes that D0 -> PA0, D1 -> PA1, D2 -> PA2 ... D7 -> PA7
#define WRITE_DATA_DIRECT_TO_DATA_PINS(data)                                   \
  GPIOA->ODR = ((uint16_t)(data)) | ili->RST.pin | ili->RD.pin | ili->RS.pin

#define WRITE_COMMAND_DIRECT_TO_DATA_PINS(data)                                \
  GPIOA->ODR = ((uint16_t)(data)) | ili->RST.pin | ili->RD.pin

#define WR_IDLE_FAST(ili) GPIOA->BSRR = ili->WR.pin

#define COLOR_AVERAGE(x, y) (uint16_t)(((uint32_t)(x) + (uint32_t)(y)) / 2)

#define WRITE_PIXEL(p)                                                         \
  WRITE_DATA_DIRECT_TO_DATA_PINS((uint8_t)((p) >> 8));                         \
  WR_IDLE_FAST(ili);                                                           \
  WRITE_DATA_DIRECT_TO_DATA_PINS((uint8_t)(p));                                \
  WR_IDLE_FAST(ili);

  const int FB_WIDTH = 160;
  const int FB_HEIGHT = 144;
  const int SCALED_WIDTH = 240;
  const int SCALED_HEIGHT = 216;

  {
    ILI9341_WriteCommand(ili, CMD_COLUMN_ADDRESS_SET);

    ILI9341_WriteData(ili, 0);
    ILI9341_WriteData(ili, 0);

    ILI9341_WriteData(ili, (SCALED_WIDTH - 1) >> 8);
    ILI9341_WriteData(ili, (SCALED_WIDTH - 1) & 0xff);

    ILI9341_WriteCommand(ili, CMD_PAGE_ADDRESS_SET);

    ILI9341_WriteData(ili, 0);
    ILI9341_WriteData(ili, 0);

    ILI9341_WriteData(ili, (SCALED_HEIGHT - 1) >> 8);
    ILI9341_WriteData(ili, (SCALED_HEIGHT - 1) & 0xff);
  }

  ILI9341_WriteCommand(ili, CMD_MEMORY_WRITE);

  int i = 0;
  HAL_Delay(1);
  while (i < FB_WIDTH * FB_HEIGHT) {
    for (int j = i; j < i + FB_WIDTH; j += 2) {
      WRITE_PIXEL(framebuffer[j]);
      WRITE_PIXEL(framebuffer[j + 1]);

      // TODO: Calculate average color with the next pixel
      WRITE_PIXEL(framebuffer[j + 1]);
    }
    i += FB_WIDTH;

    for (int j = i; j < i + FB_WIDTH; j += 2) {
      WRITE_PIXEL(framebuffer[j]);
      WRITE_PIXEL(framebuffer[j + 1]);

      // TODO: Calculate average color with the next pixel
      WRITE_PIXEL(framebuffer[j + 1]);
    }
    i += FB_WIDTH;

    // Approximated row for scaling
    for (int j = i; j < i + FB_WIDTH; j += 2) {
      WRITE_PIXEL(framebuffer[j]);
      WRITE_PIXEL(framebuffer[j + 1]);

      // TODO: Calculate average color between top-left, top-right, bottom-left,
      // bottom-right pixels
      WRITE_PIXEL(framebuffer[j + 1]);
    }
    // Don't step 'i' here because we haven't printed a "real" row!
  }
}

uint32_t ILI9341_ReadID(struct ILI9341_t *ili) {
  CS_ACTIVE(ili);

  ILI9341_PrepareDataPinsForWriting(ili);
  ILI9341_WriteCommand(ili, CMD_READ_ID4);

  ILI9341_PrepareDataPinsForReading(ili);

  uint32_t r = ILI9341_ReadData(ili);
  r <<= 8;
  r |= ILI9341_ReadData(ili);
  r <<= 8;
  r |= ILI9341_ReadData(ili);
  r <<= 8;
  r |= ILI9341_ReadData(ili);

  ILI9341_PrepareDataPinsForWriting(ili);

  CS_IDLE(ili);

  return r;
}

void ILI9341_SetOrientation(struct ILI9341_t *ili, enum ILI9341_Orientation o) {
  uint8_t b = PARAM_FLAG_MEMORY_ACCESS_CONTROL_ROW_BGR;
  switch (o) {
  case VERTICAL:
    b |= 0;
    break;
  case VERTICAL_REVERSE:
    b |= PARAM_FLAG_MEMORY_ACCESS_CONTROL_COLUMN_ADDRESS_ORDER;
    break;
  case HORIZONTAL:
    b |= PARAM_FLAG_MEMORY_ACCESS_CONTROL_ROW_COLUMN_EXCHANGE;
    break;
  case HORIZONTAL_REVERSE:
    b = PARAM_FLAG_MEMORY_ACCESS_CONTROL_ROW_COLUMN_EXCHANGE |
        PARAM_FLAG_MEMORY_ACCESS_CONTROL_COLUMN_ADDRESS_ORDER |
        PARAM_FLAG_MEMORY_ACCESS_CONTROL_ROW_ADDRESS_ORDER;
    break;
  }

  CS_ACTIVE(ili);
  ILI9341_WriteCommandWithParameter(ili, CMD_MEMORY_ACCESS_CONTROL, b);
  CS_IDLE(ili);
}

void ILI9341_FillScreen(struct ILI9341_t *ili, uint16_t fill_color) {
  ILI9341_SetDrawingArea(ili, 0, DISPLAY_WIDTH - 1, 0, DISPLAY_HEIGHT - 1);
  ILI9341_WriteCommand(ili, CMD_MEMORY_WRITE);

  for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
    ILI9341_WriteData(ili, fill_color >> 8);
    ILI9341_WriteData(ili, fill_color);
  }
}
