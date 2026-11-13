# Continuous Lab Evaluation-3: Timer-Based LED Pattern Generator and Button Interrupt Controller

## Description
Bare-metal GPIO state controller implementing a four-state machine on Port 1 LEDs (Running LED, 4-bit Binary Counter, Alternating 1010/0101, All-LED Flash) advancing on each valid button press on P3.2 with software switch debouncing.

## Hardware Target
- **Microcontroller**: Atmel AT89S52 (8051 Architecture)
- **Clock Frequency**: 11.0592 MHz / 12 MHz Crystal

## Deliverables
- **Embedded C Source**: `cat3.c`
- **Startup Code**: `STARTUP.A51`
- **Keil Project**: `*.uvproj` / `*.uvopt`
- **Compiled Firmware**: `Objects/cat3.hex`
