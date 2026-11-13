#include <reg52.h>

sbit LED = P1^1;

unsigned char reload_high = 0xFC;
unsigned char reload_low = 0x66;

/* Timer 1 Interrupt - Square wave generation (10101010...) */
void timer1_ISR(void) interrupt 3
{
    TH1 = reload_high;
    TL1 = reload_low;

    LED = ~LED;
}

/* Timer 1 Initialization */
void timer1_init(void)
{
    TMOD &= 0x0F;   // Clear Timer 1 bits
    TMOD |= 0x10;   // Timer 1 Mode 1 (16-bit)

    TH1 = reload_high;
    TL1 = reload_low;

    ET1 = 1;        // Enable Timer 1 interrupt
    EA = 1;         // Enable global interrupts
    TR1 = 1;        // Start Timer 1
}

void main(void)
{
    LED = 0;

    timer1_init();

    while(1)
    {
        // Waveform generated completely by Timer 1 ISR
    }
}
