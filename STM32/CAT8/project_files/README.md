# Continuous Lab Evaluation 8: STM32 GPIO Bring-Up and Switch-Controlled LED

## 1. Experiment Title
Continuous Lab Evaluation 8 — Switch-Controlled LED with Non-Blocking Software Debouncing and Event Classification on STM32F103C8T6

## 2. Aim
To configure the General Purpose Input/Output (GPIO) peripherals of the ARM Cortex-M3 STM32F103C8T6 microcontroller using STM32CubeMX and Keil MDK-ARM, interface an external push-button and an external LED, implement a non-blocking software debouncing algorithm using `HAL_GetTick()`, and classify user input into short-press (toggle/halt) and long-press (continuous blinking) modes without CPU-blocking delays.

## 3. Hardware Components
| Component | Specification | Quantity | Role / Remarks |
|---|---|---|---|
| Microcontroller Board | STM32F103C8T6 ("Blue Pill") | 1 | 32-bit ARM Cortex-M3 MCU @ 72 MHz, 64 KB Flash, 20 KB SRAM |
| External Tactile Switch | 4-pin momentary push button | 1 | User input connected to PA1 |
| External LED | 5mm Diffused (Red/Green) | 1 | Visual actuator connected to PA2 |
| Current Limiting Resistor | 220 Ω to 330 Ω (1/4 W) | 1 | Limits forward current through PA2 LED (~5–8 mA) |
| Hardware Programmer | ST-LINK/V2 (or clone) | 1 | SWD flashing and live in-circuit debugging (SWDIO, SWCLK, 3.3V, GND) |
| Breadboard & Jumpers | Standard solderless breadboard | 1 | Component interconnects and power rail distribution |

## 4. Software Tools
- **STM32CubeMX**: Peripheral configuration, clock tree synthesis (8 MHz HSE to 72 MHz PLL), and HAL code generation (`LED_MANIP.ioc`).
- **Keil MDK-ARM (µVision v5)**: C/C++ compilation, ARMClang/ARMCC toolchain, linking, and hex generation (`LED_MANIP.uvprojx`).
- **ST-LINK Utility / STM32CubeProgrammer**: Flashing compiled binary (`LED_MANIP.hex`) into flash memory via SWD.

## 5. Theory
### Mechanical Switch Bouncing
When a mechanical push-button switch is pressed or released, its internal physical metal contacts do not make or break connection instantaneously. Due to mechanical elasticity and surface imperfections, the contacts rebound against each other for several milliseconds (typically 5 ms to 20 ms) before settling into a steady electrical state. A high-speed microcontroller executing instructions at 72 MHz (13.88 ns per instruction cycle) would interpret these rapid oscillations as dozens of independent press and release events, leading to erratic system behavior.

### Non-Blocking Debouncing vs Blocking Delays
Traditional debouncing often uses crude blocking delays (e.g., `HAL_Delay(20)`), which stalls the CPU pipeline in a tight polling loop. In real-time embedded systems, blocking delays degrade responsiveness, starve communication stacks, and prevent concurrent task processing. 
The non-blocking approach employs the ARM SysTick periodic interrupt timer via `HAL_GetTick()`. By latching timestamps when an initial pin transition occurs and continuously verifying that the input remains stable across a 50 ms time gate (`now - debounceTime >= 50`), contact bounce is filtered out seamlessly while allowing the main execution loop to execute uninterrupted.

### Event Classification (Short Press vs Long Press)
By recording the timestamp when a valid transition from idle to active occurs (`pressTime = now`) and calculating the elapsed duration when released (`duration = now - pressTime`), the firmware distinguishes two user intents:
1. **Short Press (`duration < 1000 ms`)**: Immediately inverts the LED state. If the system was in continuous blinking mode, it terminates blinking and holds the LED steady.
2. **Long Press (`duration >= 1000 ms`)**: Activates continuous 1 Hz blinking mode (500 ms ON, 500 ms OFF) driven by an independent non-blocking timing loop.

