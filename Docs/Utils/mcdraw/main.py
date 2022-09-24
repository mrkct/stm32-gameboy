#!/usr/bin/env python3
import mcdraw

### Definition of STM32F411CEU6
left = [
    "3.3V",
    "G",
    "5V",
    "B9",
    "B8",
    "B7",
    "B6",
    "B5",
    "B4",
    "B3",
    "A15",
    "A12",
    "A11",
    "A10",
    "A9",
    "A8",
    "B15",
    "B14",
    "B13",
    "B12",
]
right = [
    "VB",
    "C13",
    "C14",
    "C15",
    "R",
    "A0",
    "A1",
    "A2",
    "A3",
    "A4",
    "A5",
    "A6",
    "A7",
    "B0",
    "B1",
    "B2",
    "B10",
    "3V3",
    "G",
    "5V",
]
stm32 = mcdraw.Part("STM32F411CEU6", left=left, right=right)


### Definition of the ILI9341
left = [
    "SD_SCK",
    "SD_DO",
    "SD_DI",
    "SD_SS",
    "LCD_D1",
    "LCD_D0",
    "LCD_D7",
    "LCD_D6",
    "LCD_D4",
    "LCD_D5",
    "LCD_D3",
    "LCD_D2",
]
right = ["3V3", "5V", "GND", "LCD_RD", "LCD_WR", "LCD_RS", "LCD_CS", "LCD_RST", "F_CS"]
ili = mcdraw.Part("ILI9341", left=left, right=right)


### Definition of the GBDMG PCB
up = [
    "A",
    "B",
    "X",
    "Y",
    "L",
    "R",
    "SELECT",
    "START",
    "UP",
    "DOWN",
    "LEFT",
    "RIGHT" "GND",
]
pcb = mcdraw.Part("GB_DMG_PCB", up=up)


### Control connections from stm32 to ili
stm32_to_ili_ctrl = mcdraw.make_connections([
    (stm32.left["A8"], ili.right["LCD_RD"]),
    (stm32.left["A10"], ili.right["LCD_RS"]),
    (stm32.left["A9"], ili.right["LCD_WR"]),
    (stm32.left["A11"], ili.right["LCD_CS"]),
])

### Data connections from stm32 to ili
stm32_to_ili_data_tup = [
    (stm32.right["A0"], ili.left["LCD_D0"]),
    (stm32.right["A1"], ili.left["LCD_D1"]),
    (stm32.right["A2"], ili.left["LCD_D2"]),
    (stm32.right["A3"], ili.left["LCD_D3"]),
    (stm32.right["A4"], ili.left["LCD_D4"]),
    (stm32.right["A5"], ili.left["LCD_D5"]),
    (stm32.right["A6"], ili.left["LCD_D6"]),
    (stm32.right["A7"], ili.left["LCD_D7"]),
    (stm32.left["A8"], ili.left["LCD_D8"]),
    (stm32.left["A9"], ili.left["LCD_D9"]),
]


### Connections from stm32 to the pcb
stm32_to_pcb_tup = [
    (stm32.left["B7"], pcb.up["B"]),
    (stm32.left["B6"], pcb.up["A"]),
    (stm32.left["B5"], pcb.up["SELECT"]),
    (stm32.left["B4"], pcb.up["START"]),
    (stm32.left["B3"], pcb.up["RIGHT"]),
    (stm32.right["B0"], pcb.up["LEFT"]),
    (stm32.right["B1"], pcb.up["DOWN"]),
    (stm32.right["B2"], pcb.up["UP"]),
]
