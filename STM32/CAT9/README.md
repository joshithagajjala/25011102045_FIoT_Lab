# Continuous Lab Evaluation-9: STM32 UART Command Controlled LED

## Overview
STM32F103 Blue Pill serial command interface decoding single-character ASCII instructions over UART to control peripheral and onboard LED state.

## Project Structure
- `UART_9.ioc`: STM32CubeMX device and pinout configuration.
- `.mxproject`: STM32CubeMX code generation settings.
- `Core/`: Application source (`Src/main.c`) and header files (`Inc/main.h`).
- `Drivers/`: Vendor hardware abstraction layer (STM32 HAL & CMSIS).
- `MDK-ARM/`: Keil MDK project files (`UART_9.uvprojx`), vector table startup assembly, and compiled firmware binary (`UART_9.hex`).

## Build & Toolchain
- **IDE**: Keil µVision / MDK-ARM (ArmClang V6 / ARMCC)
- **Configuration**: STM32CubeMX
