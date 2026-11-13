# Continuous Lab Evaluation 10: STM32-Wemos Wi-Fi Modem Interface Using AT Commands

## 1. Experiment Title
Continuous Lab Evaluation 10 — Interfacing STM32F103C8T6 with ESP8266 (Wemos D1 Mini) Wi-Fi Modem via Dual Hardware UART and Hayes AT Command Sequence

## 2. Aim
To configure dual hardware serial peripherals on the STM32F103C8T6 microcontroller—USART1 for host PC diagnostic logging and USART2 for modem communication—both operating at 115200 bps; interface an ESP8266 Wi-Fi module (Wemos D1 Mini); execute an automated sequence of Hayes AT commands to initialize the modem into Station mode, enable DHCP, associate with a local wireless access point, retrieve the assigned IP address, and establish a multi-connection TCP listener on port 80.

## 3. Hardware Components
| Component | Specification | Quantity | Role / Remarks |
|---|---|---|---|
| Microcontroller Board | STM32F103C8T6 ("Blue Pill") | 1 | 32-bit ARM Cortex-M3 host controller |
| Wi-Fi SoC Module | Wemos D1 Mini (ESP8266EX) | 1 | Wireless network coprocessor running official Espressif AT firmware |
| USB-to-UART Adapter | CP2102 / CH340 / FT232RL | 1 | Connects STM32 USART1 to PC serial terminal for debug monitoring |
| External 3.3V Power Supply | 3.3V Regulated (> 500 mA) | 1 | Powers ESP8266 during high-current 802.11b/g/n RF transmissions |
| Hardware Programmer | ST-LINK/V2 | 1 | SWD firmware programming and live debugging |
| Breadboard & Jumpers | Solderless breadboard + wires | 1 | Interconnecting dual serial lines and power rails |

## 4. Software Tools
- **STM32CubeMX**: Peripheral initialization for dual USART and system clocks (`UART_wemos.ioc`).
- **Keil MDK-ARM (µVision v5)**: Firmware development, compilation, and hex generation (`UART_wemos.uvprojx`).
- **Serial Terminal Emulator**: PuTTY, Tera Term, or Minicom (115200 baud, 8N1).
- **Espressif AT Firmware**: Default factory firmware on ESP8266 supporting standard `AT+` commands.

## 5. Theory
### Architecture of Dual-UART Microcontroller-to-Modem Interface
In IoT systems, microcontrollers lacking integrated RF radios leverage external network coprocessors (such as the ESP8266) acting as smart serial modems. This architecture delegates the complete TCP/IP protocol stack, WPA2 encryption, and RF physical layer handling to the coprocessor, freeing microcontroller CPU resources.
To maintain transparency and allow debugging:
- **Primary Peripheral (USART2)**: Transmits commands to and receives responses from the ESP8266.
- **Secondary Peripheral (USART1)**: Mirrors modem status, connection progress, and diagnostic traces to the developer's PC terminal.

### Hayes AT Command Protocol for Wi-Fi
The modem is controlled using the industry-standard AT (Attention) command set originally developed for telephony modems and standardized for cellular/Wi-Fi devices. Each command is sent as an ASCII string terminated by carriage return and line feed (`\r\n`), and the modem acknowledges with responses such as `OK`, `ERROR`, or data payloads.

### The Wi-Fi Initialization Sequence
The initialization pipeline requires strict state progression:
1. `AT`: Basic communication synchronization check. The modem responds with `OK`.
2. `AT+CWMODE=1`: Configures Wi-Fi mode to Station (STA) client mode (1 = Station, 2 = SoftAP, 3 = SoftAP+Station).
3. `AT+CWDHCP=1,1`: Enables Dynamic Host Configuration Protocol (DHCP) for the station interface, allowing automatic IP assignment from the router.
4. `AT+CWJAP="[REDACTED]","[REDACTED]"`: Associates the modem with the specified Wi-Fi network SSID using WPA2 credentials.
5. `AT+CIFSR`: Queries the TCP/IP stack for the station IP address (`STAIP`) and MAC address (`STAMAC`).
6. `AT+CIPMUX=1`: Configures multi-connection mode (required to operate as a multi-client server).
7. `AT+CIPSERVER=1,80`: Starts a TCP server daemon listening on port 80 (HTTP).

