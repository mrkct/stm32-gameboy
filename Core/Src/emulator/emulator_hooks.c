#include "emulator/emulator_hooks.h"
#include "emulator/peanut_gb.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

struct priv_t {
  /* Pointer to allocated memory holding GB file. */
  uint8_t const *rom;
  /* Pointer to allocated memory holding save file. */
  uint8_t *cart_ram;

  /* Colour palette for each BG, OBJ0, and OBJ1. */
  uint16_t selected_palette[3][4];
  uint16_t fb[LCD_HEIGHT * LCD_WIDTH];
};

static uint8_t Hook_ReadRom(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t *const p = gb->direct.priv;
  return p->rom[addr];
}

static uint8_t Hook_ReadCartridgeRam(struct gb_s *gb,
                                     const uint_fast32_t addr) {
  const struct priv_t *const p = gb->direct.priv;
  return p->cart_ram[addr];
}

void Hook_WriteCartridgeRam(struct gb_s *gb, const uint_fast32_t addr,
                            const uint8_t val) {
  const struct priv_t *const p = gb->direct.priv;
  p->cart_ram[addr] = val;
}

void Hook_ReportEmulationError(struct gb_s *gb, const enum gb_error_e gb_err,
                               const uint16_t val) {
  switch (gb_err) {
  case GB_INVALID_OPCODE:
    printf("Invalid opcode %#04x at PC: %#06x, SP: %#06x\n", val,
           gb->cpu_reg.pc - 1, gb->cpu_reg.sp);
    break;

  case GB_INVALID_WRITE:
  case GB_INVALID_READ:
    return;

  default:
    printf("Unknown error");
    break;
  }

  printf("Error. Press q to exit, or any other key to continue.");
}

void Hook_DrawDisplayLine(struct gb_s *gb, const uint8_t pixels[160],
                          const uint_fast8_t line) {
  struct priv_t *priv = gb->direct.priv;

  uint16_t *l = &priv->fb[LCD_WIDTH * line];
  for (unsigned int x = 0; x < LCD_WIDTH; x++) {
    l[x] = priv->selected_palette[(pixels[x] & LCD_PALETTE_ALL) >> 4]
                                 [pixels[x] & 3];
  }
}

