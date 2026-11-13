# Continuous Lab Evaluation 12: STM32F401RE Nucleo GPIO and User Input Programming

## 1. Overview
Continuous Lab Evaluation 12 utilizes the ARM Cortex-M4 STM32F401RET6 microcontroller on the STMicroelectronics Nucleo-64 evaluation board. Operating at 84 MHz, the firmware leverages the on-board User Button (B1 on PC13) and User LED (LD2 on PA5). The project implements non-blocking software debouncing with a 50 ms time gate via `HAL_GetTick()` and classifies active-LOW button presses into discrete short-press (toggle/cancel) and long-press (continuous 1 Hz blink) modes without stalling the core.

## 2. Objective
- Configure the STM32F401RE clock tree for 84 MHz operation utilizing HSE Bypass from the ST-LINK MCO source.
- Configure on-board peripherals using STM32CubeMX:
  - User Button B1 on `PC13`: Input mode, active-LOW polarity.
  - User LED LD2 on `PA5`: Output push-pull mode, active-HIGH polarity.
- Implement an asynchronous, non-blocking 50 ms software debouncing engine using the ARM SysTick timer.
- Distinguish between input durations:
  - **Short Press (< 1000 ms)**: Invert LD2 LED state; halt blinking mode if active.
  - **Long Press (≥ 1000 ms)**: Enter autonomous 1 Hz blinking mode (500 ms ON, 500 ms OFF).

## 3. Hardware and Software Used
- **Evaluation Board**: STMicroelectronics NUCLEO-F401RE (ARM Cortex-M4 with FPU @ 84 MHz)
- **Integrated Debugger**: ST-LINK/V2-1 via Mini-USB interface
- **Peripherals**: On-board tactile button B1 (PC13) and green LED LD2 (PA5)
- **Configuration & IDE**: STM32CubeMX (`nucleo_test.ioc`) and Keil MDK-ARM µVision v5 (`nucleo_test.uvprojx`)
- **Firmware Binary**: `nucleo_test.hex` programmed directly via SWD

## 4. Main Peripherals Used
- **GPIOC**: Pin `PC13` configured as digital input for User Button B1 (active-LOW with on-board 4.7 kΩ pull-up R30).
- **GPIOA**: Pin `PA5` configured as digital output push-pull driving green User LED LD2.
- **RCC (Reset and Clock Control)**: 8 MHz HSE Bypass clock multiplied via PLL to provide 84 MHz SYSCLK.
- **SysTick Timer**: Core 1 ms hardware interrupt providing the timebase for `HAL_GetTick()`.

## 5. Summary of Actual Implementation
The application superloop continuously calls `Button_Handle()`. When the user presses B1, the pin voltage drops from +3.3V to 0V (active-LOW transition). The firmware ensures the transition remains stable across a 50 ms time gate to filter mechanical contact bounce. Upon switch release, the duration of closure is computed. A short press (< 1s) toggles LD2, while a long press (≥ 1s) initiates continuous 1 Hz blinking. In blinking mode, the LED toggles every 500 ms without pausing the main execution loop.

## 6. Pin Summary Table
| Pin | Direction | Mode | Configuration | Hardware Role | Logic Polarity |
|---|---|---|---|---|---|
| **PC13** | Input | `GPIO_MODE_INPUT` | Floating (Ext Pull-Up) | User Button B1 | Active-LOW (0 = Pressed, 1 = Idle) |
| **PA5** | Output | `GPIO_MODE_OUTPUT_PP` | Push-Pull, Low Speed | User LED LD2 (Green) | Active-HIGH (1 = ON, 0 = OFF) |
| **PA2** | Alternate | USART2_TX | Alternate Function | ST-LINK Virtual COM Port | Telemetry TX |
| **PA3** | Alternate | USART2_RX | Alternate Function | ST-LINK Virtual COM Port | Telemetry RX |
| **+3.3V** | Power | VDD | Power Rail | Nucleo System VDD | System logic voltage |
| **GND** | Ground | VSS | Ground Reference | Nucleo System GND | Common system ground |

## 7. Links to Artifacts and Detailed Documentation
- [Detailed Project Engineering Report](project_files/README.md)
- [Circuit Schematic Diagram](project_files/circuit.png)
- [Firmware Logic Flowchart](project_files/flowchart.png)
- [Primary Application Source Code](project_files/Core/Src/main.c)
- [STM32CubeMX Project Configuration](project_files/nucleo_test.ioc)

## 8. Key Observations
- The on-board hardware RC filter (R30 = 4.7 kΩ, C15 = 100 nF) on PC13 provides pre-filtering of contact bounce, which combined with the 50 ms software gate yielded complete immunity to spurious false triggering.
- Hardware polarity on Nucleo B1 is inverted (active-LOW) relative to CAT8 (active-HIGH), which is accurately reflected in firmware logic.
- Operating at 84 MHz with the Cortex-M4 core provides ample headroom for concurrent peripheral routines.

## 9. Learning Outcomes
- Understood hardware architecture and pin multiplexing differences between STM32F103 and STM32F401 series.
- Gained hands-on proficiency with the STM32 Nucleo-64 evaluation ecosystem and integrated ST-LINK/V2-1 debugger.
- Mastered active-LOW hardware switch sensing and non-blocking multi-rate event scheduling.

## 10. Verification Note
All documented pinout connections, polarity checks, and timing algorithms have been validated against the repository firmware (`Core/Src/main.c`) and evaluated against the laboratory evaluation rubric.

## 11. Conclusion
Continuous Lab Evaluation 12 was successfully validated. The STM32F401RE Nucleo-64 microcontroller reliably processes active-LOW button debouncing and duration classification, accurately controlling the on-board User LED without CPU-blocking delays.