## 6. Actual Pin Configuration
| STM32 Pin | Function | Peripheral | Connected Device Pin | Description |
|---|---|---|---|---|
| **PA9** | Serial TX | USART1_TX | USB-UART Adapter RXD | PC debug output from STM32 |
| **PA10** | Serial RX | USART1_RX | USB-UART Adapter TXD | PC debug input to STM32 |
| **PA2** | Serial TX | USART2_TX | Wemos D1 RX (GPIO3) | Transmits AT commands to ESP8266 |
| **PA3** | Serial RX | USART2_RX | Wemos D1 TX (GPIO1) | Receives AT responses from ESP8266 |
| **3.3V** | Power | VDD | Power Rail | Microcontroller 3.3V logic supply |
| **GND** | Ground | VSS | Adapter GND & Wemos GND | Mandatory common ground reference |
| **PA13/PA14** | Debug | SWDIO/SWCLK | ST-LINK V2 | Programming & debugging |

> [!CAUTION]
> **ESP8266 Power Rail Current Spike Warning**:
> The ESP8266 can draw transient current pulses up to 250–300 mA during RF packet synthesis and Wi-Fi association. The on-board 3.3V linear regulator on low-cost Blue Pill boards often cannot supply this current, resulting in brownout resets. An external 3.3V power supply or dedicated USB-powered rail with a shared common ground must be used.

## 7. Circuit Diagram
![Circuit Diagram](circuit.png)

## 8. CubeMX / Peripheral Configuration
- **RCC Configuration**: HSE Crystal Resonator (8 MHz) $\rightarrow$ PLL $\times 9$ $\rightarrow$ SYSCLK = 72 MHz.
- **USART1 Configuration (PC Interface)**:
  - Bus: APB2 (72 MHz)
  - Mode: Asynchronous
  - Baud Rate: 115200 bps, 8N1
  - Flow Control: None
- **USART2 Configuration (ESP8266 Interface)**:
  - Bus: APB1 (36 MHz)
  - Mode: Asynchronous
  - Baud Rate: 115200 bps, 8N1
  - Flow Control: None
- **SYS**: Serial Wire Debug enabled.

## 9. Key HAL APIs Used
- `HAL_UART_Init(UART_HandleTypeDef *huart)`: Initializes USART hardware registers and baud rate generators for both `huart1` and `huart2`.
- `HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)`: Sends AT command strings across the selected serial interface.
- `HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)`: Receives modem acknowledgment frames and characters.
- `HAL_Delay(uint32_t Delay)`: Provides pacing delays between successive AT commands to allow Wi-Fi association and DHCP negotiation to stabilize.

## 10. Source Code Logic
The AT command orchestration in `Core/Src/main.c`:

```c
/* USER CODE BEGIN 2 */
uint8_t buffer[100];
uint8_t rxAT[10];
uint8_t rxMode[10];
uint8_t rxDhcp[10];
uint8_t rxJoin[50];
uint8_t rxIP[50];
uint8_t rxMux[10];
uint8_t rxServer[10];

// Step 1: Check AT Communication
HAL_UART_Transmit(&huart2, (uint8_t *)"AT\r\n", 4, 1000);
HAL_UART_Receive(&huart2, rxAT, 6, 1000);
HAL_UART_Transmit(&huart1, rxAT, 6, 1000); // Echo to PC
HAL_Delay(1000);

// Step 2: Set Station Mode
HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CWMODE=1\r\n", 13, 1000);
HAL_UART_Receive(&huart2, rxMode, 6, 1000);
HAL_UART_Transmit(&huart1, rxMode, 6, 1000);
HAL_Delay(1000);

// Step 3: Enable DHCP
HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CWDHCP=1,1\r\n", 15, 1000);
HAL_UART_Receive(&huart2, rxDhcp, 6, 1000);
HAL_UART_Transmit(&huart1, rxDhcp, 6, 1000);
HAL_Delay(1000);

// Step 4: Join Wi-Fi Access Point (credentials redacted for security)
HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CWJAP=\"[REDACTED]\",\"[REDACTED]\"\r\n", 35, 1000);
HAL_UART_Receive(&huart2, rxJoin, 30, 10000); // Extended timeout for RF scan & auth
HAL_UART_Transmit(&huart1, rxJoin, 30, 1000);
HAL_Delay(5000);

// Step 5: Query Station IP
HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CIFSR\r\n", 10, 1000);
HAL_UART_Receive(&huart2, rxIP, 40, 2000);
HAL_UART_Transmit(&huart1, rxIP, 40, 1000);
HAL_Delay(1000);

// Step 6: Enable Multi-Connection Mode
HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CIPMUX=1\r\n", 13, 1000);
HAL_UART_Receive(&huart2, rxMux, 6, 1000);
HAL_UART_Transmit(&huart1, rxMux, 6, 1000);
HAL_Delay(1000);

// Step 7: Start TCP Server on Port 80
HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CIPSERVER=1,80\r\n", 19, 1000);
HAL_UART_Receive(&huart2, rxServer, 6, 1000);
HAL_UART_Transmit(&huart1, rxServer, 6, 1000);
/* USER CODE END 2 */
```

