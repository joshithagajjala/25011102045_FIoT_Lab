# Continuous Assessment Test 2 (CAT2) — TASK2B

### EXP
CAT2 — Continuous Assessment Test 2  
*(Shiv Nadar University, Chennai — Department of Computer Science)*  
*Course: 25CS2741 Fundamentals of Internet of Things + Lab*

---

### DATE
20/07/2026 (Week 2)  
*(Official evaluation date specified in the CAT2 question paper)*

---

### NAME OF EXP
Bare-Metal LED Blinking Using AT89S52 Microcontroller  
*(Official Course Topic: 8051 Tool Chain setup – Development Board – AT89S52: Bare-Metal LED Blink)*

---

### HARDWARE SETUP / PLATFORM

#### 1. Confirmed Hardware Platform
- **Microcontroller**: Atmel / Microchip AT89S52 (8-bit 8051 / MCS-51 CISC architecture)
- **Development Board**: AmBTech Labs 8051 Development Board (identified via PCB silk-screen marking)
- **Clock Frequency**: 11.0592 MHz crystal oscillator (crystal load capacitors integrated on board, exact values unverified)
- **Target GPIO Pin**: Port 1, specifically Pin 1 (P1.0)
- **Power Supply**: Nominal 5.0 V DC (operating rail voltage unmeasured with multimeter; not treated as a failure)
- **Dual LED Setup**:
  1. **Onboard User LED**: Integrated on the AmBTech Labs development board and associated with P1.0.
  2. **External Breadboard LED**: Connected externally to P1.0 through a series current-limiting resistor to ground (`P1.0 → R_LED → LED Anode → LED Cathode → GND`).
- **Resistor Specification**: Current-limiting resistor labeled `R_LED (value unverified)` (recalled by student; resistance not independently measured or verified as 330 Ω).

#### 2. Toolchain and Programming Hierarchy
- **Software IDE & Toolchain**: Keil µVision (C51 Compiler, A51 Macro Assembler, BL51 Linker/Locator). Used for writing code, building `STARTUP.A51` and `main.c`, and generating Intel HEX firmware (`Objects/led_test.hex`).
- **Device Flashing Utility**: ProgISP v1.72 (`progisp.exe`). A Windows PC utility used to load the compiled HEX file over USB into the microcontroller's Flash memory.
- **Physical ISP Programmer**: Dedicated hardware device programmer (specific model unconfirmed; e.g., USBasp or compatible ISP programmer).

> [!NOTE]
> **Toolchain Distinction:** Keil µVision is an IDE and compiler toolchain, while ProgISP v1.72 is a flashing software utility. Neither is a hardware programmer. Physical in-system programming requires an external ISP hardware programmer interfacing with the AT89S52 ISP pins (MOSI/P1.5, MISO/P1.6, SCK/P1.7, RST).

> [!IMPORTANT]
> **Byte-Wide Port Register Manipulation:** In `main.c`, the statements `P1 = 0x00;` and `P1 = 0xFF;` assign values to the entire 8-bit Port 1 Special Function Register (SFR address `0x90`). This simultaneously drives all eight pins (P1.0 through P1.7), rather than masking or bit-addressing P1.0 alone (`sbit`).

#### 3. Hardware Bring-Up Sheet (Question Paper Deliverables)

| Parameter | Laboratory Entry / Status | Verification Notes |
| :--- | :--- | :--- |
| **MCU** | Microchip / Atmel AT89S52 | Confirmed from IC package marking |
| **CPU Core** | 8-bit 8051 (MCS-51 Architecture) | Confirmed architecture |
| **Development Board** | AmBTech Labs 8051 Development Board | Confirmed from PCB silk-screen marking |
| **Operating Voltage** | Nominal 5.0 V | Unmeasured with DMM in lab; marked as unverified (not a failure) |
| **Clock Frequency** | 11.0592 MHz crystal oscillator | Confirmed from crystal marking on development board |
| **Debug / Program Interface** | In-System Programming (ISP) | Flashed via ProgISP v1.72; physical programmer hardware model unconfirmed |
| **LED GPIO Port** | Port 1 (`P1`, SFR Address `0x90`) | Confirmed from code register assignment |
| **LED GPIO Pin** | Pin 1 (`P1.0`) | Confirmed target pin; firmware toggles byte-wide across P1.0–P1.7 |
| **LED Indicators Present** | Dual indicators: (1) Onboard LED, (2) External LED | Confirmed: both LEDs observed blinking simultaneously |
| **External LED Circuit** | `P1.0 → R_LED → Anode → Cathode → GND` | Active-High to GND (student reported wiring; `R_LED` value unverified) |
| **Onboard LED Circuit** | Associated with P1.0 on PCB | Internal PCB routing / polarity unverified |
| **Observed Physical State** | Both LEDs observed blinking simultaneously | Confirmed physical observation during experiment |
| **LED Blink Frequency (Hz)** | Intended ~1 Hz (Nominal ~500 ms ON / ~500 ms OFF) | Unmeasured with oscilloscope; marked as unverified software target |

