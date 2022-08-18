#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <stdint.h>

struct Gamepad
{
  union
  {
    struct
    {
      unsigned a : 1;
      unsigned b : 1;
      unsigned select : 1;
      unsigned start : 1;
      unsigned right : 1;
      unsigned left : 1;
      unsigned up : 1;
      unsigned down : 1;
    } joypad_bits;
    uint8_t joypad;
  };
};

void InitializeGamepadPins ();
struct Gamepad ReadGamepadStatus ();

#endif
