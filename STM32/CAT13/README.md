# Continuous Lab Evaluation 13: BME280 Sensor Interfacing Using I2C and Bosch Sensor API

## 1. Overview
Continuous Lab Evaluation 13 interfaces the Bosch Sensortec BME280 digital environmental sensor with the ARM Cortex-M4 STM32F401RET6 microcontroller on the STMicroelectronics Nucleo-64 development board. Operating over the I2C serial bus at 100 kHz, the system integrates the official vendor-provided Bosch BME280 C API. By reading on-chip factory calibration parameters, the firmware performs real-time polynomial compensation to compute accurate temperature (°C), barometric pressure (hPa), and relative humidity (%RH), transmitting continuous serial telemetry to a PC terminal via the integrated ST-LINK Virtual COM Port at 115200 baud.

## 2. Objective
- Configure the STM32F401RE I2C1 peripheral in Master Standard Mode (100 kHz) on pins PB6 (SCL) and PB7 (SDA) with open-drain outputs and pull-up resistors.
- Configure USART2 (PA2/PA3) at 115200 baud for telemetry transmission across the ST-LINK Virtual COM Port.
- Integrate the official Bosch Sensortec BME280 driver by developing custom platform abstraction callback functions (`user_i2c_read`, `user_i2c_write`, `user_delay_us`) wrapping STM32 HAL APIs.
- Retrieve factory calibration trimming parameters from the sensor NVM and configure sensor operating modes and oversampling filters.
- Periodically acquire raw ADC registers, compute calibrated environmental values, format telemetry strings, and stream data to a host serial terminal at 1 Hz.

## 3. Hardware and Software Used
- **Target Platform**: STMicroelectronics NUCLEO-F401RE (ARM Cortex-M4 with FPU @ 84 MHz)
- **Sensor Module**: Bosch Sensortec BME280 (Digital Humidity, Pressure, and Temperature Sensor)
- **Interface**: Inter-Integrated Circuit (I2C) with 4.7 kΩ pull-up resistors to +3.3V
- **Debugger & Serial Bridge**: Integrated ST-LINK/V2-1 Virtual COM Port over Mini-USB
- **Host Software**: Serial terminal emulator (PuTTY / Tera Term / Minicom) @ 115200 baud, 8N1
- **Configuration & IDE**: STM32CubeMX (`I2C-1.ioc`) and Keil MDK-ARM µVision v5 (`I2C-1.uvprojx`)
- **Firmware Binary**: `I2C-1.hex` programmed via SWD

## 4. Main Peripherals Used
- **I2C1**:
  - `PB6`: Serial Clock (`I2C1_SCL`) connected to BME280 SCL.
  - `PB7`: Serial Data (`I2C1_SDA`) connected to BME280 SDA.
  - Bus Speed: Standard Mode (100 kHz) on APB1 peripheral bus.
- **USART2**:
  - `PA2`: USART2_TX routed to ST-LINK VCP RX.
  - `PA3`: USART2_RX routed to ST-LINK VCP TX.
- **RCC & SysTick**: 84 MHz core clock synthesized via PLL; SysTick millisecond timebase for acquisition pacing.

## 5. Summary of Actual Implementation
The application initializes the system clock, I2C1, and USART2. It instantiates the Bosch driver structure (`struct bme280_dev`), registers the HAL wrapper callbacks, sets the 7-bit device address to `0x76`, and invokes `bme280_init(&dev)`. The driver reads the 24 factory calibration parameters from sensor NVM. The sensor is configured for Normal Mode with oversampling and internal IIR filtering enabled. Every 1000 ms, the main loop executes `bme280_get_sensor_data()`, performs multi-order compensation calculations, converts pressure from Pa to hPa, and transmits formatted environmental metrics to the PC console.

## 6. Pin Summary Table
| Pin | Direction | Mode | Configuration | Hardware Role | Description |
|---|---|---|---|---|---|
| **PB6** | Bidirectional | `I2C1_SCL` | Open-Drain, 4.7k Pull-Up | BME280 SCL | I2C Serial Clock line (100 kHz) |
| **PB7** | Bidirectional | `I2C1_SDA` | Open-Drain, 4.7k Pull-Up | BME280 SDA | I2C Serial Data line |
| **PA2** | Output | `USART2_TX` | Alternate Push-Pull | ST-LINK VCP RX | Serial telemetry output to PC |
| **PA3** | Input | `USART2_RX` | Alternate Function | ST-LINK VCP TX | Serial command input from PC |
| **+3.3V** | Power | VDD | Power Rail | BME280 VDD & CSB | Sensor 3.3V power supply |
| **GND** | Ground | VSS | Ground Reference | BME280 GND & SDO | Sensor ground & address select (0x76) |

## 7. Links to Artifacts and Detailed Documentation
- [Detailed Project Engineering Report](project_files/README.md)
- [Circuit Schematic Diagram](project_files/circuit.png)
- [Firmware Logic Flowchart](project_files/flowchart.png)
- [Primary Application Source Code](project_files/Core/Src/main.c)
- [STM32CubeMX Project Configuration](project_files/I2C-1.ioc)

## 8. Key Observations
- The I2C address bit shifting (`dev_id << 1`) was essential: passing unshifted `0x76` caused `HAL_I2C_Mem_Read` to time out because bit 0 was interpreted as write instead of address alignment.
- Reading all 8 data registers (`0xF7`–`0xFE`) in a single burst read guarantees atomic measurement consistency across pressure, temperature, and humidity.
- Applying warm breath to the sensor immediately reflected a sharp rise in humidity (+15–20% RH) and a slight rise in temperature (+1–2°C), followed by a gradual decay back to ambient room conditions.

## 9. Learning Outcomes
- Mastered I2C bus timing, open-drain configuration, pull-up sizing, and 7-bit/8-bit addressing dynamics.
- Gained practical proficiency integrating official vendor-provided C sensor APIs using HAL wrapper functions.
- Learned the mathematical necessity and implementation of factory calibration compensation algorithms for high-accuracy digital sensors.

## 10. Verification Note
All documented pinout connections, I2C addresses, and driver callback implementations have been validated against the repository firmware (`Core/Src/main.c`, `bme280.c`) and evaluated against the laboratory evaluation rubric.

## 11. Conclusion
Continuous Lab Evaluation 13 was successfully validated. The STM32F401RET6 Nucleo-64 board reliably communicates with the Bosch BME280 environmental sensor via I2C at 100 kHz, accurately applies factory compensation math, and streams calibrated temperature, pressure, and humidity readings over the ST-LINK Virtual COM Port.
