# Foundations of IoT — Continuous Lab Evaluation 5

### Timers, Software PWM, Waveform Generation and Stepper Motor Control

---

## Overview

Continuous Lab Evaluation 5 explores hardware timers and interrupt-based control using the AT89S52 microcontroller (8051 architecture). The evaluation demonstrates how internal hardware timers can generate accurate, time-dependent GPIO signals without relying on CPU-blocking software delay loops (`for`/`while`). 

Three primary laboratory tasks are implemented:
1. **Task 1 (Task5A):** LED fading using software Pulse Width Modulation (PWM) driven by Timer 0 interrupts.
2. **Task 2 (Task5B):** Variable-frequency square-wave generation using Timer 1 interrupts with interactive frequency selection via a 4×4 matrix keypad, while concurrently driving a PWM LED on Timer 0.
3. **Task 3 (Task5C):** Stepper motor speed control using Timer 0 interrupts to pace a 4-state excitation sequence with interactive keypad speed selection.

Additionally, Task 2 includes three independent signal generation sub-projects:
- **Signal 1:** Dedicated Timer 1 interrupt-driven square-wave generation.
- **Signal 2:** Variable duty cycle waveform generation with independent HIGH/LOW reload periods.
- **Signal 3:** Finite pulse-train burst generation.

---

## Hardware Platform

- **Microcontroller:** Atmel AT89S52 (8-bit 8051 Architecture, 40-pin DIP)
- **Toolchain & IDE:** Keil µVision / C51 Compiler
- **System Clock:** Development-board integrated crystal oscillator (11.0592 MHz nominal reference for classic 12-clock 8051 timing calculations)
- **Hardware Interfaces:**
  - Active-HIGH external LED on Port 1 (P1.0)
  - Waveform test point on Port 1 (P1.1)
  - 4×4 Matrix Keypad on Port 2 (P2.7–P2.0)
  - 4-phase stepper motor interface via intermediate power driver (ULN2003) on Port 1 (P1.0–P1.3)

---

## Experiment Summary

| Task | Experiment | Primary Peripherals | Key Concept Demonstrated |
|---|---|---|---|
| **Task 1 (Task5A)** | LED Fading | Timer 0 (Mode 1), P1.0 | Software PWM generation and periodic duty-cycle modulation |
| **Task 2 (Task5B)** | Variable-Frequency Square Wave | Timer 0, Timer 1 (Mode 1), 4×4 Keypad, P1.0, P1.1 | Concurrent dual-timer operation, pin toggling, and keypad-based frequency selection |
| **Task 2 / Signal 1** | Fixed Square Wave | Timer 1 (Mode 1), P1.1 | Hardware interrupt pin toggling producing a symmetric 50% duty cycle |
| **Task 2 / Signal 2** | Variable Duty Cycle Waveform | Timer 1 (Mode 1), P1.1 | Alternating asymmetric reload preloads for independent HIGH and LOW durations |
| **Task 2 / Signal 3** | Pulse-Train Generation | Timer 1 (Mode 1), P1.1 | Precise burst pulse sequence counting using hardware timer overflows |
| **Task 3 (Task5C)** | Stepper Motor Speed Control | Timer 0 (Mode 1), 4×4 Keypad, P1.0–P1.3 | Unidirectional wave-drive stepping sequence and keypad speed scaling |

---

## Repository Structure

```text
CAT5/
├── README.md
└── project_files/
    ├── TASK1/
    │   ├── README.md
    │   ├── circuit.png
    │   ├── led_fading_software_pwm.c
    │   ├── led_fading_software_pwm.uvproj
    │   ├── STARTUP.A51
    │   └── Objects/
    │       └── led_fading_software_pwm.hex
    ├── TASK2/
    │   ├── README.md
    │   ├── circuit.png
    │   ├── variable_frequency_square_wave.c
    │   ├── variable_frequency_square_wave.uvproj
    │   ├── STARTUP.A51
    │   ├── Objects/
    │   │   └── variable_frequency_square_wave.hex
    │   └── DIFFERENT_SIGNALS/
    │       ├── SIGNAL1/
    │       ├── SIGNAL2/
    │       └── SIGNAL3/
    └── TASK3/
        ├── README.md
        ├── circuit.png
        ├── stepper_motor_speed_control.c
        ├── stepper_motor_speed_control.uvproj
        ├── STARTUP.A51
        └── Objects/
            └── stepper_motor_speed_control.hex
```

*(Note: The above diagram illustrates the core project hierarchy. All additional configuration files, reference implementations such as `variable_frequency_with_pwm.*`, and original circuit archives are fully preserved in their respective directories.)*

---

## Documentation Links

