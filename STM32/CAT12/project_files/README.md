# Continuous Lab Evaluation 12: STM32F401RE Nucleo GPIO and User Input Programming

## 1. Experiment Title
Continuous Lab Evaluation 12 — Digital Input/Output, Non-Blocking Debounce, and Event Classification on STM32F401RE Nucleo-64 Using On-Board User Peripherals

## 2. Aim
To configure the General Purpose Input/Output (GPIO) peripherals of the 32-bit ARM Cortex-M4 STM32F401RET6 microcontroller on the STMicroelectronics Nucleo-64 evaluation board; utilize the on-board User Button (B1 on PC13) and User LED (LD2 on PA5); implement non-blocking software debouncing with a 50 ms time gate using `HAL_GetTick()`; and classify user inputs into short-press (toggle/halt) and long-press (continuous 1 Hz blink) modes without blocking the processor core.

## 3. Hardware Components
| Component | Specification | Quantity | Role / Remarks |
|---|---|---|---|
| Development Board | NUCLEO-F401RE | 1 | 32-bit ARM Cortex-M4 MCU with FPU @ 84 MHz, 512 KB Flash, 96 KB SRAM |
| Integrated Debugger | ST-LINK/V2-1 | 1 | On-board USB programmer, hardware debugger, and Virtual COM Port (VCP) |
| On-Board User Button (B1) | Blue Tactile Switch | 1 | Connected to PC13 with hardware RC filter and pull-up resistor (active-LOW) |
| On-Board User LED (LD2) | Green 0805 SMD LED | 1 | Connected to PA5 via series resistor (active-HIGH) |
| Host Cable | USB Type-A to Mini-B | 1 | Supplies +5V board power and carries SWD/VCP communication |

## 4. Software Tools
- **STM32CubeMX**: Peripheral initialization, clock tree synthesis (84 MHz HSE Bypass / HSI), and code generation (`nucleo_test.ioc`).
- **Keil MDK-ARM (µVision v5)**: C/C++ compilation, ARMClang / ARMCC toolchain, linking, and hex file generation (`nucleo_test.uvprojx`).
- **ST-LINK Firmware / ST-LINK Utility**: On-board programmer interface for flashing `nucleo_test.hex`.

## 5. Theory
### The STM32F401RE Architecture & Nucleo-64 Ecosystem
The STM32F401RE is a high-performance microcontroller built around an ARM Cortex-M4 core featuring a single-precision Floating Point Unit (FPU), Memory Protection Unit (MPU), and digital signal processing (DSP) instructions operating up to 84 MHz. 
The Nucleo-64 platform integrates an on-board ST-LINK/V2-1 debugger and pre-wires two essential user interaction peripherals:
- **User Button B1**: Connected directly to pin **PC13**. The Nucleo schematic includes an external 4.7 kΩ pull-up resistor (R30) to +3.3V and a 100 nF capacitor (C15) for partial hardware debouncing. When unpressed, PC13 reads logic HIGH (+3.3V); when pressed, it shorts to ground, pulling PC13 to logic LOW (0V). This is an **active-LOW** input topology.
- **User LED LD2**: Connected directly to pin **PA5** through a series current-limiting resistor. Driving PA5 to logic HIGH illuminates the LED (**active-HIGH** output).

### Hardware Comparison: CAT8 vs CAT12
| Parameter | CAT8 (Blue Pill) | CAT12 (Nucleo-64) |
|---|---|---|
| **Core Architecture** | ARM Cortex-M3 (ARMv7-M) | ARM Cortex-M4 with FPU (ARMv7E-M) |
| **Max Clock Speed** | 72 MHz (8 MHz HSE Crystal + PLL) | 84 MHz (HSE Bypass from ST-LINK / HSI) |
| **User Button Pin** | PA1 (External breadboard button) | PC13 (On-board tactile button B1) |
| **Button Electrical Polarity** | Active-HIGH (Internal pull-down in code) | Active-LOW (External pull-up on PCB) |
| **User LED Pin** | PA2 (External breadboard LED) | PA5 (On-board green LED LD2) |
| **Flash / SRAM** | 64 KB Flash / 20 KB SRAM | 512 KB Flash / 96 KB SRAM |

### Non-Blocking Temporal Event Classification
Instead of utilizing crude busy-wait delays (e.g. `HAL_Delay()`), which paralyze the Cortex-M4 core and waste energy, the firmware utilizes the ARM SysTick timer via `HAL_GetTick()`:
1. Contact bounce is filtered by requiring the physical state to remain steady for $> 50\,\text{ms}$.
2. Upon button release, the elapsed closure duration $\Delta t = t_{release} - t_{press}$ is evaluated:
   - If $\Delta t < 1000\,\text{ms}$: Classified as a **Short Press**. Toggles LD2. If continuous blinking was active, it terminates blinking.
   - If $\Delta t \ge 1000\,\text{ms}$: Classified as a **Long Press**. Activates continuous 1 Hz blinking mode.
