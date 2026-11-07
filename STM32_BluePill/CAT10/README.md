# CAT10 – STM32–Wemos Wi-Fi Modem Interface Using AT Commands & HTTP Server

## Experiment Overview

| Field | Details |
| :--- | :--- |
| **EXP** | CAT10 |
| **DATE** | 10/10/2026 |
| **NAME OF EXP** | STM32–Wemos Wi-Fi Modem Interface Using AT Commands & HTTP Server |
| **Course** | 25CS2741 Fundamentals of Internet of Things + Lab, Shiv Nadar University – Chennai |

---

## Hardware Setup / Platform

| Component | Details |
| :--- | :--- |
| **Microcontroller board** | STM32 Blue Pill (STM32 development board) |
| **Wi-Fi module** | Wemos D1 Mini (ESP8266), controlled with AT commands |
| **PC interface** | USB-to-TTL (UART) converter, viewed in a serial terminal (PuTTY / Tera Term) |
| **Software tools** | STM32CubeMX, Keil MDK, STM32 HAL libraries |
| **Network tools** | Web browser, Node-RED |

---

## UART Assignment

| UART | Connection | Purpose |
| :--- | :--- | :--- |
| **USART1** | STM32 ↔ Wemos | AT commands and responses |
| **USART3** | STM32 ↔ PC (USB-TTL converter) | Debug / display |

---

## Pin Connections

### Wemos D1 Mini ↔ STM32 Blue Pill

| Wemos D1 Mini | STM32 Blue Pill | Description |
| :--- | :--- | :--- |
| **TX** | PA10 (USART1_RX) | Data from Wemos to STM32 |
| **RX** | PA9 (USART1_TX) | Data from STM32 to Wemos |
| **GND** | GND | Common ground |
| **5V / 3.3V** | 5V / 3.3V | Power supply |

### USB-TTL Converter ↔ STM32 Blue Pill

| USB-TTL Converter | STM32 Blue Pill | Description |
| :--- | :--- | :--- |
| **RX** | PB10 (USART3_TX) | Debug data from STM32 to PC |
| **TX** | PB11 (USART3_RX) | Data from PC to STM32 |
| **GND** | GND | Common ground |

---

## Task Details

| Task | Description |
| :--- | :--- |
| **Task 9A** | Establish STM32 ↔ Wemos UART communication (send `AT\r\n`, display the Wemos response on the PC terminal) |
| **Task 9B** | Explore the ESP8266 AT command interface (command investigation table) |
| **Task 9C** | Connect Wemos to Wi-Fi (configure station mode, join an AP, query network status) |
| **Task 10C** | Configure ESP8266 as a TCP server |
| **Task 10d** | Observe the HTTP request (`+IPD`) and send an HTTP response using the two-step `CIPSEND` |
| **Task 10E** | Node-RED as a second HTTP client of the STM32/ESP8266 HTTP server |

---

## Circuit Diagram

![Circuit Diagram](circuit.png)
