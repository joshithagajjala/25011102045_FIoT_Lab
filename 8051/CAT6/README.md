# Foundations of IoT — Continuous Lab Evaluation 6

## UART Serial Communication Using AT89S52

---

## Overview

Continuous Lab Evaluation 6 (CAT6) explores asynchronous serial communication using the integrated hardware **Universal Asynchronous Receiver/Transmitter (UART)** peripheral of the Atmel AT89S52 microcontroller (8051 architecture). 

The evaluation consists of two progressive experiments:
- **Task A (Task6A)**: Fixed-string serial transmission (`"Hello from 8051\r\n"`) to a host PC serial terminal using an interrupt-driven pipeline.
- **Task B (Task6B)**: Interactive, bidirectional UART echo terminal featuring interrupt-driven character reception into a 32-byte circular ring buffer and immediate character retransmission.

Both experiments leverage **UART Mode 1** (8-bit asynchronous data frame with 1 start bit and 1 stop bit) and **Timer 1 in Mode 2** (8-bit auto-reload) clocked at 11.0592 MHz to synthesize a standard communication rate of **9600 baud**.

---

## Hardware and Software Stack

### Hardware Stack
- **Target Microcontroller**: Atmel AT89S52 (40-Pin DIP, 8051 Architecture)
- **Clock Subsystem**: Onboard 11.0592 MHz Quartz Crystal Oscillator circuit
- **Reset Subsystem**: Onboard power-on and manual tactile reset circuitry
- **PC Communication Interface**: Compatible USB-to-UART Serial Converter (e.g., CP2102, FT232RL, or CH340)
- **Power Supply**: Regulated +5 V DC power source

### Software Stack
- **IDE**: Keil µVision 4
- **Toolchain**: Keil C51 Compiler and Macro Assembler
- **Language**: Embedded C / A51 Startup Assembly
- **Host Serial Terminal**: PuTTY / Tera Term / RealTerm (configured for 9600 baud, 8N1, No Parity, No Flow Control)

---

## Experiment Comparison

| Feature / Parameter | Task A (String Transmission) | Task B (Interactive Echo Terminal) |
|---|---|---|
| **Primary Functional Goal** | Continuous transmission of a fixed ASCII string | Bidirectional reception and character echoing |
| **Microcontroller TXD (P3.1)** | **Active**: Transmits string bytes to PC terminal | **Active**: Transmits echoed characters to PC terminal |
| **Microcontroller RXD (P3.0)** | Enabled in `SCON`, but not processed in software | **Active**: Receives keystrokes from PC terminal |
| **UART Frame Mode** | Mode 1 (8-bit UART, 8N1 framing) | Mode 1 (8-bit UART, 8N1 framing) |
| **Baud Rate Generator** | Timer 1 | Timer 1 |
| **Timer 1 Mode** | Mode 2 (8-bit auto-reload, `TH1 = 0xFD`) | Mode 2 (8-bit auto-reload, `TH1 = 0xFD`, `TL1 = 0xFD`) |
| **Nominal Baud Rate** | 9600 bps (at 11.0592 MHz, SMOD = 0) | 9600 bps (at 11.0592 MHz, SMOD = 0) |
| **Interrupt Vector Used** | Serial Interrupt Vector 4 (`0x0023`) | Serial Interrupt Vector 4 (`0x0023`) |
| **Interrupt Trigger Condition** | Transmit completion (`TI == 1`) | Both Reception (`RI == 1`) and Transmit (`TI == 1`) |
| **Circular Ring Buffer** | Not implemented (direct array indexing) | **Implemented**: 32-byte FIFO queue (`rxBuffer`) |
| **Buffer Overflow Protection**| Not applicable | Flag-based drop protection (`rxOverflow`) |
| **Transmission Synchronization**| Software index auto-wraps on null terminator | Flag-based mutual exclusion (`txBusy`) |
| **Main Loop Operation** | Empty loop (transmission driven entirely by ISR)| Polls buffer (`uart_available()`) and executes echo |

---

## Documentation & Circuit Links

The detailed engineering documentation, timing derivations, register maps, and wiring diagrams for each task are available in their dedicated subdirectories:

- **Task A (Fixed-String Transmission)**:
  - [Task A Technical Documentation](project_files/TASK_A/README.md)
  - [Task A Circuit Diagram](project_files/TASK_A/circuit.png)
  - [Task A Embedded C Source](project_files/TASK_A/uart_string_transmission.c)
- **Task B (Interactive Echo Terminal)**:
  - [Task B Technical Documentation](project_files/TASK_B/README.md)
  - [Task B Circuit Diagram](project_files/TASK_B/circuit.png)
  - [Task B Embedded C Source](project_files/TASK_B/uart_echo_terminal.c)

---

## Core Technical Concepts