3. While blinking, the LED toggles every 500 ms in the background without obstructing ongoing switch sampling.

## 6. Actual Pin Configuration
| Pin Name | Connector Pin | Direction | Mode | Pull Configuration | Connected Hardware | Logic Polarity |
|---|---|---|---|---|---|---|
| **PC13** | Morpho CN7 Pin 23 | Input | `GPIO_MODE_INPUT` | No Pull (`GPIO_NOPULL`) | On-board User Button B1 | Active-LOW (0 = Pressed, 1 = Idle) |
| **PA5** | Morpho CN10 Pin 21 | Output | `GPIO_MODE_OUTPUT_PP` | No Pull (`GPIO_NOPULL`) | On-board Green LED LD2 | Active-HIGH (1 = ON, 0 = OFF) |
| **PA2** | Morpho CN10 Pin 35 | Alternate | USART2_TX | None | ST-LINK Virtual COM Port | Diagnostic Telemetry TX |
| **PA3** | Morpho CN10 Pin 37 | Alternate | USART2_RX | None | ST-LINK Virtual COM Port | Diagnostic Telemetry RX |
| **+3.3V** | Morpho CN6 Pin 4 | Power | VDD | Power | Power Rail | 3.3V System Voltage |
| **GND** | Morpho CN6 Pin 6 | Ground | VSS | Ground | Common Ground | System Ground Reference |

## 7. Circuit Diagram
![Circuit Diagram](circuit.png)

## 8. CubeMX / Peripheral Configuration
- **RCC Configuration**:
  - High Speed Clock (HSE): BYPASS Clock Source (8 MHz clock supplied from ST-LINK MCO).
  - System Clock Mux: PLLCLK $\rightarrow$ SYSCLK = 84 MHz.
  - AHB Prescaler: /1 $\rightarrow$ HCLK = 84 MHz.
  - APB1 Prescaler: /2 $\rightarrow$ 42 MHz.
  - APB2 Prescaler: /1 $\rightarrow$ 84 MHz.
- **GPIO Pin Configuration**:
  - **PC13**:
    - GPIO mode: Input mode
    - GPIO Pull-up/Pull-down: No pull-up and no pull-down (utilizes on-board 4.7 kΩ pull-up R30)
    - User Label: `B1 [Blue PushButton]`
  - **PA5**:
    - GPIO mode: Output Push-Pull
    - GPIO Pull-up/Pull-down: No pull-up and no pull-down
    - Maximum output speed: Low
    - User Label: `LD2 [Green Led]`
- **SYS Debug**: Serial Wire Debug enabled.

## 9. Key HAL APIs Used
- `HAL_Init()`: Resets all peripherals, initializes the Flash interface, Systick, and data caches.
- `SystemClock_Config()`: Sets internal/external oscillators, PLL factors, and core clock tree to 84 MHz.
- `__HAL_RCC_GPIOA_CLK_ENABLE()` & `__HAL_RCC_GPIOC_CLK_ENABLE()`: Supplies clock gating to Port A and Port C register banks.
- `HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)`: Reads bit 13 of the Port C Input Data Register (`GPIOC->IDR`).
- `HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PinState PinState)`: Sets or resets PA5 output via the Bit Set/Reset Register (`GPIOA->BSRR`).
- `HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5)`: Inverts PA5 output state.
- `HAL_GetTick()`: Returns 32-bit millisecond tick count from SysTick interrupt.

## 10. Source Code Logic
The core button event handling in `Core/Src/main.c`:

