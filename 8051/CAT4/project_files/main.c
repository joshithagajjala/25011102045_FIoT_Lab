#include <reg51.h>

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 120; j++);
    }
}

int keypad_scan(void)
{
    static const int keypad[4][4] =
    {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    static const unsigned char rowPin[4] = {7, 6, 5, 4};
    static const unsigned char colPin[4] = {3, 2, 1, 0};

    int row, col;

    for (row = 0; row < 4; row++)
    {
        P2 |= 0xF0;
        P2 &= ~(1 << rowPin[row]);

        for (col = 0; col < 4; col++)
        {
            if ((P2 & (1 << colPin[col])) == 0)
            {
                return keypad[row][col];
            }
        }
    }

    return -1;
}

void display_digit(unsigned char value)
{
    static const unsigned char seg_code[] = {
        0xC0,
        0xF9,
        0xA4,
        0xB0,
        0x99,
        0x92,
        0x82,
        0xF8,
        0x80,
        0x90
    };

    if (value >= '0' && value <= '9')
    {
        P0 = seg_code[value - '0'];
    }
}

void running_led(void)
{
    unsigned char i;
    for (i = 0; i < 4; i++)
    {
        P1 = ~(1 << i);
        delay_ms(200);
        if (keypad_scan() != -1)
        {
            return;
        }
    }
}

void binary_counter(void)
{
    unsigned char i;
    for (i = 0; i < 16; i++)
    {
        P1 = ~(i & 0x0F);
        delay_ms(250);
        if (keypad_scan() != -1)
        {
            return;
        }
    }
}

void alternate_led(void)
{
    P1 = 0xF5;
    delay_ms(300);
    if (keypad_scan() != -1)
    {
        return;
    }
    P1 = 0xFA;
    delay_ms(300);
}

void blink_all(void)
{
    P1 = 0xF0;
    delay_ms(300);
    if (keypad_scan() != -1)
    {
        return;
    }
    P1 = 0xFF;
    delay_ms(300);
}

void led_control(unsigned char key)
{
    switch (key)
    {
        case '1':
            P1 = 0xFE;
            break;
        case '2':
            P1 = 0xFD;
            break;
        case '3':
            P1 = 0xFB;
            break;
        case '4':
            P1 = 0xF7;
            break;
        case '5':
            running_led();
            break;
        case '6':
            alternate_led();
            break;
        case '7':
            binary_counter();
            break;
        case '8':
            blink_all();
            break;
        case '9':
            P1 = 0xFF;
            break;
        case '0':
            P0 = 0xC0;
            P1 = 0xFF;
            break;
    }
}

void main(void)
{
    int key;
    unsigned char current_key = '0';

    P0 = 0xFF;
    P1 = 0xFF;
    P2 = 0xFF;

    display_digit('0');

    while (1)
    {
        key = keypad_scan();
        if (key != -1)
        {
            current_key = (unsigned char)key;
            display_digit(current_key);
            delay_ms(200);
        }
        led_control(current_key);
    }
}
