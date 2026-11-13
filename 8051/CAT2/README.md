# Continuous Lab Evaluation-2: Bare-Metal LED Blink

## Description
Bare-metal Embedded C program for Atmel AT89S52 microcontroller controlling LEDs via direct memory-mapped GPIO register access on Port 1 (P1). Uses a software delay loop to blink LEDs at approximately 1 Hz without external libraries.

## Hardware Target
- **Microcontroller**: Atmel AT89S52 (8051 Architecture)
- **Clock Frequency**: 11.0592 MHz / 12 MHz Crystal

## Deliverables
- **Embedded C Source**: `led_test.c`
- **Startup Code**: `STARTUP.A51`
- **Keil Project**: `*.uvproj` / `*.uvopt`
- **Compiled Firmware**: `Objects/led_test.hex`