> [!NOTE]
> **Credential Security Notice**:
> In accordance with secure embedded engineering practices, the Wi-Fi SSID and password parameters are redacted as `[REDACTED]` in documentation.

## 11. Algorithm
1. **Boot Initialization**: Initialize HAL, configure 72 MHz system clock, and initialize both USART1 and USART2 at 115200 baud.
2. **Sync Verification**: Transmit `AT\r\n` to ESP8266 over USART2. Read response into `rxAT` and mirror to USART1.
3. **Configure Mode**: Transmit `AT+CWMODE=1\r\n` to configure station mode. Mirror response to USART1.
4. **Enable DHCP**: Transmit `AT+CWDHCP=1,1\r\n` to enable dynamic IP addressing. Mirror response to USART1.
5. **Access Point Association**: Transmit `AT+CWJAP="..."` with a 10-second timeout to allow RF association and DHCP lease acquisition.
6. **Query Station IP**: Transmit `AT+CIFSR\r\n`. Receive IP string and print to PC terminal.
7. **Enable MUX**: Transmit `AT+CIPMUX=1\r\n` to allow concurrent TCP sockets.
8. **Launch TCP Server**: Transmit `AT+CIPSERVER=1,80\r\n`.
9. **Superloop Listening**: Enter continuous listening loop to monitor for incoming `+IPD` client HTTP connections.

## 12. Pseudocode
```text
INITIALIZE HAL
SET Clock = 72MHz
INITIALIZE USART1 (115200 8N1) // Debug console
INITIALIZE USART2 (115200 8N1) // ESP8266 Modem

FUNCTION Execute_AT(cmd, response_buf, expected_len, timeout):
    TRANSMIT(USART2, cmd)
    RECEIVE(USART2, response_buf, expected_len, timeout)
    TRANSMIT(USART1, response_buf, expected_len) // Echo to PC
    DELAY(1000)
END FUNCTION

MAIN:
    Execute_AT("AT\r\n", rxAT, 6, 1000)
    Execute_AT("AT+CWMODE=1\r\n", rxMode, 6, 1000)
    Execute_AT("AT+CWDHCP=1,1\r\n", rxDhcp, 6, 1000)
    Execute_AT("AT+CWJAP=\"[REDACTED]\",\"[REDACTED]\"\r\n", rxJoin, 30, 10000)
    Execute_AT("AT+CIFSR\r\n", rxIP, 40, 2000)
    Execute_AT("AT+CIPMUX=1\r\n", rxMux, 6, 1000)
    Execute_AT("AT+CIPSERVER=1,80\r\n", rxServer, 6, 1000)

    LOOP FOREVER:
        POLL_FOR_INCOMING_PACKETS()
    END LOOP
```

## 13. Flowchart
![Flowchart](flowchart.png)

## 14. Expected Output
On the host PC serial terminal (connected to USART1 @ 115200):
```text
AT
OK
AT+CWMODE=1
OK
AT+CWDHCP=1,1
OK
WIFI CONNECTED
WIFI GOT IP
AT+CIFSR
+CIFSR:STAIP,"192.168.1.105"
+CIFSR:STAMAC,"84:f3:eb:21:4a:12"
OK
AT+CIPMUX=1
OK
AT+CIPSERVER=1,80
OK
```

## 15. Actual Observations
- The `AT+CWJAP` command requires up to 3 to 6 seconds depending on beacon intervals and DHCP negotiation time. Providing a generous timeout (10000 ms) in `HAL_UART_Receive` prevented timeout truncation errors.
- Wi-Fi association successfully printed the assigned station IP address (`192.168.x.x`), confirming DHCP configuration.
- The modem reliably responded with `OK` to both `AT+CIPMUX=1` and `AT+CIPSERVER=1,80`.

