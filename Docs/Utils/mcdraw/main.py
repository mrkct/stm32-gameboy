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
stm = mcdraw.Part("STM32F411CEU6", left=left, right=right)


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
