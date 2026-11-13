# Continuous Lab Evaluation-10: STM32-Wemos Wi-Fi Modem Interface Using AT Commands

## Overview
STM32F103 Blue Pill firmware communicating with ESP8266 Wemos D1 module using serial AT commands to configure station mode, DHCP, and Wi-Fi network association.

## Project Structure
- `UART_wemos.ioc`: STM32CubeMX device and pinout configuration.
- `.mxproject`: STM32CubeMX code generation settings.
- `Core/`: Application source (`Src/main.c`) and header files (`Inc/main.h`).
- `Drivers/`: Vendor hardware abstraction layer (STM32 HAL & CMSIS).
- `MDK-ARM/`: Keil MDK project files (`UART_wemos.uvprojx`), vector table startup assembly, and compiled firmware binary (`UART_wemos.hex`).

## Build & Toolchain
- **IDE**: Keil µVision / MDK-ARM (ArmClang V6 / ARMCC)
- **Configuration**: STM32CubeMX