## 16. Troubleshooting
1. **No Response to Initial `AT` (rxAT remains empty)**:
   - Check baud rate: Some factory ESP8266 modules default to 9600 or 74880 baud. If needed, configure the module to 115200 baud first.
   - Verify crossed wiring: STM32 PA2 (TX) goes to ESP8266 RX; STM32 PA3 (RX) goes to ESP8266 TX.
2. **Module Resets or Reboots Continuously**:
   - Insufficient power supply current. Connect ESP8266 to an external 3.3V power source capable of delivering 500 mA, and bond all GNDs together.
3. **`CWJAP` Returns `FAIL` or `DISCONNECT`**:
   - Check SSID spelling and credentials.
   - Ensure the Wi-Fi network operates on the 2.4 GHz band (ESP8266 does not support 5 GHz 802.11a/ac).

## 17. Limitations
- Uses static fixed-size buffer arrays for AT response reception rather than dynamic string parsing.
- Does not automatically retry if `AT+CWJAP` fails to associate on the first attempt.
- Superloop polling without interrupts can miss packets during lengthy blocking delays.

## 18. Result
Dual hardware UART interfaces on the STM32F103C8T6 were configured and validated. The ESP8266 Wi-Fi modem was successfully commanded into Station mode, associated with the local network, acquired a DHCP IP address, and initialized as a TCP server on port 80.

## 19. Viva Questions with Answers
1. **Q: Why are two separate UART peripherals used in this design?**  
   *A:* USART2 is dedicated to binary and command exchange with the ESP8266 modem, while USART1 provides an independent telemetry channel to the host PC for live debugging without interfering with modem framing.
2. **Q: What is the purpose of `AT+CIPMUX=1` before starting `AT+CIPSERVER`?**  
   *A:* `AT+CIPMUX=1` enables multi-connection mode. The ESP8266 server daemon requires multi-connection mode so it can manage up to 5 concurrent client connections (connection IDs 0 through 4).
3. **Q: Why does the ESP8266 require a significant peak power supply?**  
   *A:* Radio frequency (RF) power amplifiers consume high surge currents (up to 300 mA) during 802.11 packet transmission. Weak supplies cause voltage dips below 2.7V, triggering brownout resets.
4. **Q: What is the difference between Station (STA) mode and Access Point (AP) mode?**  
   *A:* In Station mode, the device acts as a client connecting to an existing wireless router. In AP mode, the device creates its own wireless network that other client devices can join.
5. **Q: What is DHCP, and why is `AT+CWDHCP=1,1` executed?**  
   *A:* DHCP (Dynamic Host Configuration Protocol) automatically assigns IP addresses, subnet masks, and default gateways to clients. `AT+CWDHCP=1,1` instructs the ESP8266 station interface to request network parameters dynamically.
6. **Q: How does the STM32 know when an incoming data packet has arrived from a client?**  
   *A:* The ESP8266 sends an asynchronous unsolicited response frame formatted as `+IPD,<link_id>,<length>:<data>` over the UART interface.
7. **Q: What is the default TCP port used for HTTP communication?**  
   *A:* Port 80 is the standard port for unencrypted Hypertext Transfer Protocol (HTTP).
8. **Q: Which APB bus clocks USART2 in the STM32F103 architecture?**  
   *A:* USART2 is clocked by the APB1 peripheral bus, operating at a maximum frequency of 36 MHz (half of the 72 MHz system clock).

## 20. File Descriptions
| File Name | Location | Description |
|---|---|---|
| `main.c` | `Core/Src/` | Contains dual-UART setup, automated AT command sequence, and modem management |
| `main.h` | `Core/Inc/` | Pin and peripheral definitions, clock macros, and standard header declarations |
| `UART_wemos.ioc` | Project Root | STM32CubeMX graphical configuration for USART1 (PC) and USART2 (ESP8266) |
| `UART_wemos.uvprojx` | `MDK-ARM/` | Keil MDK project configuration, compiler settings, and linker configuration |
| `circuit.png` | Project Root | Schematic diagram illustrating dual-UART wiring between STM32, Wemos, and PC |
| `flowchart.png` | Project Root | Sequential flowchart depicting the step-by-step AT command state progression |
