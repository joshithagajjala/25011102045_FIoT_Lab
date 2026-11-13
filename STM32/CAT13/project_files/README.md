# Continuous Lab Evaluation 13: BME280 Sensor Interfacing Using I2C and Bosch Sensor API

## 1. Experiment Title
Continuous Lab Evaluation 13 — Environmental Sensing (Temperature, Pressure, and Humidity) via I2C Bus Interfacing and Official Bosch Sensortec API on STM32F401RE Nucleo-64

## 2. Aim
To configure the I2C1 peripheral of the STM32F401RET6 microcontroller in Master Standard Mode (100 kHz); interface a Bosch Sensortec BME280 digital environmental sensor over the Inter-Integrated Circuit (I2C) serial bus; integrate the official vendor-provided Bosch BME280 Sensor API (modern API using `dev.intf_ptr`, `dev.delay_us`, and `struct bme280_settings`); implement custom HAL callback wrappers (`user_i2c_read`, `user_i2c_write`, `user_delay_us`); configure the sensor with oversampling 1X and filter OFF; acquire compensated temperature (°C), barometric pressure (hPa), and relative humidity (%RH); and stream formatted telemetry to a PC console over USART2 via the integrated ST-LINK Virtual COM Port at 115200 baud.

## 3. Hardware Components
| Component | Specification | Quantity | Role / Remarks |
|---|---|---|---|
| Development Board | NUCLEO-F401RE | 1 | 32-bit ARM Cortex-M4 MCU @ 84 MHz with integrated ST-LINK/V2-1 |
| Environmental Sensor | Bosch Sensortec BME280 | 1 | Combined digital humidity, barometric pressure, and temperature sensor |
| I2C Pull-Up Resistors | 4.7 kΩ (1/4 W) | 2 | Pulls SCL (PB6) and SDA (PB7) lines up to +3.3V |
| USB Cable | USB Type-A to Mini-B | 1 | Powers board and provides ST-LINK Virtual COM Port communication |
| Breadboard & Wires | Solderless breadboard + jumpers | 1 | Interconnects sensor module with Nucleo Morpho/Arduino headers |

## 4. Software Tools
- **STM32CubeMX**: Peripheral initialization for I2C1 and USART2, clock tree synthesis (`I2C-1.ioc`).
- **Keil MDK-ARM (µVision v5)**: C firmware compilation, linking, and hex file generation (`I2C-1.uvprojx`).
- **Bosch Sensortec BME280 Sensor API**: Official C driver files (`bme280.c`, `bme280.h`, `bme280_defs.h`).
- **Serial Terminal Emulator**: PuTTY, Tera Term, or Minicom (115200 baud, 8N1) for telemetry display.

## 5. Theory
### The I2C Bus Protocol
I2C is a synchronous, multi-master, multi-slave two-wire serial bus using two bidirectional open-drain lines pulled up to VDD by external resistors:
- **SCL (Serial Clock)**: Generated exclusively by the master device (PB6 on STM32).
- **SDA (Serial Data)**: Bidirectional data line (PB7 on STM32).
Bus transactions begin with a START condition, transfer 8-bit data bytes with a 9th ACK/NACK clock pulse, and terminate with a STOP condition.

### Modern Bosch Sensortec BME280 Driver API
The repository integrates the modern Bosch Sensortec BME280 Sensor API (v3.5+ / v4.0):
1. **Device Structure (`struct bme280_dev`)**:
   - `intf_ptr`: Generic interface pointer storing the 7-bit device address (`&dev_addr`, `0x76`).
   - `intf`: Set to `BME280_I2C_INTF`.
   - `read`: Callback pointer matching `bme280_read_fptr_t`:
     `int8_t (*read)(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)`.
   - `write`: Callback pointer matching `bme280_write_fptr_t`:
     `int8_t (*write)(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr)`.
   - `delay_us`: Microsecond delay callback matching `bme280_delay_us_fptr_t`:
     `void (*delay_us)(uint32_t period, void *intf_ptr)`.
2. **Settings Structure (`struct bme280_settings`)**:
   Configuration settings are maintained in an independent structure and applied via:
   `bme280_set_sensor_settings(desired_settings, &settings, &dev)`.
   - `osr_h = BME280_OVERSAMPLING_1X`
   - `osr_p = BME280_OVERSAMPLING_1X`
   - `osr_t = BME280_OVERSAMPLING_1X`
   - `filter = BME280_FILTER_COEFF_OFF` (Internal IIR filter is explicitly OFF).
   - Sensor mode is set to normal via `bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &dev)`.