### 1. Asynchronous Serial Communication
Asynchronous communication eliminates the need for a shared clock signal line between the sender and receiver. Both nodes are configured to an agreed-upon bit transfer rate (baud rate). Framing bits (1 Start bit and 1 Stop bit) synchronize byte decoding at the receiver on a per-character basis.

### 2. Physical Pin Functions (TXD & RXD)
- **TXD (P3.1, Pin 11)**: Serial output transmitting binary bit patterns serially from the internal transmit shift register.
- **RXD (P3.0, Pin 10)**: Serial input receiving incoming bit patterns and assembling them into complete 8-bit parallel bytes.

### 3. UART Framing (8N1)
Both tasks utilize **8N1** framing under 8051 Mode 1:
$$\text{Total Frame Length} = 1\text{ Start Bit (0)} + 8\text{ Data Bits} + 1\text{ Stop Bit (1)} = 10\text{ bits per character}$$

### 4. Baud Rate Generation via Timer 1 Mode 2
The 8051 hardware UART relies on Timer 1 overflows to clock its internal serializer. Mode 2 provides automatic hardware reloading of `TL1` from `TH1` on overflow, completely eliminating timing jitter:
$$\text{Baud Rate} = \frac{2^{\text{SMOD}}}{32} \times \frac{F_{\text{osc}}}{12 \times (256 - \text{TH1})}$$
For $F_{\text{osc}} = 11.0592\text{ MHz}$, $\text{SMOD} = 0$, and $\text{TH1} = 0\text{xFD} = 253_{10}$:
$$\text{Baud Rate} = \frac{11,059,200}{384 \times (256 - 253)} = \frac{11,059,200}{1152} = 9600\text{ bps}$$

### 5. SBUF and SCON Registers
- **`SBUF` (`0x99`)**: Two physically distinct registers sharing a single SFR address. Writing loads the transmit serializer; reading retrieves the received byte.
- **`SCON` (`0x98`)**: Configured to `0x50` (`0101 0000b`), setting Mode 1 and enabling reception (`REN = 1`).
- **`TI` (`SCON.1`) & `RI` (`SCON.0`)**: Transmit and receive completion flags. They trigger interrupt vector 4 and must be cleared by software.

### 6. Circular Buffer (FIFO) & Producer-Consumer Architecture
Task B introduces an asynchronous circular buffer in internal RAM:
- The **Interrupt Routine** acts as the high-priority producer, inserting bytes at `rxHead`.
- The **Main Loop** acts as the consumer, reading bytes from `rxTail` and re-transmitting them.
- This decouples receipt from processing, preventing data loss when keystroke bursts arrive.

---

## Learning Outcomes

By completing the CAT6 evaluation tasks, the following embedded competencies were developed:
1. **Peripheral Configuration**: Initializing 8051 special-function registers (`SCON`, `TMOD`, `PCON`, `IE`) for serial operation.
2. **Deterministic Baud Rate Synthesis**: Deriving precise timer reload values for classic crystal frequencies (11.0592 MHz).
3. **Interrupt-Driven Transmit Pipelines**: Orchestrating non-blocking character transmission using hardware `TI` flags.
4. **Serial Keystroke Ingestion**: Servicing incoming asynchronous data bursts via hardware `RI` interrupts.
5. **Data Queue Management**: Designing and debugging a ring buffer with wrap-around pointers and capacity bounds.
6. **Hardware Mutual Exclusion**: Utilizing state flags (`txBusy`) to synchronize shared peripheral access.
7. **Host Interfacing**: Connecting microcontrollers to PC serial terminals and avoiding local echo duplication.
8. **Toolchain Diagnostics**: Identifying cross-compiler requirements (C51 vs. ARM) in mixed embedded environments.

---

## Important Laboratory Notes

- **Onboard Infrastructure**: The AT89S52 development board integrates its own clock oscillator and reset networks; no external crystal or pull-up wiring was configured on a breadboard.
- **Voltage Compatibility**: Standard 8051 TTL UART outputs operate at 5 V. Level translation must be considered when interfacing with 3.3 V USB-to-UART converters. Direct connection to $\pm 12\text{ V}$ RS-232 ports is strictly prohibited.
- **Compiled Firmware Artifacts**: Verified compiled Intel HEX firmware files are tracked in the repository for both experiments (`TASK_A/Objects/uart_string_transmission.hex` and `TASK_B/Objects/uart_echo_terminal.hex`).
- **Environment & Build Note**: In development environments lacking the licensed Keil C51 compiler, existing HEX files ensure full functionality without requiring project modification or compilation workarounds.

---

## Conclusion

Continuous Lab Evaluation 6 establishes the fundamentals of embedded serial communication. Starting from automated, interrupt-driven fixed-string broadcasting in Task A, the lab advances in Task B to full-duplex interactive communication incorporating ring buffering, overflow detection, and asynchronous producer-consumer task separation.
