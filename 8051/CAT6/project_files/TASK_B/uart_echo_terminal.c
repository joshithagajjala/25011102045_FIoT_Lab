#include <reg51.h> 

unsigned char c;
void serial_interrupt(void) interrupt 4 {
    if(RI) {
        RI = 0; // Clear receive interrupt flag
        c = SBUF; // Read received character
        SBUF = c; // Echo back the received character
    }

    if(TI) {
        TI = 0; // Clear transmit interrupt flag
    }
}

void init() {
    SCON = 0x50; // Set serial mode to 8-bit UART, enable reception
    TMOD |= 0x20; // Set Timer1 in mode 2 (8-bit auto-reload)
    TH1 = 0xFD; // Set baud rate to 9600
    TR1 = 1; // Start Timer1
    ES = 1; // Enable serial interrupt
    EA = 1; // Enable global interrupts
}

void main() {
    init();

    while(1) {
        // Main loop can perform other tasks or enter low power mode
    }
}