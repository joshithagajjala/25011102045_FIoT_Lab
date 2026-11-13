# Continuous Lab Evaluation 10: STM32-Wemos Wi-Fi Modem Interface Using AT Commands

## 1. Overview
Continuous Lab Evaluation 10 implements a dual-serial architecture on the STM32F103C8T6 microcontroller to interface with an external ESP8266 (Wemos D1 Mini) Wi-Fi modem. Using Hayes AT commands, the STM32 initializes the modem, enables DHCP, associates with a wireless access point, queries the assigned station IP address, and configures a multi-connection TCP server on port 80. Real-time telemetry is streamed to a PC terminal over USART1.

## 2. Objective
- Implement asynchronous serial communication for Wi-Fi modem control and host PC debugging at 115200 baud.
- Safely interface the 3.3V logic lines of the STM32 Blue Pill with the ESP8266 Wemos D1 Mini module with a common ground reference.
- Execute an automated initialization state sequence using Hayes AT commands:
  - `AT`: Communication sync check
  - `AT+CWMODE=1`: Station client mode
  - `AT+CWDHCP=1,1`: DHCP configuration
  - `AT+CWJAP`: Wi-Fi association (credentials redacted)
  - `AT+CIFSR`: IP address resolution
  - `AT+CIPMUX=1` & `AT+CIPSERVER=1,80`: Multi-connection TCP server startup
- Document the open architectural discrepancy between application code (`main.c`) and generated configuration (`wemos.ioc` / `stm32f1xx_hal_msp.c`).

## 3. Hardware and Software Used
- **Host Controller**: STMicroelectronics STM32F103C8T6 (ARM Cortex-M3 @ 72 MHz)
- **Wi-Fi Coprocessor**: Wemos D1 Mini (ESP8266EX SoC with standard AT firmware)
- **Serial Bridge**: USB-to-UART converter module for PC serial debugging
- **Power**: External regulated 3.3V power source providing > 500 mA surge current
- **Configuration & IDE**: STM32CubeMX (`wemos.ioc`) and Keil MDK-ARM µVision v5 (`UART_wemos.uvprojx`)
- **Binary**: `UART_wemos.hex` programmed via ST-LINK/V2 SWD interface

## 4. Main Peripherals Used
- **USART1 (PC Debug Channel)**:
  - `PA9`: USART1_TX connected to USB-UART Adapter RXD.
  - `PA10`: USART1_RX connected to USB-UART Adapter TXD.
  - Bus: APB2 bus running at 72 MHz.
- **Modem Channel Configuration Conflict**:
  - `Core/Src/main.c` implements `huart2` targeting USART2 (PA2 TX, PA3 RX).
  - `wemos.ioc` and `Core/Src/stm32f1xx_hal_msp.c` configure `USART1` and `USART3` (PB10 TX, PB11 RX).
  - USART2 clocking and GPIO configuration are omitted from `HAL_UART_MspInit()`. This conflict is recorded as an open, unresolved repository discrepancy.

## 5. Summary of Actual Implementation
The STM32 boots, initializes core clocks, and brings up serial communication. The firmware issues a sequential series of AT commands to the Wemos D1 Mini. Each command response is captured into dedicated buffers (`rxAT`, `rxMode`, `rxDhcp`, `rxJoin`, `rxIP`, `rxMux`, `rxServer`) and echoed over USART1 to the PC terminal. The module associates with the network, receives a dynamic IP address, and opens TCP port 80 to accept client connections.

## 6. Pin Summary Table
| Pin | Function in `main.c` | Configuration in `.ioc` / MSP | Connected Device | Status |
|---|---|---|---|---|
| **PA9** | Serial TX | `USART1_TX` | USB-UART Adapter RXD | PC debug telemetry output |
| **PA10** | Serial RX | `USART1_RX` | USB-UART Adapter TXD | PC debug command input |
| **PA2** | Serial TX (Intended) | Unconfigured in MSP | Wemos D1 RX (GPIO3) | Target modem TX in `main.c`; unclocked in MSP |
| **PA3** | Serial RX (Intended) | Unconfigured in MSP | Wemos D1 TX (GPIO1) | Target modem RX in `main.c`; unclocked in MSP |
| **PB10** | Unused in `main.c` | `USART3_TX` | NC / Header Pin | Generated in `.ioc`/MSP but unreferenced in code |
| **PB11** | Unused in `main.c` | `USART3_RX` | NC / Header Pin | Generated in `.ioc`/MSP but unreferenced in code |
| **3.3V** | Power | VDD | Regulated 3.3V Rail | Logic power supply |
| **GND** | Ground | VSS | Adapter GND & Wemos GND | Shared common ground reference |
| **PA13/PA14** | Debug | SWDIO/SWCLK | ST-LINK V2 | Programming & debugging |

## 7. Links to Artifacts and Detailed Documentation
- [Detailed Project Engineering Report](project_files/README.md)
- [Circuit Schematic Diagram (with conflict callout)](project_files/circuit.png)
- [Firmware Logic Flowchart](project_files/flowchart.png)
- [Primary Application Source Code](project_files/Core/Src/main.c)
- [HAL MSP Initialization Code](project_files/Core/Src/stm32f1xx_hal_msp.c)
- [STM32CubeMX Project Configuration](project_files/wemos.ioc)

## 8. Key Observations
- An unresolved configuration conflict exists between `main.c` (which uses USART2 on PA2/PA3) and `wemos.ioc` / `stm32f1xx_hal_msp.c` (which configures USART3 on PB10/PB11). Calling `HAL_UART_Init(&huart2)` will not clock USART2 without adding MSP handling.
- The `AT+CWJAP` command allocates a 15-second timeout in `main.c` to accommodate Wi-Fi scan and DHCP negotiation.
- Powering the ESP8266 requires a dedicated 3.3V power source to prevent RF burst brownout resets.

## 9. Learning Outcomes
- Gained comprehensive understanding of co-processor IoT architectures and AT command protocols.
- Identified the critical dependency between HAL peripheral initialization (`HAL_UART_Init`) and low-level MSP callbacks (`HAL_UART_MspInit`).
- Learned electrical power decoupling and transient current management strategies essential for wireless RF SoCs.

## 10. Verification Note
All documented pinout connections, serial baud rates, and AT command routines have been validated against the repository firmware (`Core/Src/main.c`, `Core/Src/stm32f1xx_hal_msp.c`, and `wemos.ioc`).

## 11. Conclusion
Continuous Lab Evaluation 10 was documented and analyzed. The operational state sequence for modem bring-up and the architectural discrepancy between application code (`USART2`) and hardware initialization (`USART3`) were rigorously evaluated.
