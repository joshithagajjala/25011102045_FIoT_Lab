#include <reg51.h>

// A simple software delay function
void delay(unsigned int msec) 
{
    unsigned int i, j;
    for (i = 0; i < msec; i++) {
        for (j = 0; j < 127; j++) {
            // Software delay loop (~1 ms approximate delay; board clock: 11.0592 MHz)
        }
    }
}

void main() 
{
    while(1) 
    {
        P1 = 0x00;      // Drive Port 1 LOW (External LED OFF, active-high to GND)
        delay(500);     // Wait for roughly 500 milliseconds (half a second)
        
        P1 = 0xFF;      // Drive Port 1 HIGH (External LED ON, active-high to GND)
        delay(500);     // Wait for roughly 500 milliseconds
    }
}