### Code Structure Analysis: The Apparent Extra Closing Brace
In `Core/Src/main.c`, lines 153–163 appear formatted as follows:
```c
153: HAL_Delay(1000); // Poll data once per second 
154:   /* USER CODE END 3 */
155: }
156: 
157: /**
158:   * @brief System Clock Configuration
159:   * @retval None
160:   */
161: }
162: 
163: void SystemClock_Config(void)
```
- **Structural Analysis**:
  - Line 81 opens `int main(void) {`.
  - Line 136 opens the superloop `while (1) {`.
  - Line 155 is the closing brace `}` for the `while (1)` loop.
  - Lines 157–160 are Doxygen comments placed before the end of `main()`.
  - Line 161 is the closing brace `}` for `main()`.
  - Line 163 begins `void SystemClock_Config(void)`.
- **Static Syntax Inspection**:
  Because C preprocessors strip comments prior to parsing, the compiler sees line 155 closing `while (1)` and line 161 closing `main()`. The total count of open and closing braces across the translation unit balances cleanly to zero. This observation represents a static syntax inspection of the source file's brace balance, not proof of successful compilation or a completed toolchain build.

## 6. Actual Pin Configuration
| Pin Name | Header Pin | Alternate Function | Mode | Pull Configuration | Connected Device Pin |
|---|---|---|---|---|---|
| **PB6** | CN10 Pin 17 (D10) | `I2C1_SCL` | Open-Drain, High Speed | External 4.7 kΩ to 3.3V | BME280 SCL (Serial Clock) |
| **PB7** | CN7 Pin 21 (D11) | `I2C1_SDA` | Open-Drain, High Speed | External 4.7 kΩ to 3.3V | BME280 SDA (Serial Data) |
| **PA2** | CN10 Pin 35 | `USART2_TX` | Alternate Push-Pull | None | ST-LINK VCP RX (Telemetry) |
| **PA3** | CN10 Pin 37 | `USART2_RX` | Alternate Function | None | ST-LINK VCP TX (Telemetry) |
| **+3.3V** | CN6 Pin 4 | VDD | Power Rail | Power | BME280 VDD & CSB |
| **GND** | CN6 Pin 6 | VSS | Ground Reference | Ground | BME280 GND & SDO (Address 0x76) |

## 7. Circuit Diagram
![Circuit Diagram](circuit.png)

## 8. CubeMX / Peripheral Configuration
- **Clock Tree Synthesis**: 84 MHz SYSCLK.
- **I2C1 Configuration**:
  - Mode: I2C Standard Mode (100 kHz)
  - Addressing Mode: 7-bit (`0x76`)
- **USART2 Configuration**:
  - Mode: Asynchronous, 115200 bps, 8N1 routed to ST-LINK Virtual COM Port.

## 9. Key HAL APIs Used
- `HAL_I2C_Init()`: Configures I2C clock generation and timing registers.
- `HAL_I2C_Mem_Read()`: Performs burst reading of sensor registers over I2C.
- `HAL_I2C_Mem_Write()`: Performs register writes to configure sensor parameters.
- `HAL_UART_Transmit()`: Streams formatted text telemetry across USART2 to PC console.

## 10. Source Code Logic
The actual implementation in `Core/Src/main.c`:

```c
/* USER CODE BEGIN 4 */
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) { 
    uint8_t dev_addr = *(uint8_t*)intf_ptr; 
    if (HAL_I2C_Mem_Read(&hi2c1, (dev_addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, len, HAL_MAX_DELAY) == HAL_OK) { 
        return BME280_OK; 
    } 
    return BME280_E_COMM_FAIL; 
} 
 
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) { 
    uint8_t dev_addr = *(uint8_t*)intf_ptr; 
    if (HAL_I2C_Mem_Write(&hi2c1, (dev_addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)reg_data, len, HAL_MAX_DELAY) == HAL_OK) { 
        return BME280_OK; 
    } 
    return BME280_E_COMM_FAIL; 
} 
 
void user_delay_us(uint32_t period, void *intf_ptr) { 
    HAL_Delay((period / 1000) + 1); 
} 
/* USER CODE END 4 */

/* USER CODE BEGIN 2 */
static uint8_t dev_addr = BME280_I2C_ADDR_PRIM; // 0x76
dev.intf_ptr = &dev_addr; 
dev.intf = BME280_I2C_INTF; 
dev.read = user_i2c_read; 
dev.write = user_i2c_write; 
dev.delay_us = user_delay_us; 

rslt = bme280_init(&dev); 

struct bme280_settings settings;
settings.osr_h = BME280_OVERSAMPLING_1X; 
settings.osr_p = BME280_OVERSAMPLING_1X; 
settings.osr_t = BME280_OVERSAMPLING_1X; 
settings.filter = BME280_FILTER_COEFF_OFF; // Filter explicitly OFF

rslt = bme280_set_sensor_settings(BME280_SEL_OSR_PRESS | BME280_SEL_OSR_TEMP | BME280_SEL_OSR_HUM | BME280_SEL_FILTER, &settings, &dev); 
rslt = bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &dev); 
/* USER CODE END 2 */

/* USER CODE BEGIN WHILE */
while (1)
{
    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev); 
    if (rslt == BME280_OK) { 
        temperature = comp_data.temperature;
        pressure = comp_data.pressure / 100.0f; // Pa to hPa
        humidity = comp_data.humidity;
        
        char uart_buf[100];
        int len = snprintf(uart_buf, sizeof(uart_buf), "Temperature: %.2f C\r\nPressure: %.2f hPa\r\nHumidity: %.2f %%RH\r\n\n", temperature, pressure, humidity);
        HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, len, HAL_MAX_DELAY);
    } 
    HAL_Delay(1000);
}
/* USER CODE END WHILE */
```

## 11. Algorithm
1. **Core Initialization**: Initialize HAL, configure 84 MHz system clock, initialize I2C1 and USART2.
2. **Device Configuration**: Assign `dev.intf_ptr = &dev_addr`, `dev.read = user_i2c_read`, `dev.write = user_i2c_write`, `dev.delay_us = user_delay_us`.
3. **Execute `bme280_init(&dev)`**: Read chip identification register (`0xD0`) and load calibration trim parameters.
4. **Sensor Settings**: Instantiate `struct bme280_settings`, assign 1X oversampling, set `settings.filter = BME280_FILTER_COEFF_OFF`, and invoke `bme280_set_sensor_settings()`.
5. **Set Normal Mode**: Call `bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &dev)`.
6. **Periodic Acquisition**:
   - Call `bme280_get_sensor_data(BME280_ALL, &comp_data, &dev)`.
   - Format temperature (°C), pressure (hPa), and humidity (%RH) using `snprintf()`.
   - Transmit over USART2 to PC console.
   - Wait 1000 ms via `HAL_Delay()`.
7. **Repeat**: Return to Step 6.

## 12. Pseudocode
```text
INITIALIZE HAL, Clocks, I2C1 (100kHz), USART2 (115200)

dev.intf_ptr = &dev_addr (0x76)
dev.read = user_i2c_read
dev.write = user_i2c_write
dev.delay_us = user_delay_us

STATUS = bme280_init(&dev)
IF STATUS != OK THEN HALT

settings.osr_h = 1X
settings.osr_p = 1X
settings.osr_t = 1X
settings.filter = FILTER_COEFF_OFF
bme280_set_sensor_settings(ALL_SEL, &settings, &dev)
bme280_set_sensor_mode(POWERMODE_NORMAL, &dev)

LOOP FOREVER:
    bme280_get_sensor_data(ALL, comp_data, dev)
    p_hpa = comp_data.pressure / 100.0
    msg = FORMAT("Temperature: %.2f C\r\nPressure: %.2f hPa\r\nHumidity: %.2f %%RH\r\n\n", comp_data.temp, p_hpa, comp_data.hum)
    UART_TRANSMIT(USART2, msg)
    DELAY_MS(1000)
END LOOP
```

## 13. Flowchart
![Flowchart](flowchart.png)

## 14. Expected Output
On PC serial terminal (connected to ST-LINK Virtual COM Port @ 115200 baud):
```text
Temperature: <ambient> C
Pressure: <ambient> hPa
Humidity: <ambient> %RH
```

