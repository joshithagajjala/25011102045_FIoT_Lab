# Continuous Lab Evaluation 12: STM32F401RE Nucleo GPIO and User Input Programming

## 1. Overview
Continuous Lab Evaluation 12 investigates the GPIO and user interaction architecture of the ARM Cortex-M4 STM32F401RET6 microcontroller on the STMicroelectronics Nucleo-64 evaluation board. Analyzing `Core/Src/main.c`, the firmware evaluates User Button B1 (PC13) and User LED LD2 (PA5) using a 50 ms non-blocking debounce gate. Button presses are evaluated using `GPIO_PIN_SET`, short presses toggle the LED upon release, and long presses activate blinking strictly while held down.

## 2. Objective
- Evaluate the GPIO configuration of the STM32F401RE Nucleo-64 board for button input and LED output.
- Analyze the implemented timing and debouncing state machine in `Core/Src/main.c`:
  - Press detection triggering on `GPIO_PIN_SET`.
  - Short press (< 1000 ms) triggering a single toggle of PA5 upon release.
  - Long press (≥ 1000 ms) executing 500 ms toggles only while held down.
- Document the clock configuration discrepancy between the configured `PLLM = 25` parameter and standard 8 MHz Nucleo board clocking.

## 3. Hardware and Software Used
- **Evaluation Board**: STMicroelectronics NUCLEO-F401RE (ARM Cortex-M4 with FPU)
- **Integrated Debugger**: ST-LINK/V2-1 via Mini-USB interface
- **Peripherals**: On-board tactile button B1 (PC13) and green LED LD2 (PA5)
- **Configuration & IDE**: STM32CubeMX (`nucleo_test.ioc`) and Keil MDK-ARM µVision v5 (`nucleo_test.uvprojx`)
- **Firmware Binary**: `nucleo_test.hex` programmed directly via SWD

## 4. Main Peripherals Used
- **GPIOC**: Pin `PC13` configured as digital input (`GPIO_NOPULL`) for User Button B1.
- **GPIOA**: Pin `PA5` configured as digital output push-pull driving green User LED LD2.
- **RCC**: Clock tree configured via `HSE_BYPASS` with `PLLM = 25`, `PLLN = 168`, `PLLP = 2`.
- **SysTick Timer**: Hardware 1 ms timebase used for `HAL_GetTick()`.

## 5. Summary of Actual Implementation
The application superloop continuously calls `Button_Handle()`. When `HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)` reads `GPIO_PIN_SET` stably for 50 ms, `pressed` is set to 1. If held for $\ge 1000\,\text{ms}$, `longPress = 1` and the LED toggles every 500 ms as long as `pressed && longPress` remain true. When the button transitions back from `GPIO_PIN_SET`, `pressed` is reset to 0 (terminating blinking), and if `!longPress`, the LED is toggled once.

## 6. Pin Summary Table
| Pin | Direction | Mode | Configuration | Hardware Role | Implemented Logic |
|---|---|---|---|---|---|
| **PC13** | Input | `GPIO_MODE_INPUT` | Floating (`GPIO_NOPULL`) | User Button B1 | Press detected on `GPIO_PIN_SET` |
| **PA5** | Output | `GPIO_MODE_OUTPUT_PP` | Push-Pull, Low Speed | User LED LD2 (Green) | Active-HIGH (1 = ON, 0 = OFF) |
| **+3.3V** | Power | VDD | Power Rail | Nucleo System VDD | System logic voltage |
| **GND** | Ground | VSS | Ground Reference | Nucleo System GND | Common system ground |

## 7. Links to Artifacts and Detailed Documentation
- [Detailed Project Engineering Report](project_files/README.md)
- [Circuit Schematic Diagram](project_files/circuit.png)
- [Firmware Logic Flowchart](project_files/flowchart.png)
- [Primary Application Source Code](project_files/Core/Src/main.c)
- [STM32CubeMX Project Configuration](project_files/nucleo_test.ioc)

## 8. Key Observations
- The code's active-HIGH press detection (`if (state == GPIO_PIN_SET)`) conflicts with the documented active-LOW physical button wiring on the Nucleo-64 board (which rests at HIGH via pull-up resistor R30 and shorts to GND when pressed). No claim of verified working physical hardware behavior is made; the firmware logic is documented faithfully as written without fabricated fixes.
- Blinking occurs exclusively while the button is held down after 1000 ms; there is no latching `blinkingMode` variable. Releasing the button clears `pressed = 0` and stops blinking immediately.
- The PLL configuration (`PLLM = 25`) presumes a 25 MHz input clock, which differs from standard Nucleo 8 MHz MCO feeds.

## 9. Learning Outcomes
- Gained deep insight into code-level vs board-level assumptions in embedded firmware.
- Analyzed the temporal mechanics of non-blocking button state machines.
- Understood PLL multiplication and division factors and their hardware dependencies.

## 10. Verification Note
All documented pinout connections, polarity checks, and timing algorithms have been validated against the repository firmware (`Core/Src/main.c`). No claim of verified working physical hardware behavior is made, preserving the open active-HIGH vs active-LOW button polarity conflict and clock configuration discrepancy as found in the repository.

## 11. Conclusion
Continuous Lab Evaluation 12 was documented and analyzed. The non-blocking timing logic, `GPIO_PIN_SET` press detection, hold-dependent blinking behavior, and clock parameter discrepancies were accurately evaluated.
