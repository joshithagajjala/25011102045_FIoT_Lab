#include <reg51.h>

sbit BTN = P3^2;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 120; j++);
    }
}

unsigned char check_button(void)
{
    if (BTN == 0)
    {
        delay_ms(20);
        if (BTN == 0)
        {
            while (BTN == 0);
            delay_ms(20);
            return 1;
        }
    }
    return 0;
}

void delay_check(unsigned int ms, unsigned char *mode)
{
    unsigned int i;
    for (i = 0; i < ms / 10; i++)
    {
        if (check_button())
        {
            *mode = (*mode % 4) + 1;
            return;
        }
        delay_ms(10);
    }
}

void main(void)
{
    unsigned char mode = 0;
    unsigned char step = 0;
    unsigned char count = 0;

    P1 = 0xFF;
    BTN = 1;

    while (mode == 0)
    {
        if (check_button())
        {
            mode = 1;
        }
    }

    while (1)
    {
        if (mode == 1)
        {
            switch (step % 4)
            {
                case 0:
                    P1 = 0xFE;
                    break;
                case 1:
                    P1 = 0xFD;
                    break;
                case 2:
                    P1 = 0xFB;
                    break;
                case 3:
                    P1 = 0xF7;
                    break;
            }
            step++;
            delay_check(300, &mode);
        }
        else if (mode == 2)
        {
            P1 = ~(count & 0x0F);
            count = (count + 1) & 0x0F;
            delay_check(300, &mode);
        }
        else if (mode == 3)
        {
            if (step % 2 == 0)
            {
                P1 = 0xF5;
            }
            else
            {
                P1 = 0xFA;
            }
            step++;
            delay_check(400, &mode);
        }
        else if (mode == 4)
        {
            if (step % 2 == 0)
            {
                P1 = 0xF0;
            }
            else
            {
                P1 = 0xFF;
            }
            step++;
            delay_check(400, &mode);
        }
    }
}