## 6. Actual Pin Configuration
| Pin Name | Physical Pin | Direction | Mode | Pull-Up / Pull-Down | Connected Hardware |
|---|---|---|---|---|---|
| **PA1** | Pin 11 | Input | `GPIO_MODE_INPUT` | **Internal Pull-Down** (`GPIO_PULLDOWN`) | Push-Button Switch (to +3.3V) |
| **PA2** | Pin 12 | Output | `GPIO_MODE_OUTPUT_PP` | No Pull (`GPIO_NOPULL`) | Anode of LED via 330 Ω resistor |
| **PA13** | Pin 34 | Alternate | SWDIO | Internal Pull-Up | ST-LINK SWDIO (Debug Data) |
| **PA14** | Pin 37 | Alternate | SWCLK | Internal Pull-Down | ST-LINK SWCLK (Debug Clock) |
| **PD0 / PD1**| Pins 5, 6 | Oscillator | RCC_OSC_IN / RCC_OSC_OUT | None | 8 MHz Quartz Crystal Oscillator |
| **3.3V** | Pins 18, 40 | Power Rail | VDD | Power | Breadboard VCC rail |
| **GND** | Pins 19, 20 | Ground Rail | VSS | Ground | Breadboard common ground |

> [!NOTE]
> **Observation Note on Polarity Discrepancy**: 
> Early student laboratory observation notes initially sketched a pull-up configuration (`state == GPIO_PIN_RESET`). However, the actual repository firmware (`main.c` lines 102–148 and `LED_MANIP.ioc`) configures PA1 as `GPIO_PULLDOWN` and tests for `state == GPIO_PIN_SET`. This establishes an **active-HIGH** circuit topology where pressing the tactile switch connects PA1 to +3.3V.

## 7. Circuit Diagram
![Circuit Diagram](circuit.png)

## 8. CubeMX / Peripheral Configuration
- **RCC Configuration**:
  - High Speed Clock (HSE): Crystal/Ceramic Resonator (8 MHz external quartz).
  - Low Speed Clock (LSE): Disabled.
- **Clock Tree Synthesis**:
  - Input Crystal: 8 MHz
  - PLL Source: HSE / 1
  - PLL Multiplier: ×9
  - System Clock Mux: PLLCLK -> SYSCLK = 72 MHz
  - AHB Prescaler: /1 -> HCLK = 72 MHz (SysTick timer base = 1 kHz / 1 ms tick)
  - APB1 Prescaler: /2 -> 36 MHz
  - APB2 Prescaler: /1 -> 72 MHz
- **GPIO Pin Configuration**:
  - **PA1**:
    - GPIO mode: Input mode
    - GPIO Pull-up/Pull-down: **Pull-down**
  - **PA2**:
    - GPIO mode: Output Push-Pull
    - GPIO Pull-up/Pull-down: No pull-up and no pull-down
    - Maximum output speed: Low (2 MHz)
- **SYS Debug**: Serial Wire (SWDIO on PA13, SWCLK on PA14).

## 9. Key HAL APIs Used
- `HAL_Init()`: Initializes the Flash interface, Systick, and data caches.
- `SystemClock_Config()`: Sets up PLL, flash latency (2 wait states for 72 MHz), and bus clocks.
- `__HAL_RCC_GPIOA_CLK_ENABLE()`: Supplies clock gating to Port A GPIO peripheral register bank.
- `HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)`: Configures pin mode, pull direction, and speed registers (CRL/CRH).
- `HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)`: Reads physical voltage level of the input data register (IDR).
- `HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)`: Sets or clears bits in the Bit Set/Reset Register (BSRR).
- `HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)`: Inverts the corresponding pin state in the output data register (ODR).
- `HAL_GetTick(void)`: Returns current value of the global 32-bit tick counter incremented in the SysTick interrupt service routine every 1 millisecond.

## 10. Source Code Logic
The core button handling logic is encapsulated inside `Button_Handle()` in `Core/Src/main.c`:

```c
/* USER CODE BEGIN 0 */
void Button_Handle(void)
{
    static uint32_t lastDebounceTime = 0;
    static uint8_t buttonState = 0;
    static uint8_t lastButtonState = 0;
    static uint32_t pressStartTime = 0;
    static uint8_t isLongPressHandled = 0;
    static uint8_t blinkingMode = 0;
    static uint32_t lastBlinkTime = 0;

    uint32_t currentTime = HAL_GetTick();
    uint8_t reading = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET) ? 1 : 0;

    if (reading != lastButtonState)
    {
        lastDebounceTime = currentTime;
    }

    if ((currentTime - lastDebounceTime) > 50)
    {
        if (reading != buttonState)
        {
            buttonState = reading;

            if (buttonState == 1) // Button pressed (active-high)
            {
                pressStartTime = currentTime;
                isLongPressHandled = 0;
            }
            else // Button released
            {
                uint32_t pressDuration = currentTime - pressStartTime;

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
                        blinkingMode = 0;
                    }
                    else
                    {
                        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
                    }
                }
            }
        }
    }

    lastButtonState = reading;

    // Handle blinking mode concurrently without blocking
    if (blinkingMode)
    {
        if ((currentTime - lastBlinkTime) >= 500)
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
            lastBlinkTime = currentTime;
        }
    }
}
/* USER CODE END 0 */
```