```c
/* USER CODE BEGIN 0 */
void Button_Handle(void)
{
    static uint32_t lastDebounceTime = 0;
    static uint8_t buttonState = 1;      // Default unpressed state (active-low: HIGH)
    static uint8_t lastButtonState = 1;
    static uint32_t pressStartTime = 0;
    static uint8_t isButtonPressed = 0;
    static uint8_t blinkingMode = 0;
    static uint32_t lastBlinkTime = 0;

    uint32_t currentTime = HAL_GetTick();
    uint8_t reading = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

    // Detect raw edge transition
    if (reading != lastButtonState)
    {
        lastDebounceTime = currentTime;
    }

    // Filter mechanical bounce across 50 ms window
    if ((currentTime - lastDebounceTime) > 50)
    {
        if (reading != buttonState)
        {
            buttonState = reading;

            if (buttonState == GPIO_PIN_RESET) // Active-LOW: Button is physically pressed
            {
                pressStartTime = currentTime;
                isButtonPressed = 1;
            }
            else // Button released (state returned to GPIO_PIN_SET)
            {
                if (isButtonPressed)
                {
                    uint32_t pressDuration = currentTime - pressStartTime;
                    isButtonPressed = 0;

                    if (pressDuration >= 1000)
                    {
                        // Long press detected
                        blinkingMode = 1;
                    }
                    else
                    {
                        // Short press detected
                        if (blinkingMode)
                        {
                            blinkingMode = 0; // Cancel blinking mode
                        }
                        else
                        {
                            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // Toggle LD2
                        }
                    }
                }
            }
        }
    }

    lastButtonState = reading;

    // Concurrent non-blocking blinking handler
    if (blinkingMode)
    {
        if ((currentTime - lastBlinkTime) >= 500)
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            lastBlinkTime = currentTime;
        }
    }
}
/* USER CODE END 0 */
```

## 11. Algorithm
1. **Initialize Microcontroller Core**: Execute `HAL_Init()` and configure the 84 MHz Cortex-M4 clock tree.
2. **Configure GPIO Peripherals**: Enable Port A and Port C clocks; initialize PC13 as input; initialize PA5 as push-pull output.
3. **Execute Main Superloop**: Repeatedly invoke `Button_Handle()` on every loop iteration.
4. **Sample PC13 Input**: Read digital voltage level of PC13. If state differs from previous reading, reset `lastDebounceTime` to `currentTime`.
5. **Debounce Gate**: Check if reading remained stable for $> 50\,\text{ms}$.
6. **Detect State Transition**:
   - If transition is from `SET` to `RESET` (Press): Record `pressStartTime = currentTime` and set `isButtonPressed = 1`.
   - If transition is from `RESET` to `SET` (Release): Calculate `pressDuration = currentTime - pressStartTime`.
7. **Action Dispatcher**:
   - If `pressDuration >= 1000 ms`: Set `blinkingMode = 1`.
   - If `pressDuration < 1000 ms`: If `blinkingMode == 1`, reset `blinkingMode = 0`; otherwise, toggle PA5 immediately.
8. **Asynchronous Blinker**: If `blinkingMode == 1` and `currentTime - lastBlinkTime >= 500 ms`, toggle PA5 and update `lastBlinkTime`.

## 12. Pseudocode
```text
FUNCTION Button_Handle():
    STATIC lastDebounceTime = 0, buttonState = 1, lastButtonState = 1
    STATIC pressStartTime = 0, isButtonPressed = 0, blinkingMode = 0, lastBlinkTime = 0

    now = HAL_GetTick()
    reading = READ_PIN(GPIOC, PIN_13)

    IF reading != lastButtonState THEN
        lastDebounceTime = now
    END IF

    IF (now - lastDebounceTime) > 50 THEN
        IF reading != buttonState THEN
            buttonState = reading
            IF buttonState == 0 THEN // Active-LOW press
                pressStartTime = now
                isButtonPressed = 1
            ELSE // Release
                IF isButtonPressed THEN
                    isButtonPressed = 0
                    duration = now - pressStartTime
                    IF duration >= 1000 THEN
                        blinkingMode = 1
                    ELSE
                        IF blinkingMode == 1 THEN
                            blinkingMode = 0
                        ELSE
                            TOGGLE_PIN(GPIOA, PIN_5)
                        END IF
                    END IF
                END IF
            END IF
        END IF
    END IF

    lastButtonState = reading

    IF blinkingMode == 1 AND (now - lastBlinkTime) >= 500 THEN
        TOGGLE_PIN(GPIOA, PIN_5)
        lastBlinkTime = now
    END IF
END FUNCTION
```

## 13. Flowchart
![Flowchart](flowchart.png)

## 14. Expected Output
- **Boot State**: LD2 (Green LED on PA5) is initially OFF.
- **Short Press on B1 (< 1 sec)**: Every individual press cleanly toggles LD2 between ON and OFF without glitching.
- **Long Press on B1 (≥ 1 sec)**: Holding B1 down for 1 second or longer and releasing initiates continuous 1 Hz blinking (500 ms ON, 500 ms OFF).
- **Subsequent Short Press**: Immediately halts blinking and freezes LD2 in its current state.

## 15. Actual Observations
- The on-board hardware RC filter (C15 = 100 nF, R30 = 4.7 kΩ) provides an analog time constant $\tau = RC \approx 0.47\,\text{ms}$. Combined with the 50 ms firmware gate, switch debouncing is $100\%$ reliable with zero false triggers.
- The active-LOW behavior (`buttonState == GPIO_PIN_RESET` on press) precisely matches the Nucleo-64 hardware schematic.
- Blinking executes smoothly with no jitter, confirming that the Cortex-M4 SysTick timer maintains millisecond precision during concurrent superloop execution.

