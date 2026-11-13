# Continuous Lab Evaluation-6: Task B - UART Interactive Echo Terminal

## Description
Full-duplex UART serial receiver and echo terminal on 8051 that continuously polls for incoming serial bytes and immediately retransmits them to the host.

## Hardware Target
- **Microcontroller**: Atmel AT89S52 (8051 Architecture)
- **Clock Frequency**: 11.0592 MHz / 12 MHz Crystal

## Deliverables
- **Embedded C Source**: `uart_echo_terminal.c`
- **Startup Code**: `STARTUP.A51`
- **Keil Project**: `*.uvproj` / `*.uvopt`
- **Compiled Firmware**: `Objects/uart_echo_terminal.hex`