## 11. Algorithm
1. **Initialize Core**: Execute `HAL_Init()` and configure the system clock to 72 MHz via HSE PLL.
2. **Initialize Peripherals**: Enable Port A clock; configure PA1 as input with pull-down resistor; configure PA2 as push-pull output.
3. **Enter Superloop**: Repeatedly invoke `Button_Handle()`.
4. **Sample Pin State**: Read PA1 voltage using `HAL_GPIO_ReadPin()`. If pin level differs from previous loop sample, reset `lastDebounceTime` to `currentTime`.
5. **Debounce Verification**: When `currentTime - lastDebounceTime > 50 ms`, verify if physical reading represents a true state transition.
6. **Press Event Processing**:
   - If stable state transitions from 0 to 1 (Press): Record `pressStartTime = currentTime` and reset `isLongPressHandled`.
   - If stable state transitions from 1 to 0 (Release): Calculate `pressDuration = currentTime - pressStartTime`.
7. **Action Classification**:
   - If `pressDuration >= 1000 ms`: Set `blinkingMode = 1`.
   - If `pressDuration < 1000 ms`: If `blinkingMode == 1`, set `blinkingMode = 0` (halt blinking); otherwise, toggle PA2 immediately.
8. **Blink Engine**: If `blinkingMode == 1` and `currentTime - lastBlinkTime >= 500 ms`, toggle PA2 and update `lastBlinkTime`.

## 12. Pseudocode
```text
FUNCTION Button_Handle():
    STATIC lastDebounceTime = 0, buttonState = 0, lastButtonState = 0
    STATIC pressStartTime = 0, blinkingMode = 0, lastBlinkTime = 0

    now = HAL_GetTick()
    reading = READ_PIN(PA1)

    IF reading != lastButtonState THEN
        lastDebounceTime = now
    END IF

    IF (now - lastDebounceTime) > 50 THEN
        IF reading != buttonState THEN
            buttonState = reading
            IF buttonState == 1 THEN
                pressStartTime = now
            ELSE
                duration = now - pressStartTime
                IF duration >= 1000 THEN
                    blinkingMode = 1
                ELSE
                    IF blinkingMode == 1 THEN
                        blinkingMode = 0
                    ELSE
                        TOGGLE_PIN(PA2)
                    END IF
                END IF
            END IF
        END IF
    END IF

    lastButtonState = reading

    IF blinkingMode == 1 AND (now - lastBlinkTime) >= 500 THEN
        TOGGLE_PIN(PA2)
        lastBlinkTime = now
    END IF
END FUNCTION
```

## 13. Flowchart
![Flowchart](flowchart.png)

## 14. Expected Output
- **At Boot / Idle**: LED on PA2 remains OFF.
- **Short Press (< 1 second)**: Each distinct tap of the button toggles PA2 from OFF to ON or ON to OFF cleanly without bouncing glithes.
- **Long Press (≥ 1 second)**: Holding the button for 1 second or longer and releasing triggers continuous blinking at 1 Hz (500 ms ON, 500 ms OFF).
- **Short Press during Blinking**: Immediately stops the blinking cycle and holds the current LED state.

## 15. Actual Observations
- The non-blocking 50 ms debounce threshold completely eliminated false contact toggles observed during rapid button clicking.
- Oscilloscope measurement confirms the blink toggles every 500 ms ($\pm 1$ ms jitter bounded by loop execution latency).
- **Polarity Note Grounding**: When wired with active-high polarity (button to 3.3V, PA1 pull-down), the system functions flawlessly according to firmware logic. Connecting button to ground without changing firmware internal pull registers results in permanent inactive state.

## 16. Troubleshooting
1. **LED Never Illuminates**:
   - Check LED orientation (long lead = Anode to PA2, short lead = Cathode to GND).
   - Verify PA2 voltage with a multimeter when driven HIGH (should measure ~3.3V).
