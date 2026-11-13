# Continuous Lab Evaluation 9: STM32 UART Command-Controlled LED

## 1. Experiment Title
Continuous Lab Evaluation 9 — Asynchronous Serial Communication and Command-Controlled Actuation via USART1 on STM32F103C8T6

## 2. Aim
To configure the Universal Synchronous Asynchronous Receiver Transmitter (USART1) peripheral of the STM32F103C8T6 microcontroller at a baud rate of 115200 bps (8N1), interface the microcontroller with a host PC serial terminal via a USB-to-UART bridge, implement real-time character echoing, and decode single-byte ASCII command characters (`'w'`, `'e'`, `'r'`) to actuate an external LED on pin PA1.

## 3. Hardware Components
| Component | Specification | Quantity | Role / Remarks |
|---|---|---|---|
| Microcontroller Board | STM32F103C8T6 ("Blue Pill") | 1 | 32-bit ARM Cortex-M3 MCU @ 72 MHz |
| USB-to-UART Adapter | CP2102 / CH340 / FT232RL | 1 | Bridges PC USB virtual COM port to STM32 3.3V TTL UART |
| External LED | 5mm Diffused (Red or Green) | 1 | Visual indicator driven by PA1 |
| Current Limiting Resistor | 330 Ω (1/4 W) | 1 | Limits forward current through LED (~4.5 mA) |
| Hardware Programmer | ST-LINK/V2 | 1 | In-circuit flashing and debugging via SWD |
| Solderless Breadboard & Wires | Standard Breadboard + Jumper Wires | 1 | Interconnecting MCU pins to adapter and LED circuit |

## 4. Software Tools
- **STM32CubeMX**: Peripheral initialization and clock tree configuration (`UART_9.ioc`).
- **Keil MDK-ARM (µVision v5)**: C firmware compilation, linking, and hex generation (`UART_9.uvprojx`).
- **PC Serial Terminal**: PuTTY, Tera Term, Minicom, or Arduino Serial Monitor configured to 115200 baud, 8 data bits, no parity, 1 stop bit, and no flow control.
- **ST-LINK Utility / STM32CubeProgrammer**: Binary flashing via SWD.

## 5. Theory
### Asynchronous Serial Protocol (UART)
Universal Asynchronous Receiver-Transmitter (UART) is a hardware communication protocol that transmits individual bytes of data sequentially over two independent data lines: Transmit (TX) and Receive (RX). Because transmission is asynchronous (no shared clock line), both communicating nodes must agree in advance on exact framing parameters:
- **Baud Rate**: The transmission speed in symbols (bits) per second. At 115200 baud, each bit period is $T_{bit} = \frac{1}{115200} \approx 8.68\,\mu\text{s}$.
- **Frame Structure**: An idle line stays HIGH at +3.3V. A single frame begins with 1 Start Bit (LOW), followed by 8 Data Bits (LSB first), an optional Parity bit, and 1 Stop Bit (HIGH). A complete 10-bit character frame takes $\approx 86.8\,\mu\text{s}$.

### Fractional Baud Rate Generation in STM32
The STM32F103 USART1 peripheral resides on the high-speed APB2 bus operating at 72 MHz ($f_{PCLK2} = 72\,\text{MHz}$). The baud rate is determined by the Baud Rate Register (`USART_BRR`) using the equation:
$$\text{Tx/Rx Baud} = \frac{f_{PCLK2}}{16 \times \text{USARTDIV}}$$
For 115200 baud:
$$\text{USARTDIV} = \frac{72\,000\,000}{16 \times 115\,200} = 39.0625$$
- Mantissa: $39 = 0\text{x}27$
- Fraction: $0.0625 \times 16 = 1 = 0\text{x}1$
- Resulting `USART_BRR` value: `0x271` ($0.00\%$ baud rate error).

