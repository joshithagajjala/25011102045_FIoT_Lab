# Continuous Lab Evaluation-11: HTTP Server via STM32-Wemos Wi-Fi Modem

## Overview
Configures ESP8266 as a multiple-connection TCP server on port 80 (AT+CIPSERVER=1,80), parses incoming +IPD frames from web browsers, and serves static HTML responses.

## Project Structure
- `UART_wemos.ioc`: STM32CubeMX device and pinout configuration.
- `.mxproject`: STM32CubeMX code generation settings.
- `Core/`: Application source (`Src/main.c`) and header files (`Inc/main.h`).
- `Drivers/`: Vendor hardware abstraction layer (STM32 HAL & CMSIS).
- `MDK-ARM/`: Keil MDK project files (`UART_wemos.uvprojx`), vector table startup assembly, and compiled firmware binary (`UART_wemos.hex`).

## Build & Toolchain
- **IDE**: Keil µVision / MDK-ARM (ArmClang V6 / ARMCC)
- **Configuration**: STM32CubeMX
