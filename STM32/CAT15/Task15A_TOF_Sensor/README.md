# Continuous Lab Evaluation 15: Task 15A — Time-of-Flight (ToF) Distance Sensing

## 1. Experiment Title
Continuous Lab Evaluation 15 — Task 15A: Real-Time Distance Measurement, Digital Smoothing, and Zone Classification using STMicroelectronics VL53L1X Time-of-Flight Sensor on STM32F401RE Nucleo-64

## 2. Objective / Aim
To configure the I²C1 peripheral of the STM32F401RET6 microcontroller in Fast Mode (400 kHz); interface the STMicroelectronics VL53L1X Time-of-Flight (ToF) laser ranging sensor over the Inter-Integrated Circuit (I²C) bus; integrate ST's VL53L1X Ultra Lite Driver (ULD) API; initialize ranging in medium distance mode (up to 3 meters) with a 50 ms timing budget and 100 ms inter-measurement period; validate measurement status flags (`rangeStatus == 0`); apply a 5-sample circular moving average smoothing filter; classify distances into discrete navigation zones (`STOP`, `TURN_LEFT`, `SLOW`, `CLEAR`); encode the results into structured JSON telemetry; transmit telemetry packets over USART1 at 9600 baud every 500 ms to an ESP8266 Wi-Fi co-processor for MQTT forwarding; and output real-time human-readable diagnostic logs over USART2 at 115200 baud to the PC console via the ST-LINK Virtual COM Port.

For comprehensive architectural diagrams, system flow charts, MQTT broker configurations, and Node-RED dashboard details, refer to the [CAT15 Comprehensive Documentation](../README.md).

---

## 3. Hardware Components
| Component | Specification | Quantity | Role / Remarks |
| :--- | :--- | :---: | :--- |
| **Development Board** | NUCLEO-F401RE | 1 | 32-bit ARM Cortex-M4 MCU @ 84 MHz with integrated ST-LINK/V2-1 |
| **ToF Distance Sensor** | STMicroelectronics VL53L1X | 1 | 940 nm VCSEL laser emitter with SPAD receiving array (up to 4 m) |
| **Wireless Co-Processor** | Wemos D1 Mini (ESP8266) | 1 | Wi-Fi client and MQTT publisher bridge (802.11 b/g/n) |
| **I²C Pull-Up Resistors** | Integrated on breakout | 2 | Pulls SCL (`PB6`) and SDA (`PB7`) lines up to +3.3V |
| **USB Cable** | USB Type-A to Mini-B | 1 | Powers board and provides ST-LINK Virtual COM Port communication |
| **Interconnect Jumpers** | Female-to-Female / Male-to-Female | 8 | Connects Nucleo Morpho headers to VL53L1X and Wemos D1 Mini |

---

## 4. Software Tools
- **STM32CubeMX**: Peripheral initialization for I²C1, USART1, USART2, and clock tree synthesis (`TOF_sensor_test2harish.ioc`).
- **Keil MDK-ARM (µVision v5)**: C firmware compilation, linking, and hex file generation (`MDK-ARM/TOF_sensor_test2harish.uvprojx`).
- **STMicroelectronics VL53L1X ULD API**: Official ST driver library (`VL53L1X_api.c`, `VL53L1X_calibration.c`, `vl53l1_platform.c`).
- **Serial Terminal Emulator**: PuTTY, Tera Term, or Minicom (115200 baud, 8N1) on the ST-LINK Virtual COM Port.
- **Node-RED & MQTT**: Flow processing and real-time visualization (`../NodeRED/VL53L1X_NodeRED_Distance_Studio.json`).

---