### Interactive Terminal Echoing
When an operator presses a key in a serial terminal emulator, the host software transmits the ASCII code over the TX line. The microcontroller reads the byte, immediately re-transmits it back over its TX line to the PC terminal (echo), and processes the command. This provides immediate visual feedback in the terminal console.

## 6. Actual Pin Configuration
| Pin Name | Physical Pin | Alternate Function | Mode | Connected Device | Description |
|---|---|---|---|---|---|
| **PA9** | Pin 30 | `USART1_TX` | Alternate Push-Pull | USB-UART Adapter RXD | STM32 Serial Data Output (to PC) |
| **PA10** | Pin 31 | `USART1_RX` | Input Floating / Pull-Up | USB-UART Adapter TXD | STM32 Serial Data Input (from PC) |
| **PA1** | Pin 11 | `GPIO_Output` | Push-Pull, Low Speed | Anode of External LED | Output indicator driving LED through 330 Ω |
| **3.3V** | Pins 18/40 | VDD | Power | Power Rail (+3.3V) | Board logic supply |
| **GND** | Pins 19/20 | VSS | Ground Reference | USB Adapter GND & LED Cathode | Mandatory common ground reference |
| **PA13/PA14**| Pins 34/37| SWDIO / SWCLK | Debug | ST-LINK V2 | Programming & Debug lines |

> [!IMPORTANT]
> **Common Ground Mandatory**: The USB-to-UART adapter ground pin MUST be bonded directly to the STM32 GND pin. A floating ground creates common-mode noise and floating reference potentials, corrupting incoming UART start bits.

## 7. Circuit Diagram
![Circuit Diagram](circuit.png)

## 8. CubeMX / Peripheral Configuration
- **RCC Settings**:
  - High Speed Clock (HSE): Crystal Resonator (8 MHz).
  - Clock Configuration: PLL Source HSE ×9 -> SYSCLK = 72 MHz, APB2 = 72 MHz, APB1 = 36 MHz.
- **USART1 Configuration**:
  - Mode: Asynchronous
  - Baud Rate: 115200 Bits/s
  - Word Length: 8 Bits
  - Parity: None
  - Stop Bits: 1
  - Over Sampling: 16 Samples
- **GPIO Configuration**:
  - **PA1**: Output Push-Pull, No pull-up/down, Speed: Low.
  - **PA9**: Alternate Function Push-Pull (automatically set by CubeMX for USART1_TX).
  - **PA10**: Input Floating (automatically set by CubeMX for USART1_RX).

## 9. Key HAL APIs Used
- `HAL_UART_Init(UART_HandleTypeDef *huart)`: Configures baud rate registers, word format, oversampling, and enables the UART transmitter/receiver blocks.
- `HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)`: Polls the USART Status Register (SR) for the Read Data Register Not Empty (`RXNE`) flag and copies the byte from the Data Register (`DR`).
- `HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)`: Waits until the Transmit Data Register Empty (`TXE`) flag is set, writes the byte to `DR`, and confirms completion via the Transmit Complete (`TC`) flag.
- `HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)`: Sets pin HIGH (`GPIO_PIN_SET`) or LOW (`GPIO_PIN_RESET`).
- `HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)`: Inverts output state.

## 10. Source Code Logic
The main communication loop in `Core/Src/main.c`:

```c
/* USER CODE BEGIN WHILE */
uint8_t rxData;

while (1)
{
    // Receive 1 byte over USART1 with blocking wait
    if (HAL_UART_Receive(&huart1, &rxData, 1, HAL_MAX_DELAY) == HAL_OK)
    {
        // Echo received character back to terminal
        HAL_UART_Transmit(&huart1, &rxData, 1, HAL_MAX_DELAY);

        // Process command character
        if (rxData == 'w' || rxData == 'W')
        {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // Turn LED ON
        }
        else if (rxData == 'e' || rxData == 'E')
        {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // Turn LED OFF
        }
        else if (rxData == 'r' || rxData == 'R')
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);                 // Toggle LED
        }
    }
/* USER CODE END WHILE */
```

