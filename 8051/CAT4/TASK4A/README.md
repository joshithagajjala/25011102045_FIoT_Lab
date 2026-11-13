### EXP
CAT4

### DATE
03/08/2026 (Week 4)

### NAME OF EXP
Matrix Keypad, 7-Segment Display, and Multi-LED Controller

### HARDWARE SETUP / PLATFORM
- **Microcontroller:** AT89S52 (8051 architecture)
- **Board:** AmBTech Labs 8051 Development Board (11.0592 MHz clock, nominal 5 V)
- **Keypad:** 4×4 matrix keypad on Port 2 (rows R0–R3 on P2.7–P2.4, columns C0–C3 on P2.3–P2.0).
- **Outputs:** Common-anode 7-segment display on P0.0–P0.7; four active-low LEDs on P1.0–P1.3 with proposed 1 kΩ resistors (board circuitry unverified).
- **Software:** Keil µVision (IDE/compiler) and ProgISP v1.72 (flasher).

### TASK NO
TASK4A — Scan a 4×4 matrix keypad to display pressed digits on a 7-segment display and trigger corresponding LED operating modes.

### Circuit Diagram
![Circuit Diagram](circuit.png)

A 4×4 keypad interfaces to Port 2, driving a 7-segment display on Port 0 and four active-low LEDs on P1.0–P1.3.

### Core Logic / Pseudocode / Code Base
1. Initialize ports (`P0 = P1 = P2 = 0xFF`) and display digit `'0'`.
2. Scan keypad by grounding rows (P2.7–P2.4) and reading columns (P2.3–P2.0).
3. Display detected numeric keys ('0'–'9') on the 7-segment display via `P0`.
4. Trigger LED commands via `led_control(key)`:
   - **Keys 1–4:** Individual LEDs (`0xFE`, `0xFD`, `0xFB`, `0xF7`).
   - **Key 5:** Running LED sequence.
   - **Key 6:** Alternating pattern (`0xF5` $\leftrightarrow$ `0xFA`).
   - **Key 7:** 4-bit binary counter (`P1 = ~(count & 0x0F)`).
   - **Key 8:** All-LED flash (`0xF0` $\leftrightarrow$ `0xFF`).
   - **Key 9:** All LEDs OFF (`0xFF`).
   - **Key 0:** Reset display to `'0'` and LEDs OFF.

```c
#include <reg51.h>

int keypad_scan(void) {
    static const int keypad[4][4] = {
        {'1', '2', '3', 'A'}, {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}
    };
    static const unsigned char rowPin[4] = {7, 6, 5, 4};
    static const unsigned char colPin[4] = {3, 2, 1, 0};
    int row, col;
    for (row = 0; row < 4; row++) {
        P2 |= 0xF0;
        P2 &= ~(1 << rowPin[row]);
        for (col = 0; col < 4; col++) {
            if ((P2 & (1 << colPin[col])) == 0)
                return keypad[row][col];
        }
    }
    return -1;
}

void display_digit(unsigned char value) {
    static const unsigned char seg_code[] = {
        0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90
    };
    if (value >= '0' && value <= '9')
        P0 = seg_code[value - '0'];
}
```

### INFERENCE
1. Matrix keypad scanning using row-grounding and column-polling efficiently reads 16 keys using 8 GPIO pins on Port 2.
2. Port 0 directly decodes common-anode 7-segment numerals while Port 1 drives active-low multi-mode LED patterns.
3. Embedded polling inside active pattern routines maintains keypad responsiveness during pattern execution.
