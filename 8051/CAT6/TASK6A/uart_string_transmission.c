#include <reg51.h> 

char message[] = "Hello from 8051\r\n";
unsigned char index = 0;

void serial_interrupt(void) interrupt 4 {
    if(TI) {
        TI = 0; // Clear transmit interrupt flag
        if(message[index] == '\0') {
            index = 0; // Reset index to repeat message continuously
        }
        SBUF = message[index++]; // Send next character 
    }
}

void main() {
    // Initialize serial communication
    SCON = 0x50; // Set serial mode to 8-bit UART, enable reception
    TMOD |= 0x20; // Set Timer1 in mode 2 (8-bit auto-reload)
    TH1 = 0xFD; // Set baud rate to 9600
    TR1 = 1; // Start Timer1
    ES = 1; // Enable serial interrupt
    EA = 1; // Enable global interrupts

    SBUF = message[index++]; // Start sending the first character
    while(1) {
        // Main loop can perform other tasks or enter low power mode
    }
}