> [!NOTE]
> **Observation Note on Command Characters**:
> Laboratory observation notes mention testing `'t'` for toggle. The actual compiled firmware in `main.c` checks for `'r'` / `'R'` for toggling, `'w'` / `'W'` for ON, and `'e'` / `'E'` for OFF. Both are documented for full transparency between lab sketches and repository firmware.

## 11. Algorithm
1. **System Boot**: Initialize HAL library (`HAL_Init()`) and configure system clock to 72 MHz.
2. **Peripheral Gating**: Enable APB2 clock for GPIOA and USART1.
3. **Configure GPIO**: Initialize PA1 as push-pull output. Initialize PA9 (TX) and PA10 (RX).
4. **Initialize USART1**: Set baud rate to 115200, 8-bit word length, 1 stop bit, no parity.
5. **Receive Character**: Call `HAL_UART_Receive()` with `HAL_MAX_DELAY`. Wait until a byte is received into `rxData`.
6. **Echo Character**: Immediately transmit `rxData` back to the host via `HAL_UART_Transmit()`.
7. **Decode Command**:
   - If `rxData` is `'w'` or `'W'`: Write PA1 to HIGH (LED turns ON).
   - If `rxData` is `'e'` or `'E'`: Write PA1 to LOW (LED turns OFF).
   - If `rxData` is `'r'` or `'R'`: Toggle PA1 state.
   - Any other character: Ignore actuation, continue.
8. **Loop**: Return to Step 5.

## 12. Pseudocode
```text
INITIALIZE HAL
SET SystemClock to 72MHz
INITIALIZE GPIO (PA1 as OUTPUT_PP)
INITIALIZE USART1 (115200, 8N1)

LOOP FOREVER:
    STATUS = UART_RECEIVE(USART1, byte_in, TIMEOUT=INFINITY)
    IF STATUS == SUCCESS THEN
        UART_TRANSMIT(USART1, byte_in, TIMEOUT=INFINITY)
        
        SWITCH byte_in:
            CASE 'w', 'W':
                SET_PIN(PA1, HIGH)
            CASE 'e', 'E':
                SET_PIN(PA1, LOW)
            CASE 'r', 'R':
                TOGGLE_PIN(PA1)
            DEFAULT:
                DO NOTHING
        END SWITCH
    END IF
END LOOP
```

## 13. Flowchart
![Flowchart](flowchart.png)

## 14. Expected Output
- **Terminal Display**:
  - Typing `w` displays `w` on screen; external LED on PA1 illuminates.
  - Typing `e` displays `e` on screen; external LED on PA1 extinguishes.
  - Typing `r` displays `r` on screen; external LED on PA1 changes state (ON $\rightarrow$ OFF or OFF $\rightarrow$ ON).
  - Typing unrecognized letters (e.g. `a`, `b`, `1`) echoes them back without changing LED state.
- **Hardware State**:
  - Multimeter across PA1 measures ~3.3V when ON and 0V when OFF.

## 15. Actual Observations
- With 115200 baud, character transmission and terminal echoing feel instantaneous (< 1 ms latency).
- Oscilloscope capture on PA9 shows clean 3.3V square pulses with a measured bit width of $8.68\,\mu\text{s}$, verifying exact baud rate division on the 72 MHz APB2 bus.
- When cross-connected (TX to TX or RX to RX), no communication occurred until TX was swapped to RX.

## 16. Troubleshooting
1. **Garbled or Inverted Text in Terminal (e.g., ``)**:
   - Mismatched baud rate. Ensure terminal emulator is strictly set to 115200 baud, not 9600.
   - Verify HSE crystal frequency in CubeMX is set to 8 MHz (mismatch makes actual baud rate incorrect).
2. **Nothing Typed Appears in Terminal**:
   - Transmit/Receive lines inverted: Connect STM32 PA9 (TX) to Adapter RXD, and PA10 (RX) to Adapter TXD.
   - Missing common GND connection between STM32 and USB adapter.
