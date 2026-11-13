# Continuous Lab Evaluation-14: BME280 Sensor Interfacing and MQTT Telemetry via ESP8266

## Overview
STM32F401RE environmental telemetry publisher transmitting BME280 temperature, pressure, and humidity readings to MQTT broker using custom ESP8266 AT library.

## Project Structure
- `mqtt1.ioc`: STM32CubeMX device and pinout configuration.
- `.mxproject`: STM32CubeMX code generation settings.
- `Core/`: Application source (`Src/main.c`) and header files (`Inc/main.h`).
- `Drivers/`: Vendor hardware abstraction layer (STM32 HAL & CMSIS).
- `MDK-ARM/`: Keil MDK project files (`mqtt1.uvprojx`), vector table startup assembly, and compiled firmware binary (`mqtt1.hex`).

## Build & Toolchain
- **IDE**: Keil µVision / MDK-ARM (ArmClang V6 / ARMCC)
- **Configuration**: STM32CubeMX