## 16. Troubleshooting
1. **LD2 Never Turns ON**:
   - Verify PA5 configuration in CubeMX (`GPIO_MODE_OUTPUT_PP`, No Pull).
   - Ensure solder bridge SB21 on the Nucleo board is connected (default closed to route PA5 to LD2).
2. **Button B1 Does Not Register Presses**:
   - Check solder bridge SB63 on Nucleo board (default closed to connect PC13 to B1).
   - Remember PC13 is active-LOW; checking for `GPIO_PIN_SET` will invert the trigger condition.
3. **Firmware Fails to Download via ST-LINK**:
   - Confirm ST-LINK USB driver is installed.
   - Verify the two CN2 jumpers on the Nucleo board are closed (enabling SWD connection to the STM32F401 target).

## 17. Limitations
- Does not use external interrupt lines (`EXTI15_10_IRQn`), necessitating continuous superloop polling.
- If additional time-consuming algorithms are added to the superloop without cooperative yielding, button responsiveness may degrade.
- High-priority interrupts preempting the superloop can slightly extend the measured press duration.

## 18. Result
The GPIO peripherals of the STM32F401RET6 Nucleo-64 board were successfully configured and verified. Non-blocking button debouncing, active-LOW signal processing, and dual-mode duration classification (short press toggle, long press 1 Hz continuous blink) functioned with absolute fidelity to design specifications.

## 19. Viva Questions with Answers
1. **Q: How does the ARM Cortex-M4 core on the STM32F401 differ from the Cortex-M3 on the STM32F103?**  
   *A:* The Cortex-M4 core includes a hardware single-precision Floating Point Unit (FPU), SIMD instructions, and dedicated DSP instructions, operating at a higher maximum frequency (84 MHz vs 72 MHz).
2. **Q: Why is the Nucleo User Button B1 classified as active-LOW?**  
   *A:* The button is pulled up to +3.3V by resistor R30 when idle (logic 1). When pressed, it connects the pin directly to ground (logic 0).
3. **Q: What is the purpose of the on-board RC filter (R30/C15) on PC13?**  
   *A:* It forms a low-pass analog filter that attenuates high-frequency voltage spikes and contact bounce before the signal reaches the microcontroller input pin.
4. **Q: What is HSE Bypass mode in the Nucleo-64 clock configuration?**  
   *A:* In HSE Bypass mode, the 8 MHz clock signal generated by the integrated ST-LINK/V2-1 MCO (Master Clock Output) is fed directly into the STM32 OSC_IN pin, bypassing the internal crystal oscillator circuit.
5. **Q: How does `HAL_GetTick()` enable non-blocking timing?**  
   *A:* It reads an incremental millisecond counter driven by the SysTick hardware interrupt, enabling the firmware to measure elapsed intervals using timestamp subtraction while allowing the processor to continue other work.
6. **Q: What is the maximum current that an STM32F401 GPIO pin can sink or source?**  
   *A:* A single GPIO pin can source or sink up to 25 mA, with total current across all I/O pins capped at 120 mA to maintain silicon thermal reliability.
7. **Q: What are the function of solder bridges (SB) on the Nucleo-64 board?**  
   *A:* Zero-ohm solder jumpers configure hardware routing options, enabling or disconnecting on-board peripherals (like LD2, B1, or VCP) to reallocate pins to the Morpho headers.
8. **Q: Why is `isButtonPressed` state tracking necessary when calculating press duration?**  
   *A:* It ensures that `pressDuration` is only computed when an actual physical release event follows a validated press event, preventing spurious release computations upon system reset.

## 20. File Descriptions
| File Name | Location | Description |
|---|---|---|
| `main.c` | `Core/Src/` | Contains system clock setup, GPIO initialization, `Button_Handle()` state machine, and main superloop |
| `main.h` | `Core/Inc/` | Peripheral pin declarations (`B1_Pin`, `LD2_Pin`), macros, and standard HAL inclusions |
| `nucleo_test.ioc` | Project Root | STM32CubeMX device configuration for STM32F401RET6 (84 MHz clock tree and GPIO) |
| `nucleo_test.uvprojx` | `MDK-ARM/` | Keil MDK project configuration with ARMClang compiler flags and debugger settings |
| `circuit.png` | Project Root | Detailed hardware schematic of Nucleo-F401RE board with B1 and LD2 connections |
| `flowchart.png` | Project Root | Flowchart illustrating active-LOW debouncing and short/long press duration dispatching |
