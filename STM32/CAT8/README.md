# Continuous Lab Evaluation-8: STM32 GPIO Bring-Up and Switch-Controlled LED

## Overview
STM32F103 Blue Pill firmware implementing software switch debouncing and event classification using non-blocking HAL_GetTick(). Short press (< 1s) toggles LED / stops blinking, and long press (>= 1s) enters continuous LED blinking mode.

## Project Structure
- `LED_MANIP.ioc`: STM32CubeMX device and pinout configuration.
- `.mxproject`: STM32CubeMX code generation settings.
- `Core/`: Application source (`Src/main.c`) and header files (`Inc/main.h`).
- `Drivers/`: Vendor hardware abstraction layer (STM32 HAL & CMSIS).
- `MDK-ARM/`: Keil MDK project files (`LED_MANIP.uvprojx`), vector table startup assembly, and compiled firmware binary (`LED_MANIP.hex`).

## Build & Toolchain
- **IDE**: Keil µVision / MDK-ARM (ArmClang V6 / ARMCC)
- **Configuration**: STM32CubeMX