---

### TASK NO
TASK2B

#### Task Objectives and Requirements (According to CAT2 Question Paper)
As mandated in the official continuous evaluation prompt:
- **Core Activity**: Compile a minimal bare-metal Embedded C program, flash the AT89S52 microcontroller, and control an LED by directly accessing the microcontroller's memory-mapped GPIO registers.
- **Mandatory Constraint**: Strictly NO to using Arduino APIs, HAL libraries, or RTOS services!
- **Evaluation Requirements**:
  1. **Hardware Board Bring-Up**:
     - Identify microcontroller, programming interface, power source, user LED, and relevant GPIO pin.
     - Verify board power rails using a digital multimeter.
     - Identify LED GPIO port and pin from the datasheet and schematic.
     - Establish communication between the host PC and target MCU via the programming toolchain.
  2. **Bare-Metal LED Blink**:
     - Configure the LED GPIO as an output using direct register access.
     - Implement a delay mechanism using a simple software loop.
     - Blink the LED at approximately 1 Hz (50% duty cycle).
  3. **Custom Library Function**: Develop a custom library/function for software delay functionality (`delay(unsigned int msec)`).

---

### Circuit Diagram

![Circuit Diagram](circuit.png)

#### Dual LED Implementation and Connection Certainty
1. **Onboard User LED (AmBTech Labs 8051 Board)**:
   - **Certainty**: Physically confirmed present on the development board and associated with pin P1.0.
   - **Observed Behavior**: Both the onboard LED and external LED were observed blinking simultaneously during program execution.
   - **Circuit Details**: The internal PCB routing (active-low current sinking vs. buffered active-high) is not verified from a published manufacturer schematic. It is represented in the diagram as an onboard indicator associated with P1.0 to avoid fabricating unverified trace layouts.
2. **External Breadboard LED (Student Reported Wiring)**:
   - **Certainty**: Reported by the student from laboratory wiring, not independently verified from an instructor schematic.
   - **Connection**: `P1.0 (Pin 1) → Current-limiting resistor R_LED → LED Anode (+) → LED Cathode (-) → GND`.
   - **Polarity**: Because the LED cathode connects to GND, writing logic HIGH (~5 V) to P1.0 forward-biases the diode and turns the LED **ON**, while writing logic LOW (0 V) turns the LED **OFF**. This constitutes **active-high** operation.
   - **Resistor Value**: Labelled `R_LED (value unverified)` because the exact resistor value used during the laboratory session was not confirmed (do not assume 330 Ω).

#### Electrical Consistency & Technical Notes
- **Quasi-Bidirectional Port Sourcing vs. Sinking**:
  The AT89S52 Port 1 uses quasi-bidirectional I/O structures with internal pull-ups. When driven LOW (`0`), the strong internal pulldown transistor can sink up to 10–20 mA. When driven HIGH (`1`), the pin relies on weak internal pull-ups that supply only tens to hundreds of microamps ($\approx 50\text{--}100\ \mu\text{A}$) in steady-state. Because the reported external wiring connects the LED cathode to GND (active-high), current is sourced from P1.0. The successful observation of simultaneous blinking suggests the external circuit utilized a sensitive low-current LED, an auxiliary pull-up resistor on the board header, or board-level buffering.
- **Clock Frequency vs. Software Delay Calibration**:
  The delay loop comments in `main.c` note a 12 MHz timing basis (`for (j = 0; j < 127; j++) roughly 1 ms at 12 MHz`). Because the physical board operates at 11.0592 MHz, the machine cycle duration is $12 / 11.0592\text{ MHz} \approx 1.085\ \mu\text{s}$ (compared to $1.000\ \mu\text{s}$ at 12 MHz). This introduces an inherent $\approx 8.5\%$ elongation in the software delay duration.

---

### Core Logic / Pseudocode / Code Base

#### 1. Main Objective
Control GPIO output switching on Port 1 of the AT89S52 microcontroller using bare-metal Embedded C, toggling digital output levels with a software timing loop to achieve continuous LED blinking.

