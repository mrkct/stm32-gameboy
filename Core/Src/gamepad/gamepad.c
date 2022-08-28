#include "gamepad/gamepad.h"
#include "config.h"


void InitializeGamepadPins() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

#define INIT_BUTTON_AS_INPUT(pin)                                        \
  {                                                                            \
    GPIO_InitStruct.Pin = pin;                                                 \
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;                                \
    GPIO_InitStruct.Pull = GPIO_NOPULL;                                        \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;                               \
    HAL_GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStruct);                                     \
  }
  INIT_BUTTON_AS_INPUT(BUTTON_A_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_B_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_LEFT_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_UP_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_RIGHT_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_DOWN_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_START_PIN);
  INIT_BUTTON_AS_INPUT(BUTTON_SELECT_PIN);

#undef INIT_BUTTON_AS_INPUT
}

struct Gamepad ReadGamepadStatus() {

  struct Gamepad gamepad;

  uint32_t idr = BUTTON_GPIO_PORT->IDR;

#define TEST_BUTTON(button) idr & BUTTON_##button##_PIN ? 0 : 1

  gamepad.joypad_bits.a = TEST_BUTTON(A);
  gamepad.joypad_bits.b = TEST_BUTTON(B);
  gamepad.joypad_bits.start = TEST_BUTTON(START);
  gamepad.joypad_bits.select = TEST_BUTTON(SELECT);
  gamepad.joypad_bits.left = TEST_BUTTON(LEFT);
  gamepad.joypad_bits.up = TEST_BUTTON(UP);
  gamepad.joypad_bits.right = TEST_BUTTON(RIGHT);
  gamepad.joypad_bits.down = TEST_BUTTON(DOWN);

  return gamepad;
}
