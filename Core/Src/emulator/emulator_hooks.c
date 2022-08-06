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

uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t *const p = gb->direct.priv;
  return p->rom[addr];
}

uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t *const p = gb->direct.priv;
  return p->cart_ram[addr];
}

void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr, const uint8_t val) {
  const struct priv_t *const p = gb->direct.priv;
  p->cart_ram[addr] = val;
}

void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val) {
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

void gb_lcd_draw_line(struct gb_s *gb, const uint8_t *pixels, const uint_fast8_t line) {
  struct priv_t *priv = gb->direct.priv;

  uint16_t *l = &priv->fb[LCD_WIDTH * line];
  for (unsigned int x = 0; x < LCD_WIDTH; x++) {
    l[x] = priv->selected_palette[(pixels[x] & LCD_PALETTE_ALL) >> 4]
                                 [pixels[x] & 3];
  }
}

static void auto_assign_palette(struct priv_t *priv, uint8_t game_checksum) {
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

static void ReadGamepadStatus(struct gb_s *gb) {
#define BUTTON_A GPIOA, GPIO_PIN_8
#define BUTTON_B GPIOA, GPIO_PIN_9
#define BUTTON_START GPIOA, GPIO_PIN_10
#define BUTTON_SELECT GPIOA, GPIO_PIN_11
#define BUTTON_LEFT GPIOA, GPIO_PIN_12
#define BUTTON_UP GPIOA, GPIO_PIN_15
#define BUTTON_RIGHT GPIOB, GPIO_PIN_3
#define BUTTON_DOWN GPIOB, GPIO_PIN_4

// 1=button is up, 0=button is down
#define READ_BUTTON(b) HAL_GPIO_ReadPin(b) == GPIO_PIN_SET ? 0 : 1

  gb->direct.joypad_bits.a = READ_BUTTON(BUTTON_A);
  gb->direct.joypad_bits.b = READ_BUTTON(BUTTON_B);
  gb->direct.joypad_bits.start = READ_BUTTON(BUTTON_START);
  gb->direct.joypad_bits.select = READ_BUTTON(BUTTON_SELECT);
  gb->direct.joypad_bits.left = READ_BUTTON(BUTTON_LEFT);
  gb->direct.joypad_bits.up = READ_BUTTON(BUTTON_UP);
  gb->direct.joypad_bits.right = READ_BUTTON(BUTTON_RIGHT);
  gb->direct.joypad_bits.down = READ_BUTTON(BUTTON_DOWN);
}

void StartEmulator(struct ILI9341_t *display, uint8_t const *rom,
                   uint8_t *savefile, struct tm datetime) {
  struct gb_s gb;
  struct priv_t priv = {.rom = rom, .cart_ram = savefile};
  enum gb_init_error_e gb_ret = gb_init(&gb, &priv);
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
  gb_init_lcd(&gb);
  gb.direct.frame_skip = 1;

  char title[20];
  gb_get_rom_name(&gb, title);
  title[19] = '\0';

  int save_size = gb_get_save_size(&gb);
  if (save_size != 0 && savefile == NULL) {
    priv.cart_ram = malloc(save_size);
  }

  const double target_speed_ms = 1000.0 / VERTICAL_SYNC;


  volatile int fps_counts[16] = {0};
  int fps_counts_pos = 0;
  int frames = 0;

  uint32_t last_ticks = HAL_GetTick();
  while (1) {
    static unsigned int rtc_timer = 0;
    
    
    ReadGamepadStatus(&gb);
    gb_run_frame(&gb);
    gb_run_frame(&gb);
    
    // Tick the internal RTC when 1 second has passed
    rtc_timer += target_speed_ms;
    if (rtc_timer >= 1000) {
      rtc_timer -= 1000;
      gb_tick_rtc(&gb);
    }
    
    ILI9341_DrawFramebuffer(display, priv.fb, LCD_WIDTH, LCD_HEIGHT);
  
    frames++;
    if (HAL_GetTick() > last_ticks + 1000) {
      fps_counts[fps_counts_pos++] = frames;
      frames = 0;
      last_ticks = HAL_GetTick();
      if (fps_counts_pos == 16)
        fps_counts_pos = 0;
    }
  }
}