## 15. Actual Observations
- The firmware utilizes the modern Bosch API v3.5+ structure (`intf_ptr`, `delay_us`, `struct bme280_settings`).
- The internal filter coefficient is explicitly configured to `BME280_FILTER_COEFF_OFF` (filter disabled).
- In `main.c`, static syntax inspection confirms that the Doxygen header for `SystemClock_Config` is positioned inside the closing braces of `main()` (between line 155 and line 161), which balances cleanly and is syntactically valid in C without proving toolchain compilation.

## 16. Troubleshooting
1. **Sensor Returns Communication Error (`BME280_E_COMM_FAIL`)**:
   - Check I2C address shift: `HAL_I2C_Mem_Read` expects `(dev_addr << 1)`.
   - Ensure external 4.7 kΩ pull-up resistors are connected on PB6 and PB7.
2. **Missing Output in Terminal**:
   - Ensure ST-LINK Virtual COM Port is selected at 115200 baud, 8N1.

## 17. Limitations
- Delay function `user_delay_us` converts microseconds to milliseconds via integer division `(period / 1000) + 1`, which rounds upward.
- With the internal filter OFF, high-frequency acoustic pressure fluctuations (e.g. door slams, wind drafts) are reflected directly in raw measurements without digital smoothing.

## 18. Result
The I2C1 peripheral and Bosch Sensortec BME280 sensor integration was analyzed and documented. The modern Bosch API implementation (`intf_ptr`, `delay_us`, `struct bme280_settings`, `FILTER_OFF`) and the source code structure were verified.

## 19. Viva Questions with Answers
1. **Q: How does the modern Bosch BME280 API receive the hardware device address?**  
   *A:* Through the generic interface pointer `dev.intf_ptr`, which points to the device address variable `dev_addr` (0x76).
2. **Q: What is the filter setting configured in CAT13?**  
   *A:* The internal IIR filter is explicitly set to `BME280_FILTER_COEFF_OFF` (filter disabled).
3. **Q: Why does `(dev_addr << 1)` appear in the HAL I2C calls?**  
   *A:* The I2C protocol transmits the 7-bit device address in bits 7–1 of the first frame byte, reserving bit 0 for the Read/Write flag. STM32 HAL requires the 8-bit left-shifted address.
4. **Q: Does the placement of the Doxygen comment before line 161 in `main.c` cause a compiler error?**  
   *A:* No. The preprocessor strips comments before compilation, so the compiler sees line 155 closing `while (1)` and line 161 closing `main()`, which is syntactically valid.
5. **Q: What unit is pressure reported in by `comp_data.pressure` before conversion?**  
   *A:* The Bosch API outputs pressure in Pascals (Pa). Dividing by 100.0f converts the reading to hectopascals (hPa) / millibars.
6. **Q: What are the roles of PB6 and PB7 in this experiment?**  
   *A:* PB6 serves as I2C1_SCL (Serial Clock) and PB7 serves as I2C1_SDA (Serial Data).
7. **Q: What mode is the sensor set to operate in?**  
   *A:* `BME280_POWERMODE_NORMAL`, which automatically cycles between measurement and standby phases.
8. **Q: Which peripheral transmits data to the PC terminal?**  
   *A:* USART2 on pins PA2 and PA3, routed to the ST-LINK Virtual COM Port.

## 20. File Descriptions
| File Name | Location | Description |
|---|---|---|
| `main.c` | `Core/Src/` | Contains peripheral initialization, modern Bosch API callback wrappers, and acquisition loop |
| `bme280.c` | `Core/Src/` | Official Bosch Sensortec BME280 sensor driver implementation |
| `bme280.h` | `Core/Inc/` | Public API declarations, function prototypes, and data structures |
| `bme280_defs.h`| `Core/Inc/` | Register addresses, bit masks, and constant macro definitions |
| `I2C-1.ioc` | Project Root | STM32CubeMX graphical configuration for STM32F401RE (I2C1 and USART2) |
| `I2C-1.uvprojx` | `MDK-ARM/` | Keil MDK project configuration with compiler flags and linker maps |
| `circuit.png` | Project Root | Schematic diagram showing I2C bus wiring between Nucleo-F401RE and BME280 |
| `flowchart.png` | Project Root | Flowchart detailing modern Bosch API initialization and cyclic telemetry |
