# Continuous Lab Evaluation 9: STM32 UART Command-Controlled LED

## 1. Overview
Continuous Lab Evaluation 9 establishes asynchronous serial communication between the STM32F103C8T6 microcontroller and a host computer serial console using the hardware USART1 peripheral at 115200 baud. The firmware reads incoming ASCII keystrokes, provides visual terminal feedback via character echoing, and translates specific single-character commands into deterministic physical actuation of an external LED on pin PA1.

## 2. Objective
- Configure STM32 USART1 in full-duplex asynchronous mode (115200 baud, 8 data bits, no parity, 1 stop bit).
- Wire a USB-to-UART bridge adapter between the PC and the STM32 Blue Pill development board with a shared common ground.
- Implement full-duplex character echoing to provide immediate operator feedback in PC terminal software.
- Parse ASCII command characters:
  - `'w'` or `'W'`: Turn external LED ON (PA1 = HIGH).
  - `'e'` or `'E'`: Turn external LED OFF (PA1 = LOW).
  - `'r'` or `'R'`: Toggle external LED state (PA1 invert).

## 3. Hardware and Software Used
- **Target MCU**: STM32F103C8T6 (ARM Cortex-M3 @ 72 MHz)
- **Serial Bridge**: USB-to-UART converter module (CP2102 / CH340 / FT232RL)
- **Actuator**: 5mm diffused LED with 330 Ω series resistor connected to PA1
- **Host Software**: Serial terminal emulator (PuTTY / Tera Term / Minicom)
- **Toolchain & Config**: STM32CubeMX (`UART_9.ioc`) and Keil MDK-ARM µVision v5 (`UART_9.uvprojx`)
- **Binary**: `UART_9.hex` loaded via ST-LINK/V2 SWD interface

## 4. Main Peripherals Used
- **USART1**:
  - `PA9`: Transmitter output (`USART1_TX`) mapped to USB adapter RXD.
  - `PA10`: Receiver input (`USART1_RX`) mapped to USB adapter TXD.
  - Bus: High-speed APB2 bus (72 MHz clock frequency).
- **GPIO Port A**:
  - `PA1`: Configured as push-pull output driving the external LED circuit.

## 5. Summary of Actual Implementation
The application repeatedly polls USART1 using `HAL_UART_Receive()` with `HAL_MAX_DELAY`. When a byte arrives from the terminal emulator:
1. It is re-transmitted back to the host via `HAL_UART_Transmit()` to produce terminal character echo.
2. The character is compared against command letters:
   - `'w'` / `'W'` drives PA1 to `GPIO_PIN_SET`.
   - `'e'` / `'E'` drives PA1 to `GPIO_PIN_RESET`.
   - `'r'` / `'R'` inverts PA1 using `HAL_GPIO_TogglePin()`.
3. Non-command characters are echoed harmlessly without changing the LED state.

## 6. Pin Summary Table
| Pin | Function | Peripheral | Connected Device | Purpose |
|---|---|---|---|---|
| **PA9** | Output | USART1_TX | USB-UART Adapter RXD | Microcontroller serial transmit to PC |
| **PA10** | Input | USART1_RX | USB-UART Adapter TXD | Microcontroller serial receive from PC |
| **PA1** | Output | GPIO_Output | LED Anode via 330 Ω | Visual output indicator |
| **3.3V** | Power | VDD | Power Rail | Logic HIGH supply voltage |
| **GND** | Ground | VSS | Adapter GND & LED Cathode | Shared common ground reference |
| **PA13** | Debug | SWDIO | ST-LINK V2 SWDIO | Serial Wire Data input/output |
| **PA14** | Debug | SWCLK | ST-LINK V2 SWCLK | Serial Wire Clock line |

## 7. Links to Artifacts and Detailed Documentation
- [Detailed Project Engineering Report](project_files/README.md)
- [Circuit Schematic Diagram](project_files/circuit.png)
- [Firmware Logic Flowchart](project_files/flowchart.png)
- [Primary Application Source Code](project_files/Core/Src/main.c)
- [STM32CubeMX Project Configuration](project_files/UART_9.ioc)

## 8. Key Observations
- APB2 72 MHz clocking yielded exact integer division for 115200 baud with 0% clock error.
- Full character echo creates a fluid, highly responsive command-line terminal experience.
- The repository source strictly uses `'w'`, `'e'`, and `'r'`, resolving experimental notes that also explored `'t'` for toggle.

## 9. Learning Outcomes
- Understood asynchronous serial framing (start bit, data payload, stop bit) and baud divisor registers.
- Gained experience interfacing STM32 hardware UART with external PC host serial terminals.
- Implemented character decoding and state machine dispatching based on ASCII inputs.

## 10. Verification Note
All documented pin assignments, baud rate parameters, and command decoding routines have been verified against `Core/Src/main.c` and validated against the laboratory evaluation rubric.

## 11. Conclusion
Continuous Lab Evaluation 9 was successfully verified. The STM32F103C8T6 reliably communicates over USART1 at 115200 baud, executes bidirectional character echoing, and correctly decodes commands to actuate the external LED.
