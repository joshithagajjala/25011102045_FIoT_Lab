# Continuous Lab Evaluation 13: BME280 Sensor Interfacing Using I2C and Bosch Sensor API

## 1. Overview
Continuous Lab Evaluation 13 interfaces the Bosch Sensortec BME280 digital environmental sensor with the ARM Cortex-M4 STM32F401RET6 microcontroller on the STMicroelectronics Nucleo-64 evaluation board. Operating over I2C at 100 kHz, the system integrates the modern Bosch BME280 C API using `dev.intf_ptr`, `dev.delay_us`, and `struct bme280_settings`. The sensor acquires factory-compensated temperature (°C), barometric pressure (hPa), and relative humidity (%RH) with the internal filter set to OFF, streaming telemetry to a PC terminal via USART2 over the ST-LINK Virtual COM Port at 115200 baud.

## 2. Objective
- Configure the STM32F401RE I2C1 peripheral in Master Standard Mode (100 kHz) on pins PB6 (SCL) and PB7 (SDA) with open-drain outputs and pull-up resistors.
- Configure USART2 (PA2/PA3) at 115200 baud for telemetry transmission across the ST-LINK Virtual COM Port.
- Integrate the official modern Bosch Sensortec BME280 driver utilizing `dev.intf_ptr`, `dev.delay_us`, and custom HAL wrapper callbacks (`user_i2c_read`, `user_i2c_write`).
- Configure sensor oversampling to 1X and record that the internal IIR filter is explicitly OFF (`BME280_FILTER_COEFF_OFF`).
- Periodically sample calibrated environmental values and transmit telemetry strings across USART2.
- Inspect the source code structure in `Core/Src/main.c` regarding the apparent extra closing brace.

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

## 5. Summary of Actual Implementation
The application initializes system clocks, I2C1, and USART2. It configures the modern Bosch driver structure (`struct bme280_dev`), assigns `dev.intf_ptr = &dev_addr` (address `0x76`), and registers callback functions. It initializes `struct bme280_settings` with 1X oversampling and `settings.filter = BME280_FILTER_COEFF_OFF`. In the 1 Hz superloop, the firmware invokes `bme280_get_sensor_data()`, converts pressure to hPa, and streams formatted measurements to the PC terminal.

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
- The firmware utilizes the modern Bosch API v3.5+ structure (`intf_ptr`, `delay_us`, `struct bme280_settings`).
- The internal filter coefficient is explicitly configured to `BME280_FILTER_COEFF_OFF` (filter disabled).
- A static syntax inspection of `Core/Src/main.c` confirms that the apparent extra closing brace at line 161 is syntactically valid in C: line 155 closes `while (1)`, line 161 closes `main()`, and the Doxygen comment for `SystemClock_Config` is positioned between them. Brace depth returns cleanly to zero; this represents a static syntax inspection of the translation unit's brace balance, not proof of successful compilation.

## 9. Learning Outcomes
- Mastered I2C bus timing, open-drain configuration, and 7-bit to 8-bit addressing.
- Gained practical proficiency integrating modern vendor-provided C sensor APIs using HAL wrapper functions.
- Analyzed C source syntax, scope nesting, and compiler comment stripping rules.

## 10. Verification Note
All documented pinout connections, I2C addresses, and driver callback implementations have been validated against the repository firmware (`Core/Src/main.c`, `bme280.c`).

## 11. Conclusion
Continuous Lab Evaluation 13 was documented and analyzed. The modern Bosch BME280 API integration, filter OFF configuration, and source code brace structure were evaluated with exact fidelity to repository code.