#### 2. Working Principle and Internal Pull-Ups
- **Memory-Mapped Registers**: In 8051 architecture, I/O ports are memory-mapped Special Function Registers. Port 1 is mapped to byte address `0x90`, accessible directly through `#include <reg51.h>`.
- **Port 1 Architecture**: Port 1 pins are quasi-bidirectional with internal pull-up resistors.
  - Writing `P1 = 0x00` activates the internal pull-down FETs, pulling the pins to ground (0 V) and sinking current.
  - Writing `P1 = 0xFF` turns off the internal pull-down FETs, allowing the internal pull-up resistors to pull the pins HIGH (~5 V), sourcing current to connected loads.

#### 3. GPIO Register Operation
The firmware executes `P1 = 0x00;` followed by `P1 = 0xFF;`. This writes byte-wide across all eight bits of Port 1 (P1.0 to P1.7), causing all Port 1 pins to toggle simultaneously.

#### 4. Custom Software Delay Function
Software timing is achieved through nested `for` loops in `void delay(unsigned int msec)`:
- Outer loop executes `msec` iterations.
- Inner loop executes 127 empty iterations (`for (j = 0; j < 127; j++)`), intended to waste CPU machine cycles. At a 12 MHz clock, this approximates 1 millisecond per outer loop iteration.

#### 5. Approximate Blinking Period
- The firmware requests `delay(500)` in each state:
  $$\text{Period } T = t_{\text{LOW}} + t_{\text{HIGH}} \approx 500\text{ ms} + 500\text{ ms} = 1000\text{ ms} = 1.0\text{ s}$$
  $$\text{Nominal Frequency } f = \frac{1}{T} \approx 1\text{ Hz}$$
- **Timing Disclaimer**: This timing is an intended software approximation. Because the loop was calibrated for 12 MHz but runs on an 11.0592 MHz crystal, and because instruction cycles depend on compiler optimization levels, actual timing diverges from exactly 1.00 Hz.

#### 6. Pseudocode
```text
FUNCTION delay(msec):
    FOR i FROM 0 TO msec - 1 DO
        FOR j FROM 0 TO 126 DO
            // Waste instruction machine cycles (approx. 1 ms per outer iteration at 12 MHz)
        ENDFOR
    ENDFOR
ENDFUNCTION

FUNCTION main():
    WHILE TRUE DO
        P1 = 0x00        // Drive all Port 1 output bits LOW
        CALL delay(500)  // Wait approximately 500 ms
        P1 = 0xFF        // Drive all Port 1 output bits HIGH
        CALL delay(500)  // Wait approximately 500 ms
    ENDWHILE
ENDFUNCTION
```

#### 7. Authoritative C Source Code (`main.c`)
```c
#include <reg51.h>

// A simple software delay function
void delay(unsigned int msec) 
{
    unsigned int i, j;
    for (i = 0; i < msec; i++) {
        for (j = 0; j < 127; j++) {
            // This inner loop takes roughly 1 millisecond at 12 MHz
        }
    }
}

void main() 
{
    while(1) 
    {
        P1 = 0x00;      // Turn all Port 1 LEDs ON (Active-low)
        delay(500);     // Wait for roughly 500 milliseconds (half a second)
        
        P1 = 0xFF;      // Turn all Port 1 LEDs OFF
        delay(500);     // Wait for roughly 500 milliseconds
    }
}
```

---

### INFERENCE

1. **Verified Laboratory Observation**:
   > The onboard LED associated with P1.0 and the externally connected LED were both observed blinking during the experiment.

2. **Bare-Metal Register Access**:
   The experiment successfully demonstrates low-level bare-metal GPIO control. Directly writing to Special Function Register `P1` (SFR `0x90`) eliminates the runtime overhead, latency, and abstractions of Arduino APIs, HAL libraries, or RTOS kernels.

3. **Bare-Metal Embedded Execution Flow**:
   Reset initialization executes directly from address `0x0000` via `STARTUP.A51`, clearing memory and branching into `main()` without an underlying operating system.

4. **Intended Software Timing vs. Measured Values**:
   The ~1-second blinking cycle (500 ms per state) represents an intended software timing calculation. Actual output voltage levels, supply rail stability, and physical frequency were **not measured** with an oscilloscope, digital multimeter, or logic analyzer. These items are clearly documented as unverified rather than treated as experimental failures.

5. **Software Delay Limitations**:
   Software delay loops execute blocking CPU instructions, preventing low-power sleep modes and background task handling. Because the loop relies on CPU clock cycles, timing is sensitive to clock frequency discrepancies (11.0592 MHz physical crystal vs. 12 MHz calculation) and Keil compiler optimization levels. For deterministic timing in embedded IoT edge devices, hardware timer peripherals (Timer 0 / Timer 1) with interrupt service routines are required.
