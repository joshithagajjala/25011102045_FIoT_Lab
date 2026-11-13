#include <reg52.h>

sbit LED = P1^1;

unsigned char count = 0;

/* Timer 1 Interrupt */
void timer1_ISR(void) interrupt 3
{
    // Reload Timer 1 values
    TH1 = 0xFC;
    TL1 = 0x66;

    // Generate pulse train: 10000000
    if(count == 0)
    {
        LED = 1;
    }
    else
    {
        LED = 0;
    }

    count++;

    if(count == 8)
    {
        count = 0;
    }
}


/* Timer 1 Initialization */
void timer1_init(void)
{
    TMOD &= 0x0F;   // Clear Timer 1 bits
    TMOD |= 0x10;   // Timer 1 Mode 1 (16-bit)

    TH1 = 0xFC;
    TL1 = 0x66;

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
        // Main loop remains empty
        // Waveform generated completely by ISR
    }
}