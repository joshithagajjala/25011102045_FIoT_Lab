# Continuous Lab Evaluation-13: BME280 Sensor Interfacing Using I2C and Bosch Sensor API

## Overview
Interfaces Bosch Sensortec BME280 environmental sensor with STM32F401RE Nucleo over I2C to obtain factory-compensated temperature, pressure, and humidity measurements.

## Project Structure
- `I2C-1.ioc`: STM32CubeMX device and pinout configuration.
- `.mxproject`: STM32CubeMX code generation settings.
- `Core/`: Application source (`Src/main.c`) and header files (`Inc/main.h`).
- `Drivers/`: Vendor hardware abstraction layer (STM32 HAL & CMSIS).
- `MDK-ARM/`: Keil MDK project files (`I2C-1.uvprojx`), vector table startup assembly, and compiled firmware binary (`I2C-1.hex`).

## Build & Toolchain
- **IDE**: Keil µVision / MDK-ARM (ArmClang V6 / ARMCC)
- **Configuration**: STM32CubeMX
