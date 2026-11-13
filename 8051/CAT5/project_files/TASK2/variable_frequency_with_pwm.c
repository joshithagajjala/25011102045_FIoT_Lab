#include <reg52.h> 

unsigned char pwm_count = 0; 
unsigned char duty = 0;
unsigned char fade_count = 0;

unsigned char reload_high = 0xFE;
unsigned char reload_low = 0X33;

sbit LED = P1^0;

sbit OUTPUT = P1^1;

void timer0_ISR(void) interrupt 1 {
    TH0 = 0xFF;
    TL0 = 0xF7;

    if(pwm_count < duty) {
        LED = 1;
    } else {
        LED = 0;
    }

    pwm_count++;
    if(pwm_count >= 100) {
        pwm_count = 0;
        fade_count++;

        if(fade_count >= 10) {
            duty++;
            fade_count = 0;
            if(duty > 100) {
                duty = 0;
            }
        }
    }
}

int keypad_scan(void)
{
    static const int keypad[4][4] =
    {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, -1, -1, -1},
        {-1, -1, -1, -1}
    };

    static const unsigned char rowPin[4] = {7, 6, 5, 4};
    static const unsigned char colPin[4] = {3, 2, 1, 0};

    int row, col;


    for(row = 0; row < 4; row++)
    {
        // Set all rows HIGH
        P2 |= 0xF0;

        // Set current row LOW
        P2 &= ~(1 << rowPin[row]);

        // Check columns
        for(col = 0; col < 4; col++)
        {
            if((P2 & (1 << colPin[col])) == 0)
            {
                return keypad[row][col];
            }
        }
    }

    return -1;
}

void set_frequency(unsigned char mode)
{
    switch(mode)
    {
        case 1: // 100Hz
            reload_high = 0xEE;
            reload_low  = 0x00;
            break;

        case 2: // 500Hz
            reload_high = 0xFC;
            reload_low  = 0x66;
            break;

        case 3: // 1kHz
            reload_high = 0xFE;
            reload_low  = 0x33;
            break;

        case 4: // 2kHz
            reload_high = 0xFF;
            reload_low  = 0x1A;
            break;

        case 5: // 5kHz
            reload_high = 0xFF;
            reload_low  = 0xA4;
            break;

        case 6 : 
            reload_high = 0x00;
            reload_low  = 0x00;
            break;

        default:
            break;
    }
}

void timer1_ISR(void) interrupt 3 {
    TH1 = reload_high;
    TL1 = reload_low;

    OUTPUT = ~OUTPUT;
}


void main() {

    int key;

    TMOD = 0x11;
    TH0 = 0xFF;
    TL0 = 0xF7;

    TH1 = reload_high;
    TL1 = reload_low;
    
    ET0 = 1;
    ET1 = 1;
    EA = 1; 

    TR0 = 1;
    TR1 = 1;

    while(1) {
        key = keypad_scan();
        if(key != -1) {
            set_frequency(key);
        }
    }
}
