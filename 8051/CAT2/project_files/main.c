#include <reg51.h>

// A simple software delay function
void delay(unsigned int msec) 
{
    unsigned int i, j;
    for (i = 0; i < msec; i++) {
        for (j = 0; j < 127; j++) {
            // This inner loop takes roughly 1 millisecond at 12 MHz
        }
    }
}

void main() 
{
    while(1) 
    {
        P1 = 0x00;      // Turn all Port 1 LEDs ON (Active-low)
        delay(500);     // Wait for roughly 500 milliseconds (half a second)
        
        P1 = 0xFF;      // Turn all Port 1 LEDs OFF
        delay(500);     // Wait for roughly 500 milliseconds
    }
}