## 5. Pin Configuration Summary
| Pin Name | Peripheral | Function | Target Connection |
| :--- | :--- | :--- | :--- |
| **PB6** | I2C1_SCL | I²C Serial Clock (400 kHz Fast Mode) | VL53L1X SCL |
| **PB7** | I2C1_SDA | I²C Serial Data (400 kHz Fast Mode) | VL53L1X SDA |
| **PA9** | USART1_TX | Asynchronous Serial Transmit (9600 baud) | Wemos D1 Mini RX (`GPIO3`) |
| **PA10** | USART1_RX | Asynchronous Serial Receive (9600 baud) | Wemos D1 Mini TX (`GPIO1`) |
| **PA2** | USART2_TX | Asynchronous Serial Transmit (115200 baud)| ST-LINK Virtual COM Port (PC Debug) |
| **PA3** | USART2_RX | Asynchronous Serial Receive (115200 baud) | ST-LINK Virtual COM Port (PC Debug) |
| **+3.3V** | Power Rail | +3.3V Regulated Output | VL53L1X VCC & Wemos 3V3 Rail |
| **GND** | Ground Rail| Common Reference Ground | VL53L1X GND & Wemos GND |

---

## 6. Keil Project Instructions

### 6.1 Project Location
The Keil project file is located at:
```text
Task15A_TOF_Sensor/MDK-ARM/TOF_sensor_test2harish.uvprojx
```

### 6.2 Opening the Project
1. Launch **Keil µVision 5**.
2. Go to **Project** $\rightarrow$ **Open Project...**
3. Navigate to `Task15A_TOF_Sensor/MDK-ARM/` and select `TOF_sensor_test2harish.uvprojx`.

### 6.3 Building the Firmware
1. In the target dropdown, select `TOF_sensor_test2harish`.
2. Press **F7** or click **Project** $\rightarrow$ **Build Target**.
3. All source dependencies resolve from relative directories:
   - Application sources: `Core/Src/` (`main.c`, `vl53l1_platform.c`, `VL53L1X_api.c`, etc.)
   - Header files: `Core/Inc/`
   - HAL Drivers: `Drivers/STM32F4xx_HAL_Driver/`
   - CMSIS Core & Device: `Drivers/CMSIS/`
   - Startup Vector: `MDK-ARM/startup_stm32f401xe.s`
4. The build generates the executable binary `TOF_sensor_test2harish.hex`.

### 6.4 Flashing to Target
1. Connect the STM32F401RE Nucleo-64 board to the host PC via USB.
2. In Keil µVision, ensure the debug adapter is configured as **ST-Link Debugger** under **Project** $\rightarrow$ **Options for Target** $\rightarrow$ **Debug**.
3. Press **F8** or click **Flash** $\rightarrow$ **Download**.
4. Press the black **B2 RESET** push button on the Nucleo board to boot the new firmware.

---

## 7. Telemetry & Navigation Zone Decision Logic
The firmware applies a 5-sample circular moving average smoothing algorithm (`Avg_Add`) to valid sensor distance readings (`rangeStatus == 0`) and categorizes proximity into navigation zones:

$$\text{Distance} \le 300\text{ mm} \implies \textbf{STOP} \quad (\text{"Stop and reverse a little"})$$
$$301\text{ mm} \le \text{Distance} \le 500\text{ mm} \implies \textbf{TURN\_LEFT} \quad (\text{"Turn left, then re-check the distance"})$$
$$501\text{ mm} \le \text{Distance} \le 1000\text{ mm} \implies \textbf{SLOW} \quad (\text{"Slow down"})$$
$$\text{Distance} > 1000\text{ mm} \implies \textbf{CLEAR} \quad (\text{"Path clear, full speed"})$$

Every 500 ms, the STM32 formats this state into a compact JSON string and transmits it via USART1:
```json
{"distance":2346,"raw":2351,"zone":"CLEAR","suggestion":"Path clear, full speed"}
```

---

## 8. Related Documentation
For the complete laboratory evaluation report, circuit wiring diagrams, ESP8266 MQTT bridge configuration, and Node-RED live dashboard screenshots, see:
* [CAT15 Main Documentation](../README.md)
* [Circuit & System Architecture Diagram](../project_files/circuit.png)
* [Node-RED Distance Studio Flow](../NodeRED/VL53L1X_NodeRED_Distance_Studio.json)
