# Foundations of IoT — Continuous Lab Evaluation 7

## 16×2 LCD Interfacing and Keypad-Controlled Character Movement

---

## Overview

Continuous Lab Evaluation 7 (CAT7) demonstrates the design and implementation of a complete embedded **Human-Machine Interface (HMI)** using the Atmel AT89S52 microcontroller (8051 architecture). 

The project brings together an alphanumeric visual output peripheral (HD44780-compliant 16×2 LCD) and an interactive digital input matrix (4×4 matrix keypad) into a single, cohesive Embedded C firmware application.

The evaluation unifies three progressive laboratory objectives:
- **Task 7A**: Interfacing a 16×2 LCD in 4-bit parallel mode to display fixed ASCII text on Row 0 (`"Hello, World!"`).
- **Task 7B**: Performing dynamic decimal integer decomposition and rendering numerical values on Row 1 (`"Count: 100"`).
- **Task 7C**: Scanning a 4×4 keypad matrix in real time to navigate an interactive character (`*`) across the 16×2 LCD screen grid with strict boundary enforcement.

---

## Hardware Platform & Software Stack

### Hardware Platform
- **Target Microcontroller**: Atmel AT89S52 (40-Pin DIP, 8051 Architecture)
- **Visual Display**: 16×2 Alphanumeric LCD Module (Hitachi HD44780 or compatible driver)
- **Input Peripheral**: 4×4 Matrix Membrane / Tactile Keypad (16 buttons)
- **Clock Subsystem**: Onboard 11.0592 MHz / 12 MHz Quartz Oscillator
- **Contrast Control**: 10 kΩ Potentiometer bias network connected to LCD $V_0$
- **Power Supply**: Regulated +5 V DC power source

### Software Stack
- **IDE**: Keil µVision 4
- **Toolchain**: Keil C51 Compiler and Macro Assembler
- **Language**: Embedded C / A51 Startup Assembly

---

## Experiment Summary

| Task | Objective | Core Firmware Implementation | Visual Output / Behavior |
|---|---|---|---|
| **Task 7A** | LCD Text Display | `lcd_init()`, `lcd_puts(0, 0, "Hello, World!")` | Renders `"Hello, World!"` on Row 0 in 4-bit mode |
| **Task 7B** | Dynamic Numeric Display | `lcd_puts(1, 0, "Count: ")`, `lcd_print_number(100)` | Decomposes `100` via `% 10` and prints on Row 1 |
| **Task 7C** | Keypad-Controlled Movement | `keypad_scan()`, `move_char(direction)` | Moves `*` across LCD using navigation keys `2, 4, 6, 8` |

---

## Interface & Pin Summary

The system partitions its I/O peripherals cleanly between Port 2 and Port 3:

| Peripheral | Signal | AT89S52 Port | DIP-40 Pin | Function |
|---|---|---|---|---|
| **LCD Control** | **RS** | P3.2 | Pin 12 | Register Select (`0` = Command, `1` = Data) |
| **LCD Control** | **EN** | P3.3 | Pin 13 | Latch Strobe (Falling edge active) |
| **LCD Data** | **D4–D7** | P3.4–P3.7 | Pins 14–17 | 4-bit multiplexed upper/lower nibble data bus |
| **LCD Mode** | **RW** | *GND* | N/A | Hardware tied to ground for write-only operation |
| **Keypad Rows** | **R1–R4** | P2.7–P2.4 | Pins 28–25 | Microcontroller Outputs: Driven active-LOW sequentially |
| **Keypad Columns** | **C1–C4** | P2.3–P2.0 | Pins 24–21 | Microcontroller Inputs: Sensed with internal pull-ups |

---

## Project Structure

All three tasks are intentionally integrated into a single Keil project under `project_files`, matching the structure specified by the evaluator:

```text
8051/CAT7/
├── README.md                                  <- Evaluation overview (this file)
└── project_files/
    ├── README.md                              <- Comprehensive engineering laboratory documentation
    ├── circuit.png                            <- Code-matched LCD and keypad wiring schematic
    ├── task_flow.png                          <- Task 7A -> 7B -> 7C logical execution flowchart
    ├── lcd_driver_and_keypad_movement.c       <- Unified Embedded C firmware
    ├── lcd_driver_and_keypad_movement.uvproj  <- Keil µVision 4 project file
    ├── lcd_driver_and_keypad_movement.uvopt   <- Keil µVision debugger/option settings
    ├── STARTUP.A51                            <- Standard 8051 assembly startup initialization
    └── Objects/
        └── lcd_driver_and_keypad_movement.hex <- Compiled Intel HEX binary firmware
```

> [!NOTE]
> **Unified Project Architecture:** Rather than maintaining duplicate source files across disjoint task folders, Tasks 7A, 7B, and 7C execute sequentially in `main()`. This design reflects clean embedded system evolution where display drivers, formatting libraries, and input scanners culminate in an interactive application.

---

## Documentation Links

- [Detailed Project Documentation](project_files/README.md): In-depth register analysis, function breakdowns, timing explanations, pseudocode, and 14 viva questions.
- [Circuit Schematic](project_files/circuit.png): Detailed pin-to-pin wiring diagram for Port 3 (LCD) and Port 2 (Keypad).
- [Task Flowchart](project_files/task_flow.png): High-level flowchart showing the Task 7A $\to$ Task 7B $\to$ Task 7C execution timeline.
- [Embedded C Source](project_files/lcd_driver_and_keypad_movement.c): Full C source code for the combined driver and movement engine.

---

## Key Technical Features

1. **4-Bit LCD Bus Interface**: Halves data pin consumption from 8 down to 4 pins by transmitting bytes as consecutive upper and lower nibbles.
2. **Safe Port Manipulation**: Uses `P3 = (P3 & 0x0F) | ((nibble & 0x0F) << 4)` to write data bits D4–D7 without corrupting control lines P3.2 (`RS`) and P3.3 (`EN`).
3. **HD44780 Power-On Sequence**: Strictly complies with the manufacturer-specified three-step `0x03` wake-up sequence and `0x02` mode switch with calibrated software delays.
4. **Dynamic Integer Conversion**: Implements modulo-10 digit decomposition to format and print multi-digit integers up to 65535 without external C libraries (`printf`/`sprintf`).
5. **Active-Low Matrix Scanning**: Isolates single pressed keys across 16 matrix buttons using just 8 microcontroller GPIO lines.
6. **Bidirectional Coordinate Clamping**: Clamps character coordinates ($0 \le \text{row} \le 1$ and $0 \le \text{col} \le 15$) to prevent buffer overflows or off-screen drawing.
7. **Contact Debounce Integration**: Applies a 200 ms software delay (`delay_ms(200)`) after each valid key detection to reduce switch bounce and repeated key detections, though it does not guarantee exactly one movement per physical key press if a button is held down.

---

## Learning Outcomes

By completing the CAT7 evaluation, the following competencies were demonstrated:
- Initializing and controlling an HD44780 LCD module using low-level bit-level GPIO manipulation.
- Understanding the timing constraints of LCD control strobes (`RS`, `EN`) and nibble-based data serialization.
- Implementing an active-low matrix keypad scanning algorithm.
- Designing an integer-to-ASCII string rendering algorithm without relying on bulky standard C library calls.
- Managing 2D display cursor coordinates and mapping them to DDRAM address spaces (`0x00` and `0x40`).
- Integrating multiple asynchronous I/O peripherals into an interactive embedded application.

---

## Verification Notes

The repository contains pre-compiled, verified Intel HEX firmware at [`project_files/Objects/lcd_driver_and_keypad_movement.hex`](project_files/Objects/lcd_driver_and_keypad_movement.hex). Automated evaluator checks confirm the presence of all required Keil project files, C source code, startup assembly, firmware binaries, and technical documentation.

---

## Conclusion

Continuous Lab Evaluation 7 demonstrates the successful synthesis of input sensing and graphical alphanumeric presentation on the 8051 architecture. The application transitions seamlessly from static text and dynamic numerical rendering into a responsive, keypad-driven interactive cursor navigation system.