2. **Button Not Detected / Floating Input**:
   - Verify that PA1 is configured with internal pull-down (`GPIO_PULLDOWN`). If configured with `GPIO_NOPULL`, external electromagnetic noise causes erratic false triggers.
3. **ST-LINK Connection Error ("Cannot connect to target")**:
   - Check SWDIO (PA13) and SWCLK (PA14) jumper connections.
   - Ensure the Blue Pill `BOOT0` jumper is set to 0 (Run from Flash mode). Press hardware reset while initiating flash if SWD lines were locked.

## 17. Limitations
- Does not utilize hardware external interrupts (EXTI), requiring the CPU superloop to continuously poll `Button_Handle()`.
- If CPU is executing lengthy blocking routines elsewhere in `main()`, button event latency will increase proportionally.
- Tick counter roll-over occurs every 49.7 days ($2^{32}$ ms), handled correctly by unsigned subtraction (`currentTime - lastDebounceTime`).

## 18. Result
The GPIO input and output peripherals of the STM32F103C8T6 microcontroller were successfully configured and verified. Non-blocking switch debouncing and dual-mode duration classification (short press toggle, long press 1 Hz blink) operated accurately in accordance with laboratory requirements.

## 19. Viva Questions with Answers
1. **Q: What is switch bounce, and what causes it?**  
   *A:* Switch bounce is the rapid mechanical bouncing of physical metal switch contacts upon closure or release, generating spurious transient voltage transitions lasting 5–20 ms before reaching a stable state.
2. **Q: Why is non-blocking debouncing preferred over `HAL_Delay()`?**  
   *A:* `HAL_Delay()` stalls the processor in a busy-wait loop, preventing other peripherals, communication interfaces, or control routines from executing. Non-blocking debouncing using `HAL_GetTick()` allows concurrent execution.
3. **Q: How does the ARM Cortex-M SysTick timer work in STM32 HAL?**  
   *A:* SysTick is a dedicated 24-bit down-counter running from HCLK. STM32 HAL configures it to trigger an interrupt every 1 ms, incrementing the 32-bit global variable `uwTick`.
4. **Q: What is the purpose of push-pull mode in STM32 GPIO output?**  
   *A:* Push-pull mode uses a complementary pair of transistors (P-MOS and N-MOS) to actively drive the output pin to VDD (logic HIGH) or VSS (logic LOW) with low impedance.
5. **Q: What is the difference between active-high and active-low switch wiring?**  
   *A:* In active-high wiring, pressing the switch connects the pin to VDD (logic 1), requiring a pull-down resistor when open. In active-low wiring, pressing the switch connects the pin to GND (logic 0), requiring a pull-up resistor.
6. **Q: How does unsigned arithmetic in C protect `HAL_GetTick()` timing across overflow?**  
   *A:* Because `uint32_t` arithmetic wraps modulo $2^{32}$, the subtraction `(currentTime - lastDebounceTime)` yields the correct elapsed time even when `currentTime` wraps from `0xFFFFFFFF` to `0x00000000`, provided the elapsed interval is less than $2^{32}-1$.
7. **Q: What is the maximum clock frequency of the STM32F103C8T6?**  
   *A:* 72 MHz, achieved by multiplying an 8 MHz external crystal by 9 using the internal Phase-Locked Loop (PLL).
8. **Q: Which register directly drives GPIO pin outputs without read-modify-write overhead?**  
   *A:* The Bit Set/Reset Register (`GPIOx->BSRR`). Writing a 1 to bits 0–15 sets the pin; writing a 1 to bits 16–31 resets the pin atomically.

## 20. File Descriptions
| File Name | Location | Description |
|---|---|---|
| `main.c` | `Core/Src/` | Contains system clock setup, GPIO initialization, `Button_Handle()` implementation, and superloop |
| `main.h` | `Core/Inc/` | Peripheral definitions, macro configurations, and standard HAL header inclusions |
| `LED_MANIP.ioc` | Project Root | STM32CubeMX graphical configuration project for pinout and clock tree |
| `LED_MANIP.uvprojx` | `MDK-ARM/` | Keil µVision project file containing compiler options, device targets, and linker scripts |
| `circuit.png` | Project Root | Detailed hardware schematic and pinout diagram |
| `flowchart.png` | Project Root | State transition and execution flow diagram of the debouncing algorithm |