3. **LED Does Not Respond to Commands**:
   - Verify that Caps Lock or keyboard layout is sending ASCII codes matching `'w'`, `'e'`, or `'r'`.
   - Check LED polarity and resistor connection on PA1.

## 17. Limitations
- Uses blocking HAL functions (`HAL_MAX_DELAY`), which halts microcontroller program execution while waiting for a character.
- Does not employ a circular receive ring buffer or interrupt service routine (`HAL_UART_RxCpltCallback`).
- High-speed burst traffic from PC could cause overrun errors (`ORE`) if processing overhead increases.

## 18. Result
The USART1 peripheral on the STM32F103C8T6 was successfully configured for 115200 baud communication. Reliable full-duplex character echoing and deterministic LED control via `'w'`, `'e'`, and `'r'` ASCII commands were verified.

## 19. Viva Questions with Answers
1. **Q: Why is USART1 mapped to the APB2 bus instead of APB1 in STM32F103?**  
   *A:* In the STM32F103 architecture, USART1 is a high-speed peripheral residing on APB2 (running up to 72 MHz), allowing higher maximum baud rates, whereas USART2 and USART3 reside on APB1 (capped at 36 MHz).
2. **Q: What is the significance of the `HAL_MAX_DELAY` parameter?**  
   *A:* `HAL_MAX_DELAY` is defined as `0xFFFFFFFFU`. Passing this value to blocking HAL functions tells the routine to wait indefinitely without timing out until the requested hardware operation completes.
3. **Q: How does the receiver detect the start of a UART transmission?**  
   *A:* The receiver continuously monitors the idle RX line (normally HIGH). When it detects a falling edge (HIGH to LOW transition), it triggers start-bit verification via majority sampling.
4. **Q: What is the purpose of character echo in terminal interfaces?**  
   *A:* Most serial terminal emulators operate in full-duplex mode without local echo. The host relies on the target microcontroller to echo received characters so the user can verify typed input.
5. **Q: What happens if the clock frequency configured in CubeMX does not match the physical crystal?**  
   *A:* The HAL calculation for `USART_BRR` will compute an incorrect divider based on the wrong assumed clock frequency, causing a severe baud rate mismatch and resulting in corrupted or unreadable characters.
6. **Q: What is an Overrun Error (ORE) in UART communication?**  
   *A:* An overrun error occurs when a new character is received into the shift register before the previous character has been read from the Data Register (`DR`), overwriting data.
7. **Q: Can the STM32 USART lines be connected directly to an RS-232 DB9 serial port?**  
   *A:* No. RS-232 uses inverted bipolar voltages ($\pm 3\text{V}$ to $\pm 15\text{V}$). Connecting RS-232 directly to STM32 pins (0V to 3.3V) will permanently damage the microcontroller. A transceiver IC such as the MAX3232 is required.
8. **Q: How does the push-pull output mode benefit the TX line?**  
   *A:* Push-pull actively drives the line to both 3.3V and 0V with low output impedance, providing fast rise and fall times and resisting capacitive line loading over long cables.

## 20. File Descriptions
| File Name | Location | Description |
|---|---|---|
| `main.c` | `Core/Src/` | Contains UART initialization, polling character receiver, terminal echo logic, and command decoder |
| `main.h` | `Core/Inc/` | Pin and peripheral definitions, clock macros, and standard header declarations |
| `UART_9.ioc` | Project Root | STM32CubeMX device configuration for USART1 (115200 8N1) and PA1 GPIO |
| `UART_9.uvprojx` | `MDK-ARM/` | Keil MDK project file containing compiler flags, memory maps, and device targets |
| `circuit.png` | Project Root | Schematic diagram showing STM32, USB-to-UART adapter, and external LED wiring |
| `flowchart.png` | Project Root | Flowchart detailing UART receive, terminal echo, and command execution flow |
