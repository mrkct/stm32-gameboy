#include "gamepad/gamepad.h"
#include "config.h"


void InitializeGamepadPins() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

#define INIT_BUTTON_AS_INPUT(port, pin)                                        \
  {                                                                            \
    GPIO_InitStruct.Pin = pin;                                                 \
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;                                \
    GPIO_InitStruct.Pull = GPIO_NOPULL;                                        \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;                               \
    HAL_GPIO_Init(port, &GPIO_InitStruct);                                     \
  }
  INIT_BUTTON_AS_INPUT(BUTTON_A_PORT, BUTTON_A_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_B_PORT, BUTTON_B_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_UP_PORT, BUTTON_UP_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_DOWN_PORT, BUTTON_DOWN_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_START_PORT, BUTTON_START_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_SELECT_PORT, BUTTON_SELECT_PIN);

#undef INIT_BUTTON_AS_INPUT
}

struct Gamepad ReadGamepadStatus() {
// 1=button is up, 0=button is down
#define READ_BUTTON(b)                                                         \
  HAL_GPIO_ReadPin(b##_PORT, b##_PIN) == GPIO_PIN_SET ? 0 : 1

  struct Gamepad gamepad;

  gamepad.joypad_bits.a = READ_BUTTON(BUTTON_A);
  gamepad.joypad_bits.b = READ_BUTTON(BUTTON_B);
  gamepad.joypad_bits.start = READ_BUTTON(BUTTON_START);
  gamepad.joypad_bits.select = READ_BUTTON(BUTTON_SELECT);
  gamepad.joypad_bits.left = READ_BUTTON(BUTTON_LEFT);
  gamepad.joypad_bits.up = READ_BUTTON(BUTTON_UP);
  gamepad.joypad_bits.right = READ_BUTTON(BUTTON_RIGHT);
  gamepad.joypad_bits.down = READ_BUTTON(BUTTON_DOWN);

  return gamepad;
}
