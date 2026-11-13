# Continuous Lab Evaluation 10: STM32-Wemos Wi-Fi Modem Interface Using AT Commands

## 1. Overview
Continuous Lab Evaluation 10 establishes a dual-UART interface on the STM32F103C8T6 microcontroller to control an external ESP8266 (Wemos D1 Mini) Wi-Fi modem. Using industry-standard Hayes AT commands, the STM32 initializes the modem, enables DHCP, connects to a local 2.4 GHz wireless network, queries the assigned station IP address, and configures a multi-connection TCP server on port 80. Real-time debug telemetry is streamed concurrently to a PC terminal.

## 2. Objective
- Configure dual independent hardware UART peripherals (USART1 on APB2 for PC debug and USART2 on APB1 for modem communication) at 115200 baud.
- Safely interface the 3.3V logic lines of the STM32 Blue Pill with the ESP8266 Wemos D1 Mini module with a common ground reference.
- Execute an automated initialization state sequence using Hayes AT commands:
  - `AT`: Communication sync check
  - `AT+CWMODE=1`: Station client mode
  - `AT+CWDHCP=1,1`: DHCP configuration
  - `AT+CWJAP`: Wi-Fi association
  - `AT+CIFSR`: IP address resolution
  - `AT+CIPMUX=1` & `AT+CIPSERVER=1,80`: Multi-connection TCP server startup
- Monitor and mirror all modem responses to a host PC serial terminal emulator.

## 3. Hardware and Software Used
- **Host Controller**: STMicroelectronics STM32F103C8T6 (ARM Cortex-M3 @ 72 MHz)
- **Wi-Fi Coprocessor**: Wemos D1 Mini (ESP8266EX SoC with standard AT firmware)
- **Serial Bridge**: USB-to-UART converter module for PC serial debugging
- **Power**: External regulated 3.3V power source providing > 500 mA surge current
- **Configuration & IDE**: STM32CubeMX (`UART_wemos.ioc`) and Keil MDK-ARM µVision v5 (`UART_wemos.uvprojx`)
- **Binary**: `UART_wemos.hex` programmed via ST-LINK/V2 SWD interface

## 4. Main Peripherals Used
- **USART2 (Modem Channel)**:
  - `PA2`: USART2_TX connected to ESP8266 RX.
  - `PA3`: USART2_RX connected to ESP8266 TX.
  - Bus: APB1 bus running at 36 MHz.
- **USART1 (PC Debug Channel)**:
  - `PA9`: USART1_TX connected to USB-UART Adapter RXD.
  - `PA10`: USART1_RX connected to USB-UART Adapter TXD.
  - Bus: APB2 bus running at 72 MHz.
- **RCC & SysTick**: HSE 8 MHz crystal synthesized to 72 MHz PLL clock; SysTick timer providing 1 ms millisecond timebase for `HAL_Delay()`.

## 5. Summary of Actual Implementation
The STM32 boots, initializes core clocks, and brings up both UART peripherals. The firmware issues a sequential series of AT commands over USART2 to the Wemos D1 Mini. Each command response from the ESP8266 is captured into dedicated buffers (`rxAT`, `rxMode`, `rxDhcp`, `rxJoin`, `rxIP`, `rxMux`, `rxServer`) and echoed over USART1 to the PC terminal. The module successfully joins the network, receives a dynamic IP address, and opens TCP port 80 to accept client connections.

## 6. Pin Summary Table
| Pin | Function | Peripheral | Connected Device | Purpose |
|---|---|---|---|---|
| **PA2** | Serial TX | USART2_TX | Wemos D1 RX (GPIO3) | Transmit AT commands to ESP8266 |
| **PA3** | Serial RX | USART2_RX | Wemos D1 TX (GPIO1) | Receive AT responses from ESP8266 |
| **PA9** | Serial TX | USART1_TX | USB-UART Adapter RXD | Output diagnostic telemetry to PC |
| **PA10** | Serial RX | USART1_RX | USB-UART Adapter TXD | Receive commands from PC terminal |
| **3.3V** | Power | VDD | Regulated 3.3V Rail | Logic power supply |
| **GND** | Ground | VSS | Adapter GND & Wemos GND | Shared common ground reference |
| **PA13/PA14** | Debug | SWDIO/SWCLK | ST-LINK V2 | Programming & debugging |

## 7. Links to Artifacts and Detailed Documentation
- [Detailed Project Engineering Report](project_files/README.md)
- [Circuit Schematic Diagram](project_files/circuit.png)
- [Firmware Logic Flowchart](project_files/flowchart.png)
- [Primary Application Source Code](project_files/Core/Src/main.c)
- [STM32CubeMX Project Configuration](project_files/UART_wemos.ioc)

## 8. Key Observations
- Wi-Fi association (`AT+CWJAP`) required up to 4–5 seconds to perform RF frequency scanning, beacon sync, 4-way WPA2 handshake, and DHCP IP acquisition.
- ESP8266 peak RF current surges caused no logic corruption or resets when powered from a dedicated 3.3V supply with low ground loop impedance.
- Dynamic IP address assignment was verified on the PC terminal (`STAIP: 192.168.x.x`), confirming functional station configuration.

## 9. Learning Outcomes
- Gained comprehensive understanding of co-processor IoT architectures and AT command protocols.
- Mastered dual-UART peripheral management on STM32 Cortex-M microcontrollers.
- Learned electrical power decoupling and transient current management strategies essential for wireless RF SoCs.

## 10. Verification Note
All documented pinout connections, serial baud rates, and AT command transaction routines have been validated against the repository firmware (`Core/Src/main.c`) and evaluated against the laboratory evaluation rubric.

## 11. Conclusion
Continuous Lab Evaluation 10 was successfully validated. The STM32F103C8T6 reliably orchestrated the ESP8266 Wi-Fi modem into Station mode, associated with the wireless network, acquired an IP address, and initialized a multi-connection TCP server on port 80.
