### EXP
CAT2

### DATE
20/07/2026 (Week 2)

### NAME OF EXP
Bare-Metal LED Blinking Using AT89S52 Microcontroller

### HARDWARE SETUP / PLATFORM
- **Microcontroller:** AT89S52 (8051 architecture)
- **Board:** AmBTech Labs 8051 Development Board (11.0592 MHz clock, nominal 5 V)
- **GPIO & LEDs:** P1.0 connects to an onboard LED (wiring unverified) and an external LED with a 1 kΩ series resistor to GND.
- **Port Characteristics:** Port 1 has internal pull-ups.
- **Software:** Keil µVision (IDE/compiler) and ProgISP v1.72 (flasher).

### TASK NO
TASK2B — Configure GPIO Port 1 via direct register access to blink an LED at ~1 Hz using a software delay without external libraries.

### Circuit Diagram
![Circuit Diagram](circuit.png)

Both the onboard P1.0 LED and external LED (P1.0 → 1 kΩ resistor → LED → GND) blinked together during the experiment.

### Core Logic / Pseudocode / Code Base
1. Execution begins in `main()`.
2. Enter infinite loop (`while(1)`).
3. Set `P1 = 0x00` to drive Port 1 outputs LOW.
4. Call `delay(500)` to wait ~500 ms using a software loop.
5. Set `P1 = 0xFF` to drive Port 1 outputs HIGH.
6. Call `delay(500)` to wait ~500 ms, repeating the ~1 s cycle.

*Note: The code writes to the entire `P1` register, toggling all Port 1 pins simultaneously.*

```c
#include <reg51.h>

void delay(unsigned int msec) 
{
    unsigned int i, j;
    for (i = 0; i < msec; i++) {
        for (j = 0; j < 127; j++) {
        }
    }
}

void main() 
{
    while(1) 
    {
        P1 = 0x00;      // Drive Port 1 LOW
        delay(500);     // Wait ~500 ms
        
        P1 = 0xFF;      // Drive Port 1 HIGH
        delay(500);     // Wait ~500 ms
    }
}
```

### INFERENCE
1. Directly writing to SFR `P1` demonstrates bare-metal GPIO control without HAL or OS layers.
2. Both the onboard and external LEDs blinked simultaneously as the software delay toggled output states.
3. Software delays provide approximate timing, which varies with clock frequency and compiler optimization.
