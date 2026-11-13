# Continuous Lab Evaluation-12: STM32F401RE Nucleo GPIO and User Input Programming

## Overview
STM32F401RE Nucleo-64 clock bring-up, push-button debouncing, and short/long press detection toggling user LED LD2 (PA5) via non-blocking timing.

## Project Structure
- `nucleo_test.ioc`: STM32CubeMX device and pinout configuration.
- `.mxproject`: STM32CubeMX code generation settings.
- `Core/`: Application source (`Src/main.c`) and header files (`Inc/main.h`).
- `Drivers/`: Vendor hardware abstraction layer (STM32 HAL & CMSIS).
- `MDK-ARM/`: Keil MDK project files (`nucleo_test.uvprojx`), vector table startup assembly, and compiled firmware binary (`nucleo_test.hex`).

## Build & Toolchain
- **IDE**: Keil µVision / MDK-ARM (ArmClang V6 / ARMCC)
- **Configuration**: STM32CubeMX
