#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f4xx_hal.h"

#define DEBUG_JUMP_TO_GAME 0
#define DEBUG_SHOW_FPS     0

// Display pins
// WARNING: The function to move the framebuffer to the display
// is hardcoded to use these exact pins for performance reasons
#define LCD_D0 .port = GPIOA, .pin = GPIO_PIN_0
#define LCD_D1 .port = GPIOA, .pin = GPIO_PIN_1
#define LCD_D2 .port = GPIOA, .pin = GPIO_PIN_2
#define LCD_D3 .port = GPIOA, .pin = GPIO_PIN_3
#define LCD_D4 .port = GPIOA, .pin = GPIO_PIN_4
#define LCD_D5 .port = GPIOA, .pin = GPIO_PIN_5
#define LCD_D6 .port = GPIOA, .pin = GPIO_PIN_6
#define LCD_D7 .port = GPIOA, .pin = GPIO_PIN_7

#define LCD_RD .port = GPIOA, .pin = GPIO_PIN_8
#define LCD_WR .port = GPIOA, .pin = GPIO_PIN_9
#define LCD_RS .port = GPIOA, .pin = GPIO_PIN_10
#define LCD_CS .port = GPIOA, .pin = GPIO_PIN_11
#define LCD_RESET .port = GPIOA, .pin = GPIO_PIN_12

// Pins used for the buttons
#define BUTTON_GPIO_PORT GPIOB

#define BUTTON_B_PIN GPIO_PIN_7
#define BUTTON_A_PIN GPIO_PIN_6
#define BUTTON_SELECT_PIN GPIO_PIN_5
#define BUTTON_START_PIN GPIO_PIN_4
#define BUTTON_RIGHT_PIN GPIO_PIN_3
#define BUTTON_LEFT_PIN GPIO_PIN_2
#define BUTTON_DOWN_PIN GPIO_PIN_1
#define BUTTON_UP_PIN GPIO_PIN_0

// Stuff used for the SD Card
// Note that these are used by the external SD card driver,
// I didn't choose the names
#define SD_SPI_HANDLE hspi2
// ^This implies PB10 -> SCK, PB14 -> MISO/DO, PB15 -> MOSI/DI
#define SD_CS_GPIO_Port GPIOB
#define SD_CS_Pin GPIO_PIN_8

#define SECTOR_SIZE 512
#define SECTOR_CACHE 40

#endif