void auto_assign_palette(struct priv_t *priv, uint8_t game_checksum) {
  size_t palette_bytes = 3 * 4 * sizeof(uint16_t);

  switch (game_checksum) {
  /* Balloon Kid and Tetris Blast */
  case 0x71:
  case 0xFF: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x7E60, 0x7C00, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x7E60, 0x7C00, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x7E60, 0x7C00, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Pokemon Yellow and Tetris */
  case 0x15:
  case 0xDB:
  case 0x95: /* Not officially */
  {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x7FE0, 0x7C00, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x7FE0, 0x7C00, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x7FE0, 0x7C00, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Donkey Kong */
  case 0x19: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x7E60, 0x7C00, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Pokemon Blue */
  case 0x61:
  case 0x45:

  /* Pokemon Blue Star */
  case 0xD8: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x329F, 0x001F, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x329F, 0x001F, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Pokemon Red */
  case 0x14: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x3FE6, 0x0200, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Pokemon Red Star */
  case 0x8B: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x329F, 0x001F, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x3FE6, 0x0200, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Kirby */
  case 0x27:
  case 0x49:
  case 0x5C:
  case 0xB3: {
    const uint16_t palette[3][4] = {
        {0x7D8A, 0x6800, 0x3000, 0x0000}, /* OBJ0 */
        {0x001F, 0x7FFF, 0x7FEF, 0x021F}, /* OBJ1 */
        {0x527F, 0x7FE0, 0x0180, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Donkey Kong Land [1/2/III] */
  case 0x18:
  case 0x6A:
  case 0x4B:
  case 0x6B: {
    const uint16_t palette[3][4] = {
        {0x7F08, 0x7F40, 0x48E0, 0x2400}, /* OBJ0 */
        {0x7FFF, 0x2EFF, 0x7C00, 0x001F}, /* OBJ1 */
        {0x7FFF, 0x463B, 0x2951, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Link's Awakening */
  case 0x70: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x03E0, 0x1A00, 0x0120}, /* OBJ0 */
        {0x7FFF, 0x329F, 0x001F, 0x001F}, /* OBJ1 */
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Mega Man [1/2/3] & others I don't care about. */
  case 0x01:
  case 0x10:
  case 0x29:
  case 0x52:
  case 0x5D:
  case 0x68:
  case 0x6D:
  case 0xF6: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x329F, 0x001F, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x3FE6, 0x0200, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x7EAC, 0x40C0, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  default: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x5294, 0x294A, 0x0000},
                                    {0x7FFF, 0x5294, 0x294A, 0x0000},
                                    {0x7FFF, 0x5294, 0x294A, 0x0000}};
    printf("No palette found for 0x%02X.\n", game_checksum);
    memcpy(priv->selected_palette, palette, palette_bytes);
  }
  }
}

void StartEmulator(struct ILI9341_t *display, uint8_t const *rom,
                   uint8_t *savefile, struct tm datetime) {
  struct gb_s gb;
  struct priv_t priv = {.rom = rom, .cart_ram = savefile};
  enum gb_init_error_e gb_ret =
      gb_init(&gb, &Hook_ReadRom, &Hook_ReadCartridgeRam,
              &Hook_WriteCartridgeRam, &Hook_ReportEmulationError, &priv);
  switch (gb_ret) {
  case GB_INIT_NO_ERROR:
    break;

  case GB_INIT_CARTRIDGE_UNSUPPORTED:
    printf("Unsupported cartridge.");
    return;

  case GB_INIT_INVALID_CHECKSUM:
    printf("Invalid ROM: Checksum failure.");
    return;

  default:
    printf("Unknown error: %d\n", gb_ret);
    return;
  }

  auto_assign_palette(&priv, gb_colour_hash(&gb));
  gb_set_rtc(&gb, &datetime);
  gb_init_lcd(&gb, &Hook_DrawDisplayLine);

  char title[20];
  gb_get_rom_name(&gb, title);
  title[19] = '\0';

  int save_size = gb_get_save_size(&gb);
  if (save_size != 0 && savefile == NULL) {
    priv.cart_ram = malloc(save_size);
  }

  const double target_speed_ms = 1000.0 / VERTICAL_SYNC;
  double speed_compensation = 0.0;
  uint_fast32_t new_ticks, old_ticks;

  while (1) {
    ILI9341_StepProgressBar(display);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    int delay;
    static unsigned int rtc_timer = 0;

    old_ticks = HAL_GetTick();

    // TODO: Read buttons and set 'gb.direct.joypad_bits.XXXX'
    if (old_ticks > 10000)
      gb.direct.joypad_bits.start = 1;

    gb_run_frame(&gb);

    // Tick the internal RTC when 1 second has passed
    rtc_timer += target_speed_ms;

    if (rtc_timer >= 1000) {
      rtc_timer -= 1000;
      gb_tick_rtc(&gb);
    }

    // TODO: Update the screen copying from priv.fb
    ILI9341_DrawFramebuffer(display, priv.fb, LCD_WIDTH, LCD_HEIGHT);

    new_ticks = HAL_GetTick();

    speed_compensation += target_speed_ms - (new_ticks - old_ticks);
    delay = (int)(speed_compensation);
    speed_compensation -= delay;

    // Only run delay logic if required
    if (delay > 0) {
      uint_fast32_t delay_ticks = HAL_GetTick();
      uint_fast32_t after_delay_ticks;

      rtc_timer += delay;

      if (rtc_timer >= 1000) {
        rtc_timer -= 1000;
        gb_tick_rtc(&gb);
      }

      HAL_Delay(delay_ticks);
      after_delay_ticks = HAL_GetTick();
      speed_compensation +=
          (double)delay - (int)(after_delay_ticks - delay_ticks);
    }
  }
}
