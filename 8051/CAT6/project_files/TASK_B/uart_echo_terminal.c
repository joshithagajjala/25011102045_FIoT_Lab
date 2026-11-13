
#include <reg51.h>

#define BUFFER_SIZE 32

// RX Circular Buffer
volatile unsigned char data rxBuffer[BUFFER_SIZE];
volatile unsigned char rxHead = 0;
volatile unsigned char rxTail = 0;
volatile unsigned char rxOverflow = 0;

// TX status
volatile bit txBusy = 0;

// UART Serial Interrupt
void serial_interrupt(void) interrupt 4
{
    unsigned char nextHead;
    unsigned char received;

    // Receive interrupt
    if (RI)
    {
        received = SBUF;
        RI = 0;

        nextHead = (rxHead + 1) % BUFFER_SIZE;

        if (nextHead != rxTail)
        {
            rxBuffer[rxHead] = received;
            rxHead = nextHead;
        }
        else
        {
            rxOverflow = 1;
        }
    }

    // Transmit interrupt
    if (TI)
    {
        TI = 0;
        txBusy = 0;
    }
}

// UART Initialization
void uart_init(void)
{
    EA = 0;

    PCON &= 0x7F;  // SMOD = 0

    TMOD = (TMOD & 0x0F) | 0x20;

    TH1 = 0xFD;
    TL1 = 0xFD;

    SCON = 0x50;

    RI = 0;
    TI = 0;

    rxHead = 0;
    rxTail = 0;
    rxOverflow = 0;
    txBusy = 0;

    TR1 = 1;

    ES = 1;
    EA = 1;
}

// Check if received data is available
bit uart_available(void)
{
    return (rxHead != rxTail);
}

// Read one character from ring buffer
unsigned char uart_read(void)
{
    unsigned char received;

    while (!uart_available());

    received = rxBuffer[rxTail];
    rxTail = (rxTail + 1) % BUFFER_SIZE;

    return received;
}

// Transmit one character
void uart_send(unsigned char ch)
{
    while (txBusy);

    txBusy = 1;
    SBUF = ch;
}

// Main Program - Echo Task
void main(void)
{
    unsigned char c;

    uart_init();

    while (1)
    {
        if (uart_available())
        {
            c = uart_read();
            uart_send(c);
        }
    }
}
