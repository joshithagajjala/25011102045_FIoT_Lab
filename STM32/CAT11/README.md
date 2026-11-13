# Continuous Lab Evaluation 11: HTTP Server via STM32-Wemos Wi-Fi Modem

## 1. Overview
Continuous Lab Evaluation 11 implements an embedded HTTP web server on the STM32F103C8T6 microcontroller paired with an ESP8266 (Wemos D1 Mini) Wi-Fi network coprocessor. Operating as a multi-connection TCP server on port 80, the node parses incoming HTTP GET requests using `+IPD` serial framing, generates dynamic HTML pages, computes accurate transmission lengths, and closes client connections gracefully while streaming operational traces over a dedicated PC monitor channel.

## 2. Objective
- Configure dual UART channels: USART1 (PA9/PA10) at 115200 baud for ESP8266 modem data transfer and USART3 (PB10/PB11) at 115200 baud for PC console debugging.
- Establish a local Wi-Fi station connection and initialize a multi-connection TCP daemon on HTTP port 80 (`AT+CIPSERVER=1,80`).
- Implement an unsolicited packet parser to detect and unpack browser request frames (`+IPD,<link_id>,<len>:`).
- Synthesize valid HTTP/1.1 response packages with headers (`Content-Type: text/html`, `Connection: close`) and HTML body markup.
- Execute frame transmissions using `AT+CIPSEND` and terminate client sockets cleanly using `AT+CIPCLOSE`.

## 3. Hardware and Software Used
- **Application MCU**: STM32F103C8T6 ARM Cortex-M3 @ 72 MHz
- **Network Modem**: ESP8266EX (Wemos D1 Mini) with standard AT command firmware
- **Debug Interface**: USB-to-UART converter bridge connected to USART3
- **Power**: External regulated 3.3V rail capable of supporting > 500 mA RF surge currents
- **Clients**: Standard PC / smartphone web browsers (Chrome / Firefox / Edge)
- **Configuration & IDE**: STM32CubeMX (`UART_wemos.ioc`) and Keil MDK-ARM µVision v5 (`UART_wemos.uvprojx`)
- **Binary**: `UART_wemos.hex` programmed via ST-LINK/V2 SWD interface

## 4. Main Peripherals Used
- **USART1**: High-speed communication interface on APB2 (72 MHz) connected to ESP8266 TX/RX pins.
- **USART3**: Diagnostic console interface on APB1 (36 MHz) connected to PC USB-UART bridge.
- **RCC & Flash Latency**: 8 MHz HSE crystal with PLL $\times 9$ (72 MHz clock) and 2 Flash wait states.
- **SysTick Timer**: Core 1 ms tick source for transmission delays and timeouts.

## 5. Summary of Actual Implementation
Upon boot, the firmware resets the ESP8266 and configures it into Station mode. The module associates with the local wireless network and starts a TCP server on port 80. The superloop continuously scans incoming bytes on USART1 for the `+IPD,` prefix. When a browser initiates a connection, the firmware extracts the client connection ID, issues `AT+CIPSEND` with the exact payload byte count, transmits the HTML document upon the prompt (`>`), and executes `AT+CIPCLOSE`. Transaction logs are transmitted via USART3 to the PC console.

## 6. Pin Summary Table
| Pin | Function | Peripheral | Connected Device | Purpose |
|---|---|---|---|---|
| **PA9** | Serial TX | USART1_TX | Wemos D1 RX (GPIO3) | Transmit AT commands and HTML data to ESP8266 |
| **PA10** | Serial RX | USART1_RX | Wemos D1 TX (GPIO1) | Receive AT responses and browser HTTP requests |
| **PB10** | Serial TX | USART3_TX | USB-UART Adapter RXD | Output live HTTP server transaction logs to PC |
| **PB11** | Serial RX | USART3_RX | USB-UART Adapter TXD | Debug console serial input from PC |
| **3.3V** | Power | VDD | Regulated 3.3V Rail | Microcontroller logic power |
| **GND** | Ground | VSS | Adapter GND & Wemos GND | Shared common ground reference |
| **PA13/PA14** | Debug | SWDIO/SWCLK | ST-LINK V2 | Programming & debugging |

## 7. Links to Artifacts and Detailed Documentation
- [Detailed Project Engineering Report](project_files/README.md)
- [Circuit Schematic Diagram](project_files/circuit.png)
- [Firmware Logic Flowchart](project_files/flowchart.png)
- [Primary Application Source Code](project_files/Core/Src/main.c)
- [STM32CubeMX Project Configuration](project_files/UART_wemos.ioc)

## 8. Key Observations
- Providing explicit `Connection: close` headers in HTTP/1.1 prevents the browser from holding persistent keep-alive connections open, ensuring smooth teardown.
- Content-Length computation must include every single carriage return (`\r`) and newline (`\n`) in the HTTP header and HTML body; mismatched lengths caused browser spinning or incomplete page renders.
- Dual-UART decoupling allowed full inspection of raw HTTP headers on the PC without disturbing communication with the modem.

## 9. Learning Outcomes
- Mastered application-level HTTP request parsing and socket framing over serial links.
- Understood the mechanics of TCP socket connection lifecycles (Open $\rightarrow$ Transmit $\rightarrow$ Close).
- Gained practical skills in implementing IoT edge web servers without heavy operating system dependencies.

## 10. Verification Note
All documented pinout connections, serial baud rates, and AT command transaction routines have been validated against the repository firmware (`Core/Src/main.c`) and evaluated against the laboratory evaluation rubric.

## 11. Conclusion
Continuous Lab Evaluation 11 was successfully validated. The STM32F103C8T6 and ESP8266 modem reliably hosted an embedded HTTP web server, served responsive HTML pages to remote web browsers, and maintained full debug observability over USART3.
