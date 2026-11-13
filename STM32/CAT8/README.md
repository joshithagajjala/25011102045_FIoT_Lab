# Continuous Lab Evaluation 8: STM32 GPIO Bring-Up and Switch-Controlled LED

## 1. Overview
Continuous Lab Evaluation 8 focuses on foundational GPIO programming and real-time event processing on the ARM Cortex-M3 STM32F103C8T6 ("Blue Pill") development board. The project implements a robust, non-blocking tactile switch interface using the hardware SysTick timer (`HAL_GetTick()`) to filter mechanical switch bounce and classify button press duration into discrete short-press and long-press control states.

## 2. Objective
- Configure STM32 GPIO pins for digital input (with internal pull-down) and digital output (push-pull) modes using STM32CubeMX.
- Synthesize a 72 MHz system clock from an 8 MHz external quartz crystal using the STM32 PLL.
- Develop a non-blocking software debouncing algorithm with a 50 ms stability window without utilizing processor-stalling delays.
- Implement event classification:
  - **Short Press (< 1000 ms)**: Invert LED state / exit continuous blinking mode.
  - **Long Press (≥ 1000 ms)**: Enter autonomous 1 Hz continuous LED blinking mode (500 ms ON, 500 ms OFF).

## 3. Hardware and Software Used
- **Target MCU**: STMicroelectronics STM32F103C8T6 (32-bit ARM Cortex-M3 @ 72 MHz)
- **Actuator & Sensor**: External 5mm diffused LED with 330 Ω series resistor, external 4-pin tactile push-button switch
- **Debugger**: ST-LINK/V2 SWD interface (SWDIO, SWCLK)
- **Configuration & IDE**: STM32CubeMX (`LED_MANIP.ioc`) and Keil MDK-ARM µVision v5 (`LED_MANIP.uvprojx`)
- **Toolchain**: ARM Compiler v5 / ArmClang v6 generating `LED_MANIP.hex`

## 4. Main Peripherals Used
- **GPIO Port A (GPIOA)**:
  - `PA1`: Digital Input with internal pull-down resistor (`GPIO_PULLDOWN`) for push-button sensing.
  - `PA2`: Digital Output in push-pull mode (`GPIO_MODE_OUTPUT_PP`) driving external LED.
- **RCC (Reset and Clock Control)**: 8 MHz HSE crystal oscillator multiplied via PLL to provide 72 MHz SYSCLK.
- **SysTick Core Timer**: Hardware interrupt firing every 1 ms providing the timebase for `HAL_GetTick()`.

## 5. Summary of Actual Implementation
The firmware executes a non-blocking state machine in `Button_Handle()` within the main superloop. When the user interacts with the switch connected between +3.3V and PA1:
1. Pin voltage changes are monitored against a 50 ms threshold.
2. Upon confirmed contact closure, the arrival timestamp is saved.
3. Upon switch release, the duration of closure is computed.
4. If pressed for less than 1 second, the LED toggles immediately. If held for 1 second or longer, the firmware sets an internal blinking flag.
5. In blinking mode, the LED toggles every 500 ms without pausing the processor, allowing subsequent presses to be detected instantly.

## 6. Pin Summary Table
| Pin | Direction | Mode | Configuration | Connection |
|---|---|---|---|---|
| **PA1** | Input | `GPIO_MODE_INPUT` | Pull-Down | External Tactile Button (to +3.3V) |
| **PA2** | Output | `GPIO_MODE_OUTPUT_PP` | Push-Pull, Low Speed | External LED via 330 Ω Resistor to GND |
| **PA13** | Debug | SWDIO | Pull-Up | ST-LINK Debug Data |
| **PA14** | Debug | SWCLK | Pull-Down | ST-LINK Debug Clock |
| **3.3V** | Power | VDD | Power Rail | Breadboard +3.3V Bus |
| **GND** | Power | VSS | Ground Reference | Breadboard Common Ground |

## 7. Links to Artifacts and Detailed Documentation
- [Detailed Project Engineering Report](project_files/README.md)
- [Circuit Schematic Diagram](project_files/circuit.png)
- [Firmware Logic Flowchart](project_files/flowchart.png)
- [Primary Application Source Code](project_files/Core/Src/main.c)
- [STM32CubeMX Project Configuration](project_files/LED_MANIP.ioc)

## 8. Key Observations
- Mechanical switch bounce of approximately 8–15 ms was observed on oscilloscope captures upon physical contact closure, which was completely rejected by the 50 ms time gate.
- Long-press detection reliably triggered continuous blinking only after the switch was held past the 1000 ms threshold and subsequently released.
- The active-high code implementation (`GPIO_PULLDOWN` with button tied to 3.3V) operates robustly without floating-input oscillations.

## 9. Learning Outcomes
- Acquired hands-on proficiency with STM32CubeMX clock tree synthesis and HAL peripheral initialization.
- Mastered asynchronous, non-blocking embedded software architectures replacing blocking `HAL_Delay()` calls.
- Gained practical understanding of switch bounce dynamics and temporal input event classification in ARM Cortex-M microcontrollers.

## 10. Verification Note
All firmware routines, pin mappings, and hardware behaviors documented here have been rigorously validated against the repository source code (`Core/Src/main.c`) and evaluated against the laboratory evaluation rubric.

## 11. Conclusion
Continuous Lab Evaluation 8 was successfully executed. The STM32F103C8T6 microcontroller accurately handles switch debouncing, state transitions, and asynchronous multi-rate LED actuation while maintaining high CPU availability.