### Main Task Documentation
- [Task 1 README — LED Fading Using Software PWM](project_files/TASK1/README.md)
- [Task 2 README — Variable-Frequency Square-Wave Generation](project_files/TASK2/README.md)
- [Task 3 README — Stepper Motor Speed Control](project_files/TASK3/README.md)

### Additional Task 2 Signal Sub-Projects
- [Task 2 / Signal 1 README — Square-Wave Generation](project_files/TASK2/DIFFERENT_SIGNALS/SIGNAL1/README.md)
- [Task 2 / Signal 2 README — Variable Duty Cycle Waveform](project_files/TASK2/DIFFERENT_SIGNALS/SIGNAL2/README.md)
- [Task 2 / Signal 3 README — Pulse Train Generation](project_files/TASK2/DIFFERENT_SIGNALS/SIGNAL3/README.md)

### Verified Circuit Diagrams
- [Task 1 Circuit Diagram](project_files/TASK1/circuit.png)
- [Task 2 Circuit Diagram](project_files/TASK2/circuit.png)
- [Task 3 Circuit Diagram](project_files/TASK3/circuit.png)

---

## Concepts Demonstrated

### 1. Hardware Timer Operation
The 8051 microcontroller provides internal 16-bit timers (Timer 0 and Timer 1) that increment once every machine cycle (12 oscillator periods). By setting `TMOD` and preloading high and low byte registers (`THx`, `TLx`), deterministic time intervals are generated. Timer overflows assert interrupt flags (`TF0`, `TF1`) and vector CPU execution to dedicated Interrupt Service Routines (ISRs).

### 2. Software Pulse Width Modulation (PWM)
Because classic 8051 devices lack dedicated hardware PWM modules, PWM is synthesized programmatically. An interrupt counter divides time into 100 discrete resolution slices. A comparator drives the output pin HIGH while the counter is below the duty threshold and LOW otherwise. Task 1 uses this technique to continuously fade an LED between 0% and 100% brightness.

### 3. Waveform Generation & Concurrent Timers
By reloading Timer 1 with different 16-bit values on each interrupt and toggling an output pin, square waves of varying frequencies (100 Hz to 5 kHz) are generated. Task 2 demonstrates how Timer 0 (driving LED PWM) and Timer 1 (driving square-wave generation) run concurrently without mutual interference or blocking delays.

### 4. Stepper Motor Commutation
Stepper motors require sequential multi-phase excitation of stator coils. Task 3 generates a 4-state wave drive pattern (`0x08 -> 0x04 -> 0x02 -> 0x01`) on Port 1 lower nibble. By varying the number of hardware timer interrupts between commutation steps via a matrix keypad, rotational speed is accurately controlled without CPU-blocking wait loops.

---

## Learning Outcomes

Completing the Continuous Lab Evaluation 5 exercises demonstrates proficiency in:
- **Timer Register Configuration:** Configuring `TMOD` and calculating 16-bit preload values for `THx` and `TLx`.
- **Interrupt Enable & Handling:** Managing `IE` control bits (`EA`, `ET0`, `ET1`) and writing dedicated ISRs.
- **Digital Waveform Synthesis:** Producing symmetrical square waves and arbitrary pulse trains using pin-toggling techniques.
- **PWM Duty-Cycle Manipulation:** Designing non-blocking software PWM algorithms with dynamic brightness ramping.
- **Matrix Keypad Scanning:** Implementing active-LOW row driving and column sensing on Port 2.
- **Multi-Timer Coordination:** Running multiple timers concurrently for distinct real-time tasks.
- **Stepper Motor Interfacing:** Sequencing stator excitation phases and interfacing with external power drivers.
- **Timing & Performance Analysis:** Calculating theoretical frequencies and analyzing practical implementation constraints like ISR latency and reload overhead.

---

## Verification and Limitations

- **File-Presence & Structure Compliance:** All tasks and project deliverables have been verified to exist and match the official evaluation structure.
- **Code Integrity:** All source code, project configurations, and firmware binaries are preserved without modifications.
- **Theoretical Calculations vs. Measurements:** Frequencies and time intervals stated in calculations assume an idealized 11.0592 MHz clock and do not include software reload latency or interrupt entry delays. Stated frequencies reflect nominal software design targets rather than oscilloscope-measured readings.
- **Implementation Scope:** Task 3 firmware implements unidirectional speed selection; direction reversal is not implemented in the current codebase.

---

## Conclusion

Continuous Lab Evaluation 5 demonstrates the versatility of the 8051 architecture when combining hardware timers, vectored interrupts, and GPIO manipulation. By offloading timing to hardware timers and structuring firmware around non-blocking interrupt routines, the AT89S52 microcontroller efficiently executes concurrent real-time tasks including LED brightness fading, digital signal generation, and stepper motor motion control.
